/*******************************************************************************
 * Copyright (c) 2005, 2007 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>
#include <math.h>
#define NumStages 4
#define numSeasons 3
#define MDParIrrigationCropFileName "CropParameterFileName"

//#define MDParIrrigationCropFileName "CropParameterFileName"

typedef struct {
    int ID;
    int DW_ID;
    char cropName[80];
    char cropDistrFileName[80];
    int cropIsRice;
    float cropSeasLength[NumStages];
    float cropKc[NumStages-1];
    float cropRootingDepth;
    float cropDepletionFactor;
    float cropLeachReq;
} MDIrrigatedCrop;


static MDIrrigatedCrop *_MDirrigCropStruct = (MDIrrigatedCrop *) NULL;
static int *_MDInCropFractionID = (int *) NULL;
static int *_MDOutCropDeficitID = (int *) NULL;
static int _MDAbstrVolume;
static int _MDActEvaptrsID;
 //Input
static int _MDInPrecipID, _MDInRefETPID, _MDInIrrAreaID, _MDInIrrIntensityID, _MDInIrrEfficiencyID;

static int _MDGrowingSeason1ID, _MDGrowingSeason2ID, _MDGrowingSeason3ID;
static int _MDInFldCapaID, _MDInWltPntID;
static int _MDNumberOfIrrCrops;
static int _MDNumGS;

static int _MDGrowingSeasonStartCalcID;
static int _MDGrowingSeasonEndCalcID;
static int _MDIrrKonstantKc;
//Output


// The following Outputs are given in mm/d and related to the total cell area!! Module is called from MDRunoff!
static int _MDOutNetIrrDemandID, _MDOutIrrigationDrainageID, _MDOutTotalCropETPDepthID;
static int _MDOutGrossIrrDemandID = CMfailed;
static int _MDOutIrrAreaSMChangeID;
static int _MDInAirTemperatureID;
static int _MDOutActIrrAreaID;		  	
static void _MDIrrigationDummy (int itemID) { MFVarSetFloat (_MDOutGrossIrrDemandID,itemID,0.0); }


//Parameters
static float _MDParIrrDailyPercolation;
static float _MDParIrrWaterLayerDepth;

static const char *CropParameterFileName;

static int   getTotalSeasonLength(const MDIrrigatedCrop *);
static int   getDaysSincePlanting(int, int [],int,const MDIrrigatedCrop *);
static int   getCropStage(const MDIrrigatedCrop *, int);
static float getCropKc(const MDIrrigatedCrop *, int, int);
static float getCurCropRootingDepth(MDIrrigatedCrop *, int);
static float getEffectivePrecipitation(float);
static float getCorrDeplFactor(const MDIrrigatedCrop *, float);
static int   readCropParameters(const char *);
static void  printCrops(const MDIrrigatedCrop *);
static int getNumGrowingSeasons(float);

static float getIrrGrossWaterDemand(float, float);
static float irrAreaFraction;
static float numGS;

static void _MDIrrigationWater(int itemID) {
	int i;
 	//Output:
	float totalGrossIrrDemand=0;
	float totalNetIrrDemand=0;
	float totalIrrPercolation=0;
	// float totalCropETP=0;
	//Input
	//
	
	
	

// if (MFVarTestMissingVal(_MDInRefETPID,  itemID)) printf ("Missing RefETP\n");
// 	    if (MFVarTestMissingVal(_MDInIrrAreaID, itemID)) printf ("Missing area\n");
// 	    if (MFVarTestMissingVal(_MDInIrrEfficiencyID,itemID)) printf ("Missing eff\n");
// 	    if (MFVarTestMissingVal(_MDInPrecipID,itemID)) printf ("Missing precip\n");
// 	   
// 	    if (MFVarTestMissingVal(_MDGrowingSeason1ID,itemID)) printf ("Missing gs1\n");
// 	    if (MFVarTestMissingVal(_MDGrowingSeason2ID,itemID)) printf ("Missing gs2\n");
// 	    if (MFVarTestMissingVal(_MDGrowingSeason3ID,itemID)) printf ("Missing gs3\n");
// 	    if (MFVarTestMissingVal(_MDInWltPntID,  itemID)) printf ("Missing wp\n");
// 	    if (MFVarTestMissingVal(_MDInFldCapaID, itemID)) printf ("Missing fc\n");
// 	    if (MFVarTestMissingVal(_MDInIrrIntensityID, itemID)) printf ("Missing int \n");
// 	 

	if (MFVarTestMissingVal(_MDInRefETPID,  itemID) ||
	    MFVarTestMissingVal(_MDInIrrAreaID, itemID)|| 
	    MFVarTestMissingVal(_MDInIrrEfficiencyID,itemID)||
	    MFVarTestMissingVal(_MDInPrecipID,itemID)||
	   
	    MFVarTestMissingVal(_MDGrowingSeason1ID,itemID)|| 
	    MFVarTestMissingVal(_MDGrowingSeason2ID,itemID)|| 
	    MFVarTestMissingVal(_MDGrowingSeason3ID,itemID)||
	    MFVarTestMissingVal(_MDInWltPntID,  itemID)||
	    MFVarTestMissingVal(_MDInFldCapaID, itemID)||
	    MFVarTestMissingVal(_MDInIrrIntensityID, itemID)) {	
		MFVarSetFloat(_MDOutGrossIrrDemandID,itemID,0.0);
		return;
	}
	    
	for (i = 0; i < _MDNumberOfIrrCrops; i++) {
	if (MFVarTestMissingVal(_MDInCropFractionID[i],itemID)){MFVarSetFloat(_MDInCropFractionID[i],itemID,0.0);}
	}
	
	float irrEffeciency  = MFVarGetFloat(_MDInIrrEfficiencyID, itemID);
	float dailyPrecip    = MFVarGetFloat(_MDInPrecipID,itemID);
	float dailyEffPrecip = getEffectivePrecipitation(dailyPrecip);
	float refETP         = MFVarGetFloat(_MDInRefETPID,itemID);  
	irrAreaFraction = MFVarGetFloat(_MDInIrrAreaID, itemID);
	int seasStart[3];
		
	seasStart[0]= MFVarGetFloat(_MDGrowingSeason1ID,itemID); 
	seasStart[1]= MFVarGetFloat(_MDGrowingSeason2ID,itemID); 
	seasStart[2]= MFVarGetFloat(_MDGrowingSeason3ID,itemID); 
	numGS=MFVarGetFloat(_MDNumGS,itemID);
	int doy = MFDateGetDayOfYear ();
	float airTemp = MFVarGetFloat(_MDInAirTemperatureID,itemID);
	int doYStartGS=0;
	float actETP=MFVarGetFloat(_MDActEvaptrsID,itemID);
 	
	if (airTemp > 5.0) {
		if (actETP > 0.5 * refETP) {
 		doYStartGS = doy;
 		}
	}

	if (irrAreaFraction==0.0) MFVarSetFloat(_MDOutActIrrAreaID, itemID,0.0);
	
	if (irrAreaFraction > 0.0) {
	 	float dailyPercolation    = _MDParIrrDailyPercolation;
	 	float wltPnt              = MFVarGetFloat (_MDInWltPntID,  itemID);
		float fldCap              = MFVarGetFloat (_MDInFldCapaID, itemID);
		if (fldCap == 0.0) {
			fldCap=0.35;
			wltPnt=0.2;
		}
		float cropDepletionFactor;
		float meanSMChange;
		float irrIntensity        = MFVarGetFloat (_MDInIrrIntensityID, itemID) / 100.0;
		if (irrIntensity < 1.5 && irrIntensity > 1.0) irrIntensity = 1.0;
		if (irrIntensity < 2.5 && irrIntensity > 2.0) irrIntensity = 2.0;
		//printf ("Intens %f\n", irrIntensity); 
 		int daysSincePlanted;
		float totAvlWater,readAvlWater, curDepl, prevSoilMstDepl;
		float cropWR, totalCropETP;
		float sumOfCropFractions=0;
		float cropCoeff;
		int stage;
		int curDay = MFDateGetDayOfYear();
		
		float smChange;
		sumOfCropFractions=0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {sumOfCropFractions += MFVarGetFloat(_MDInCropFractionID[i],itemID);	}
		if (sumOfCropFractions==0) { // No Cropdata for irrigated cell: default to some cereal crop
			sumOfCropFractions=1.0;
		 
			MFVarSetFloat(_MDInCropFractionID[1],itemID, 1.0);
		}

	
		float relCropFraction;
		float	netIrrDemand    = 0;
		float actuallyIrrArea=0;
		float deepPercolation = 0;
		meanSMChange=0;totalCropETP=0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {
			netIrrDemand=0;cropWR=0;deepPercolation=0;smChange=0;
			float curCropFraction = MFVarGetFloat(_MDInCropFractionID[i],itemID);
			relCropFraction   = curCropFraction / sumOfCropFractions;
			int   numGrowingSeasons = getNumGrowingSeasons (irrIntensity);
			if (relCropFraction > 0) {
				daysSincePlanted = getDaysSincePlanting(curDay, seasStart,numGrowingSeasons,&_MDirrigCropStruct[i]);
				if (daysSincePlanted > 0) {
					actuallyIrrArea+=relCropFraction*irrAreaFraction;
					prevSoilMstDepl = MFVarGetFloat(_MDOutCropDeficitID[i],itemID);
					stage = getCropStage(&_MDirrigCropStruct[i], daysSincePlanted);
					cropCoeff =getCropKc(&_MDirrigCropStruct[i], daysSincePlanted, stage);
					cropWR = refETP * cropCoeff;
					float rootDepth = getCurCropRootingDepth (&_MDirrigCropStruct[i],daysSincePlanted);
					rootDepth = 250;
					cropDepletionFactor=getCorrDeplFactor(&_MDirrigCropStruct[i], cropWR);
					if (_MDirrigCropStruct[i].cropIsRice==1) {
						smChange=0;
						curDepl =0;
						netIrrDemand =dailyPercolation+ cropWR-dailyEffPrecip;
						deepPercolation=dailyPercolation;
						if (netIrrDemand <0) {
							deepPercolation=dailyEffPrecip - cropWR;
							netIrrDemand =0;;
						}
					}
					else {
						totAvlWater  = (fldCap  -wltPnt) * rootDepth;	
						readAvlWater = totAvlWater * cropDepletionFactor;
						if (daysSincePlanted ==1) {
							prevSoilMstDepl=readAvlWater;
						}
						smChange=prevSoilMstDepl;
						curDepl  = prevSoilMstDepl - dailyEffPrecip + cropWR;
						smChange=curDepl;
						if (curDepl < 0) {	
							curDepl = 0;
							deepPercolation=dailyEffPrecip - prevSoilMstDepl -cropWR;
						}
						if (curDepl >= totAvlWater) curDepl =totAvlWater;
						if (curDepl >= readAvlWater) {
							netIrrDemand=curDepl;
							curDepl=0;
						}
						smChange=prevSoilMstDepl-curDepl;
					}
					MFVarSetFloat(_MDOutCropDeficitID[i],itemID,curDepl);
				}
				else { //Crop is not currently growns
					netIrrDemand=0;
					cropWR=0;
					smChange=0;
					deepPercolation=dailyEffPrecip;
				}
				totalNetIrrDemand+=netIrrDemand*relCropFraction;
				totalCropETP+=cropWR*relCropFraction;
				meanSMChange+=smChange*relCropFraction;
				totalIrrPercolation+=deepPercolation*relCropFraction;
			}
			MFVarSetFloat(_MDOutActIrrAreaID, itemID,actuallyIrrArea);
		}
  		actuallyIrrArea=actuallyIrrArea * irrIntensity/ceil(irrIntensity);
		MFVarSetFloat(_MDOutActIrrAreaID, itemID,actuallyIrrArea);

		float totGrossDemand = getIrrGrossWaterDemand(totalNetIrrDemand, irrEffeciency);
		float loss = totGrossDemand-totalNetIrrDemand+(dailyPrecip-dailyEffPrecip);
		float OUT =totalCropETP+totalIrrPercolation+meanSMChange+loss;
		float IN = totGrossDemand+dailyPrecip;
		if (fabs(IN-OUT) > 0.1) printf ("WaterBalance in MDIrrigation!!! IN %f OUT %f BALANCE %f LOSS %f %i DEMAND %f %i EffPrecip %f    \n", IN, OUT, IN-OUT, loss, itemID, totGrossDemand, itemID, dailyEffPrecip);
	 		
// convert all outputs to cell Area
		totalNetIrrDemand = totalNetIrrDemand* actuallyIrrArea;
		totGrossDemand = totGrossDemand* actuallyIrrArea;
		totalCropETP=totalCropETP * actuallyIrrArea;
		totalIrrPercolation=(totalIrrPercolation+loss)*actuallyIrrArea;
		if (totGrossDemand < 0 ) {
			CMmsgPrint(CMmsgUsrError, "Net %f GrossDemand %f irrEffeciency %f  Area %f, ceilIrrInten %f \n",totalNetIrrDemand, totalGrossIrrDemand,irrEffeciency,irrAreaFraction, ceil(irrIntensity));
		}
	//	if (irrIntensity < 0.5) printf ("IrrIntensity lt 0.5! %f \n", irrIntensity);
		if (isnan(totGrossDemand))
			CMmsgPrint(CMmsgUsrError,"GrossDEmand  nan! LAT %f LON %f irrAreaFraction %f %i\n", MFModelGetLatitude(itemID), MFModelGetLongitude(itemID), irrAreaFraction,itemID);
		if (totalGrossIrrDemand < 0) {	
			CMmsgPrint (CMmsgAppError,"Error in Irrigation, Demand , 0: LON % LAT % Intensity % Efficiency %f FC% WP%\n ",MFModelGetLongitude(itemID), MFModelGetLatitude(itemID), irrIntensity, irrEffeciency, fldCap, wltPnt);
		}
		if (totalIrrPercolation < 0){
			CMmsgPrint (CMmsgUsrError, "Perco %f demand %f IrrArea %f itemID %i\n", totalIrrPercolation, totalGrossIrrDemand, irrAreaFraction,itemID);
		}
		MFVarSetFloat(_MDOutIrrAreaSMChangeID,    itemID, meanSMChange);
		MFVarSetFloat(_MDOutNetIrrDemandID,       itemID, totalNetIrrDemand);
		MFVarSetFloat(_MDOutGrossIrrDemandID,     itemID, totGrossDemand);
		MFVarSetFloat(_MDOutIrrigationDrainageID, itemID, totalIrrPercolation);
		MFVarSetFloat(_MDOutTotalCropETPDepthID,  itemID, totalCropETP);	
		MFVarSetFloat(_MDAbstrVolume,             itemID, totalGrossIrrDemand / 1000  * MFModelGetArea(itemID)); // MFModelGetArea is neuerdings in m2 !!!!!!!!!!!!!!! 
	}
	else { // cell is not irrigated at all
		MFVarSetFloat(_MDOutIrrAreaSMChangeID,    itemID, 0.0);
 		MFVarSetFloat(_MDOutNetIrrDemandID,       itemID, 0.0);
		MFVarSetFloat(_MDOutGrossIrrDemandID,     itemID, 0.0);
		MFVarSetFloat(_MDOutIrrigationDrainageID, itemID, 0.0);
		MFVarSetFloat(_MDOutTotalCropETPDepthID,  itemID, 0.0);	
		MFVarSetFloat(_MDAbstrVolume,             itemID, 0.0); 
	}
}

enum { MDhelp, MDinput, MDnone, MDcalc };
	 
int MDIrrigationWaterDef() {
	
	int i;
   if (_MDOutGrossIrrDemandID != CMfailed)	return (_MDOutGrossIrrDemandID);
	float par;
	int optID = MDinput;
	const char *optStr, *optName = "Irrigation";
	const char *options [] = { MDHelpStr, MDInputStr, "none", MDCalculateStr, (char *) NULL };

	if (_MDOutGrossIrrDemandID != CMfailed) return (_MDOutGrossIrrDemandID);

	MFDefEntering("Irrigation");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
   
	switch (optID) {
 		case MDinput:	 _MDOutGrossIrrDemandID = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand, "mm", MFInput,  MFFlux, false);break;
		case MDnone:
			if	((_MDOutGrossIrrDemandID = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand,         "mm", MFOutput, MFFlux, false)) == CMfailed)
				return (CMfailed);
			_MDOutGrossIrrDemandID      = MFVarSetFunction (_MDOutGrossIrrDemandID,_MDIrrigationDummy); 
			break;
		case MDcalc:
			if (((optStr = MFOptionGet(MDParIrrDailyPercolationRate)) != (char *) NULL) && (sscanf(optStr, "%f", &par) == 1))
				_MDParIrrDailyPercolation = par;
			if (((optStr = MFOptionGet(MDParIrrRiceWaterLayerDepth))  != (char *) NULL) && (sscanf(optStr, "%f", &par) == 1))
				_MDParIrrWaterLayerDepth = par;

			if (((optStr = MFOptionGet(MDIrrKonstantKc))  != (char *) NULL) && (sscanf(optStr, "%f", &par) == 1))
				_MDIrrKonstantKc = par;
 
 			//if Data for Penman is avaiable"
 			if ((_MDInRefETPID  = MFVarGetID (MDVarPotEvapotrans, "DoY", MFOutput,  MFState, false)) == CMfailed) return (CMfailed);
		
 			if  ((_MDInRefETPID      = MFVarGetID (MDVarReferenceEvapotranspiration,  "mm",   MFInput, MFState,  false)) == CMfailed) return (CMfailed);
			if ((_MDInPrecipID        = MDPrecipitationDef ())     == CMfailed) return (CMfailed);
			//if only TMP and PRE are available:
	 		if ((_MDInRefETPID        = MDHamonReferenceETPDef ()) == CMfailed) return CMfailed;
	 		 
			if ((_MDInWltPntID        = MFVarGetID(MDVarWiltingPoint,            "mm/m", MFInput,  MFState, false)) == CMfailed) return (CMfailed);	
			if ((_MDInFldCapaID       = MFVarGetID (MDVarFieldCapacity,          "mm/m",   MFInput,  MFState, false)) == CMfailed) return (CMfailed);	
			if ((_MDGrowingSeason1ID  = MFVarGetID (MDVarIrrGrowingSeason1Start, "DoY",  MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDGrowingSeason2ID  = MFVarGetID (MDVarIrrGrowingSeason2Start, "DoY",  MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			
			
		 
			if ((_MDGrowingSeason3ID  = MFVarGetID (MDVarIrrGrowingSeason3Start, "DoY",  MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDGrowingSeasonStartCalcID  = MFVarGetID (MDVarStartGrowingSeasonCalc, "DoY",  MFOutput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDGrowingSeasonEndCalcID  = MFVarGetID (MDVarEndGrowingSeasonCalc, "DoY",  MFOutput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDInAirTemperatureID  = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDInIrrIntensityID  = MFVarGetID (MDVarIrrIntensity,           "-",    MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDInIrrEfficiencyID = MFVarGetID (MDVarIrrEfficiency,          "-",    MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDInIrrAreaID       = MFVarGetID (MDVarIrrAreaFraction,        "%",    MFInput,  MFState, false)) == CMfailed) return (CMfailed);
			if ((optStr = MFOptionGet (MDParIrrigationCropFileName)) != (char *) NULL) CropParameterFileName = optStr;
  			if (readCropParameters (CropParameterFileName) == CMfailed) {
				CMmsgPrint(CMmsgUsrError,"Error reading crop parameter file   : %s \n", CropParameterFileName);
				return CMfailed;
			}
			//Ouputs
			if ((_MDAbstrVolume             = MFVarGetID (MDVarIrrGrossIrrigationWaterAbstractionVol, "km3", MFOutput, MFState, true))  == CMfailed) return (CMfailed);
 			if ((_MDOutGrossIrrDemandID     = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand,          "mm", MFOutput, MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDOutNetIrrDemandID       = MFVarGetID (MDVarIrrNetIrrigationWaterDemand,            "mm", MFOutput, MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDOutIrrAreaSMChangeID    = MFVarGetID (MDVarIrrSoilMoistureChange,                  "mm", MFOutput, MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDOutIrrigationDrainageID = MFVarGetID (MDVarIrrPercolationWater,                    "mm", MFOutput, MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDOutTotalCropETPDepthID  = MFVarGetID (MDVarIrrCropETP,                             "mm", MFOutput, MFState, false)) == CMfailed) return (CMfailed);
			if ((_MDOutActIrrAreaID	= MFVarGetID (MDVarActuallyIrrArea,			    "-", MFOutput,MFState,false))==CMfailed)return (CMfailed);
 			MDParNumberOfCrops = _MDNumberOfIrrCrops;
			CMmsgPrint(CMmsgInfo,"Number of crops read: %i \n", _MDNumberOfIrrCrops);
			for (i=0;i<_MDNumberOfIrrCrops;i++) {
 			 	char frac []= { "CropFraction_    " };
				char def [] = { "CropSMDef_    " };
				sprintf (frac + 13,"%02d", i + 1);
				sprintf (def  + 10,"%02d", i + 1);
		
				//Input Fraction of crop type per cell
				if (CMfailed == (_MDInCropFractionID[i] = MFVarGetID (frac,"mm", MFInput,MFState,false))) {
					CMmsgPrint (CMmsgUsrError, "CMfailed in MDInCropFractionID \n");
					return CMfailed;
				}
				//Output Soil Moisture Deficit per croptype
				if ((_MDOutCropDeficitID[i]=MFVarGetID (def,"mm", MFOutput,MFState,true))==CMfailed) {
					CMmsgPrint (CMmsgUsrError,"MFFAult in MDCropDeficit\n");
					return CMfailed;
				}
			}
			_MDOutGrossIrrDemandID = MFVarSetFunction (_MDOutGrossIrrDemandID,_MDIrrigationWater);
			//break;
	}
	MFDefLeaving("Irrigation");
	return (_MDOutGrossIrrDemandID);
}

static int getNumGrowingSeasons(float irrIntensity){
	
	return ceil(irrIntensity);
	
}
static float getIrrGrossWaterDemand(float netIrrDemand, float IrrEfficiency) {
	if (IrrEfficiency <= 0) { // no data, set to average value		
		IrrEfficiency=38;
	} 
	return netIrrDemand / (IrrEfficiency/100);
}

static float getCorrDeplFactor(const MDIrrigatedCrop * pIrrCrop, float dailyETP) {
	float cropdeplFactor = pIrrCrop->cropDepletionFactor + 0.04 * (5 - dailyETP);
    if (cropdeplFactor <= 0.1) cropdeplFactor = 0.1;
	if (cropdeplFactor >= 0.8) cropdeplFactor = 0.8;
	return cropdeplFactor;
}

static float getEffectivePrecipitation(float dailyPrecip) {
	float effPrecip;
	
	if (dailyPrecip == 0) effPrecip = 0;
	if (dailyPrecip < 8.3)
		return dailyPrecip * (4.17 - 0.2 * dailyPrecip) / 4.17;
	else
		effPrecip = dailyPrecip * .1 + 4.17;
	if (dailyPrecip < 0.2) effPrecip = 0;
	return effPrecip;
}

static float getCurCropRootingDepth(MDIrrigatedCrop * pIrrCrop, int dayssinceplanted) {
	float rootDepth;
	float totalSeasonLenth;
	totalSeasonLenth =pIrrCrop->cropSeasLength[0] +	pIrrCrop->cropSeasLength[1] +	pIrrCrop->cropSeasLength[2] + pIrrCrop->cropSeasLength[3];
    rootDepth= pIrrCrop->cropRootingDepth *( 0.5 + 0.5 * sin(3.03 *   (dayssinceplanted  /  totalSeasonLenth) - 1.47));
	if (rootDepth > 2)		CMmsgPrint (CMmsgDebug, "RootDepth correct ?? %f \n",rootDepth);
 	if (rootDepth <0.15) rootDepth =.15;
	return rootDepth;
}

static int getCropStage(const MDIrrigatedCrop * pIrrCrop, int daysSincePlanted) {
	int stage = 0;
	float totalSeasonLenth;
	totalSeasonLenth =
	pIrrCrop->cropSeasLength[0] +
	pIrrCrop->cropSeasLength[1] +
	pIrrCrop->cropSeasLength[2] + pIrrCrop->cropSeasLength[3];
    if (daysSincePlanted <= totalSeasonLenth)
	stage = 4;

    if (daysSincePlanted <=
	pIrrCrop->cropSeasLength[0] +
	pIrrCrop->cropSeasLength[1] + pIrrCrop->cropSeasLength[2])
	stage = 3;

    if ((daysSincePlanted <= pIrrCrop->cropSeasLength[0] + pIrrCrop->cropSeasLength[1]))
	stage = 2;

    if (daysSincePlanted <= pIrrCrop->cropSeasLength[0])
	stage = 1;

if (stage ==0 && daysSincePlanted > 0) printf ("Stagexx =0? st0 %f st1 %f st1 %f st2 %f DSP %i TOT %f  \n",  pIrrCrop->cropSeasLength[0], pIrrCrop->cropSeasLength[1],pIrrCrop->cropSeasLength[2], pIrrCrop->cropSeasLength[3] ,daysSincePlanted, totalSeasonLenth);
     return stage;
		if (stage >4)	CMmsgPrint (CMmsgDebug, "stage corrdct ?? %i \n",stage);
}

static float getCropKc(const MDIrrigatedCrop * pIrrCrop, int daysSincePlanted, int curCropStage)
{
	float kc;
if(_MDIrrKonstantKc==1)return 1.0;
   //Returns kc depending on the current stage of the growing season
	if (curCropStage == 0) kc = 0.0;		//crop is not currently grown
	if (curCropStage == 1) kc = pIrrCrop->cropKc[0];
	if (curCropStage == 2) {
		int daysInStage = (daysSincePlanted - pIrrCrop->cropSeasLength[0]);
		kc = pIrrCrop->cropKc[0] + (daysInStage /  pIrrCrop->cropSeasLength[1])*(pIrrCrop->cropKc[1]-pIrrCrop->cropKc[0]);
	} 
	if (curCropStage == 3) kc = pIrrCrop->cropKc[1];
	if (curCropStage == 4) {
		int daysInStage4 = (daysSincePlanted - (pIrrCrop->cropSeasLength[0] +  pIrrCrop->cropSeasLength[1] + pIrrCrop->cropSeasLength[2]));
		//kc = pIrrCrop->cropKc[2] -	    (daysInStage4 / pIrrCrop->cropSeasLength[3]) * abs(pIrrCrop->cropKc[3] - pIrrCrop->cropSeasLength[2]);
		kc=pIrrCrop->cropKc[1]+ daysInStage4/  pIrrCrop->cropSeasLength[3] *(pIrrCrop->cropKc[2]-pIrrCrop->cropKc[1]);
		//printf ("Len3 %f kc3 %f daysin4 %i KC %f \n" , pIrrCrop->cropSeasLength[3], pIrrCrop->cropKc[3],daysInStage4,kc);
	}
	if (kc >1.5 )	CMmsgPrint (CMmsgDebug, "kc korrect ?? kc stage dayssinceplanted  kc0 kc1 season0length %f %i %i %f %f %f \n",kc, curCropStage, daysSincePlanted, pIrrCrop->cropKc[0],pIrrCrop->cropKc[1], pIrrCrop->cropSeasLength[0]);
 	return kc;
}

static int getDaysSincePlanting(int DayOfYearModel, int DayOfYearPlanting[numSeasons],int NumGrowingSeasons,const MDIrrigatedCrop * pIrrCrop) {
	int ret=-888;
	float totalSeasonLenth;

	totalSeasonLenth =
	pIrrCrop->cropSeasLength[0] +
	pIrrCrop->cropSeasLength[1] +
	pIrrCrop->cropSeasLength[2] + pIrrCrop->cropSeasLength[3];
	int dayssinceplanted ;	//Default> crop is not grown!
	int i;
	for (i = 0; i < NumGrowingSeasons; i++) {
		dayssinceplanted = DayOfYearModel - DayOfYearPlanting[i];
		if (dayssinceplanted < 0)  dayssinceplanted = 365 + (DayOfYearModel-DayOfYearPlanting[i]);
	   
		if (dayssinceplanted  < totalSeasonLenth) ret = dayssinceplanted;
	}
	if (ret >totalSeasonLenth)	CMmsgPrint (CMmsgDebug, "dayssinceplantedkorrect ?? %i %i \n",ret, DayOfYearModel);
	return ret;
}

static int readCropParameters(const char *filename) {
	FILE *inputCropFile;
	int i = 0;
	if ((inputCropFile = fopen(filename, "r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgUsrError,"Crop Parameter file could not be opned, filename: %s\n", filename);
	
		return CMfailed;
	}
	else {
		char buffer[512];
		//read headings..
		// the right thing would be to put it in a while loop and wait for
		// new line, but I assume that the header is not longer than the buffer size.
		fgets (buffer,sizeof (buffer),inputCropFile);
		while (feof(inputCropFile) == 0) {
			_MDirrigCropStruct =  (MDIrrigatedCrop *) realloc(_MDirrigCropStruct,(i +	 1) *	sizeof(MDIrrigatedCrop));
			_MDInCropFractionID  =(int *) realloc(_MDInCropFractionID,  (i + 1) * sizeof(int));
			_MDOutCropDeficitID 	=(int *) realloc(_MDOutCropDeficitID,(i+1) *sizeof(int));	
			fscanf(inputCropFile, "%i" "%i" "%s" "%s" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f" ,
		       &_MDirrigCropStruct[i].ID,
		       &_MDirrigCropStruct[i].DW_ID,
		       _MDirrigCropStruct[i].cropName,
		       _MDirrigCropStruct[i].cropDistrFileName,
		       &_MDirrigCropStruct[i].cropKc[0],
		       &_MDirrigCropStruct[i].cropKc[1],
		       &_MDirrigCropStruct[i].cropKc[2],
		       &_MDirrigCropStruct[i].cropSeasLength[0],
		       &_MDirrigCropStruct[i].cropSeasLength[1],
		       &_MDirrigCropStruct[i].cropSeasLength[2],
		       &_MDirrigCropStruct[i].cropSeasLength[3],
		       &_MDirrigCropStruct[i].cropRootingDepth,
		       &_MDirrigCropStruct[i].cropDepletionFactor);
				
			if (strcmp(_MDirrigCropStruct[i].cropName , "Rice")==0) _MDirrigCropStruct[i].cropIsRice = 1;
			if (strcmp(_MDirrigCropStruct[i].cropName , "Rice")!=0) _MDirrigCropStruct[i].cropIsRice = 0;
				
			printCrops(&_MDirrigCropStruct[i]);
			i += 1;

			 
		}
	}
	CMmsgPrint(CMmsgDebug,"Number of crops read: %i \n", i - 1);
	_MDNumberOfIrrCrops = i - 1;	
	return CMsucceeded;
}

static void printCrops(const MDIrrigatedCrop * curCrop) {
	
	
	CMmsgPrint(CMmsgDebug,"==++++++++++================  %s ===========================", curCrop->cropName);
	CMmsgPrint(CMmsgDebug,"CurrentCropID %i \n", curCrop->ID);
	CMmsgPrint(CMmsgDebug,"CropName %s	\n ", curCrop->cropName);
	CMmsgPrint(CMmsgDebug,"kc  %f %f %f \n", curCrop->cropKc[0], curCrop->cropKc[1], curCrop->cropKc[2]);
	CMmsgPrint(CMmsgDebug,"Length./Total . %f %f %f %f %i \n",
			curCrop->cropSeasLength[0],
			curCrop->cropSeasLength[1],
			curCrop->cropSeasLength[2],
			curCrop->cropSeasLength[3],
			getTotalSeasonLength(curCrop));
	CMmsgPrint(CMmsgDebug,"RootingDepth %f \n", curCrop->cropRootingDepth);
	CMmsgPrint(CMmsgDebug,"DepleationFactors %f \n", curCrop->cropDepletionFactor);
	CMmsgPrint(CMmsgDebug,"IsRice %i \n", curCrop->cropIsRice);
	fflush(stdout);
}

static int getTotalSeasonLength(const MDIrrigatedCrop * pIrrCrop) {
	return pIrrCrop->cropSeasLength[0] + pIrrCrop->cropSeasLength[1] +
	pIrrCrop->cropSeasLength[2] + pIrrCrop->cropSeasLength[3];
}
