/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDIrrigation.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#define NumStages 4
#define numSeasons 3
#define MDParIrrigationCropFileName "CropParameterFileName"
 
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

//Input
static int _MDInIrrAreaFracID          = MFUnset;
static int _MDInIrrRefEvapotransID     = MFUnset;
static int _MDInIrrIntensityID         = MFUnset;
static int _MDInIrrEfficiencyID        = MFUnset;
static int _MDInAirTemperatureID       = MFUnset;
static int _MDInPrecipID               = MFUnset;
static int _MDInSPackChgID             = MFUnset;
static int _MDGrowingSeason1ID         = MFUnset;
static int _MDGrowingSeason2ID         = MFUnset;
static int _MDGrowingSeason3ID         = MFUnset;
static int _MDInFldCapaID              = MFUnset;
static int _MDInWltPntID               = MFUnset;
static int _MDNumberOfIrrCrops         = MFUnset;
static int *_MDInCropFractionIDs       = (int *) NULL;
//Output
static int _MDOutIrrNetDemandID        = MFUnset;
static int _MDOutIrrEvapotranspID      = MFUnset;
static int _MDOutIrrGrossDemandID      = MFUnset;
static int _MDOutIrrReturnFlowID       = MFUnset;
static int _MDOutIrrSoilMoistID        = MFUnset;
static int _MDOutIrrSMoistChgID        = MFUnset;
static int *_MDOutCropDeficitIDs = (int *) NULL;

//Parameters
static float _MDParIrrDailyPercolation;

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
static int   getNumGrowingSeasons(float);

static float getIrrGrossWaterDemand(float, float);
static float irrAreaFrac;

static void _MDIrrGrossDemand (int itemID) {
//Input
	float irrEffeciency;
	float dailyPrecip = 0.0;
	float dailyEffPrecip=0;
	float refETP;
	float cropFraction[_MDNumberOfIrrCrops+1];
	float snowpackChg=0;
	int seasStart[3];
 //Output:
	float totalNetIrrDemand  =0;
	float totalIrrPercolation=0;
// Local
	int i;
	_MDParIrrDailyPercolation = 1.0;
	float ReqpondingDepth = 80.0;
	float pondingDepth;

	irrAreaFrac = MFVarGetFloat(_MDInIrrAreaFracID, itemID, 0.0);
	if (irrAreaFrac > 0.0) {
		for (i = 0;i < _MDNumberOfIrrCrops + 1;i++) { cropFraction[i] = 0.0; }
		for (i = 0;i < _MDNumberOfIrrCrops;    i++) {
			if (MFVarTestMissingVal (_MDInCropFractionIDs [i],itemID)) {
				MFVarSetFloat(_MDInCropFractionIDs [i],itemID,0.0);
			}
		}
		irrEffeciency   = MFVarGetFloat(_MDInIrrEfficiencyID,    itemID, 38);
		dailyPrecip     = MFVarGetFloat(_MDInPrecipID,           itemID, 0.0);
		refETP          = MFVarGetFloat(_MDInIrrRefEvapotransID, itemID, 0.0);

		if (refETP == 0.0) refETP = 0.01;

		snowpackChg = MFVarGetFloat (_MDInSPackChgID, itemID, 0.0);
		if (snowpackChg >  0.0) dailyPrecip = 0.0; //Snow Accumulation, no liquid precipitation
		if (snowpackChg <= 0.0) dailyPrecip = dailyPrecip + fabs (snowpackChg); //Add Snowmelt

		dailyEffPrecip = getEffectivePrecipitation(dailyPrecip);
	 
		seasStart[0]= MFVarGetFloat(_MDGrowingSeason1ID,      itemID, 100);
		seasStart[1]= MFVarGetFloat(_MDGrowingSeason2ID,      itemID, 220);
		seasStart[2]= MFVarGetFloat(_MDGrowingSeason3ID,      itemID, 320); 

	 	float dailyPercolation    = _MDParIrrDailyPercolation;
	 	float wltPnt              = MFVarGetFloat (_MDInWltPntID,  itemID, 0.35);
		float fldCap              = MFVarGetFloat (_MDInFldCapaID, itemID, 0.15);
		if (fldCap == 0.0) {
			fldCap=0.35;
			wltPnt=0.2;
		}
		float cropDepletionFactor;
		float meanSMChange;
		float meanSMoist;
		float irrIntensity        = MFVarGetFloat (_MDInIrrIntensityID, itemID, 100.0) / 100.0;
		if (irrIntensity < 1.5 && irrIntensity > 0.5) irrIntensity = 1.0;
		if (irrIntensity < 2.5 && irrIntensity > 2.0) irrIntensity = 2.0;
 		int daysSincePlanted;
		float totAvlWater,readAvlWater, curDepl, prevSoilMstDepl;
		float cropWR=0;
		float totalCropETP=0;
		float sumOfCropFractions=0;
		float cropCoeff;
		int stage;
		float totGrossDemand=0;
		int curDay = MFDateGetDayOfYear();
		
		float smChange;
		sumOfCropFractions=0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {sumOfCropFractions += MFVarGetFloat(_MDInCropFractionIDs [i],itemID, 0.0);	}
		if (sumOfCropFractions==0) { // No Cropdata for irrigated cell: default to some cereal crop
			MFVarSetFloat(_MDInCropFractionIDs [1],itemID, 0.3);
		}

		sumOfCropFractions=0;
	  	for (i = 0; i < _MDNumberOfIrrCrops; i++) {sumOfCropFractions += MFVarGetFloat(_MDInCropFractionIDs [i],itemID, 0.0);	}
 
		float croppedArea=0;
		float relCropFraction=0;
		float netIrrDemand    = 0;
 
		float deepPercolation = 0;
		meanSMChange=0;totalCropETP=0;
		int   numGrowingSeasons;
		float addBareSoil;
		float curCropFraction;

		for (i = 0; i < _MDNumberOfIrrCrops; i++) { // cropFraction[_MDNumberOfIrrCrops] is bare soil Area!
			 numGrowingSeasons = getNumGrowingSeasons (irrIntensity);
			 curCropFraction = MFVarGetFloat(_MDInCropFractionIDs [i],itemID, 0.0);
			 if (curCropFraction>0){relCropFraction = curCropFraction / sumOfCropFractions;}else{relCropFraction=0;}
			 daysSincePlanted = getDaysSincePlanting(curDay, seasStart,numGrowingSeasons,&_MDirrigCropStruct[i]);
//			 if (itemID==330)printf ("RelCropFraction for %i %f\n",i,cropFraction[i]);
			 if (daysSincePlanted > 0) {
			 	addBareSoil = relCropFraction - irrIntensity / ceil(irrIntensity) * relCropFraction;
			    if (relCropFraction>0)cropFraction[i] = relCropFraction - addBareSoil;
				cropFraction[_MDNumberOfIrrCrops]+=addBareSoil;
			}
			else {
				cropFraction[i] = 0;
				cropFraction[_MDNumberOfIrrCrops] += relCropFraction;
			}
//			if (cropFraction[_MDNumberOfIrrCrops]==0.0 &&  itemID==330)printf ("baresoil %f\n",cropFraction[_MDNumberOfIrrCrops]);
		}

		croppedArea=0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {
			croppedArea+=cropFraction[i];
//			if (itemID==8010)printf("CropFrac %f %i\n",cropFraction[i], itemID);
		}
//		if (debugSum+cropFraction[_MDNumberOfIrrCrops] > 1.0000001)printf ("Cropped  %f \n", debugSum+cropFraction[_MDNumberOfIrrCrops]);
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {
			netIrrDemand=0;cropWR=0;deepPercolation=0;smChange=0;
			relCropFraction   = cropFraction[i];
			numGrowingSeasons = getNumGrowingSeasons (irrIntensity);
			if (relCropFraction > 0) {
			 	daysSincePlanted = getDaysSincePlanting(curDay, seasStart,numGrowingSeasons,&_MDirrigCropStruct[i]);
				if (daysSincePlanted > 0) {
					prevSoilMstDepl = MFVarGetFloat(_MDOutCropDeficitIDs [i],itemID, 0.0);
					stage = getCropStage(&_MDirrigCropStruct[i], daysSincePlanted);
					cropCoeff =getCropKc(&_MDirrigCropStruct[i], daysSincePlanted, stage);
					cropWR = refETP * cropCoeff;
//					if (itemID ==6)printf ("hier refETP %f cropCoeff %f FC %f PWP %f\n",refETP, cropCoeff,fldCap,wltPnt);
					float rootDepth = getCurCropRootingDepth (&_MDirrigCropStruct[i],daysSincePlanted);
					rootDepth = 250;
					float riceWaterBalance;
					float nonRiceWaterBalance;
					
					cropDepletionFactor=getCorrDeplFactor(&_MDirrigCropStruct[i], cropWR);
					if (_MDirrigCropStruct[i].cropIsRice==1) {
//						if (itemID==53428)printf("rice %i \n", itemID);
						if (daysSincePlanted ==1) {
							prevSoilMstDepl=0;
//							if (itemID==6)printf("rice day one %i\n", itemID);
						}
						pondingDepth=prevSoilMstDepl+dailyEffPrecip-cropWR-dailyPercolation;
						if (pondingDepth>=ReqpondingDepth) {
							deepPercolation=pondingDepth -ReqpondingDepth;
							pondingDepth=ReqpondingDepth;
						}
						if (pondingDepth<ReqpondingDepth) {
							netIrrDemand=ReqpondingDepth-pondingDepth;
							pondingDepth=ReqpondingDepth;
						}
						curDepl=pondingDepth; //so that current ponding depth gets set..		
						smChange=curDepl-prevSoilMstDepl;
					 
//						if (itemID==6)printf("netIrrDemand %f\n", smChange);
						deepPercolation+=dailyPercolation;
						riceWaterBalance=dailyEffPrecip+netIrrDemand-cropWR-deepPercolation-smChange;
						if (fabs(riceWaterBalance) > 0.0001)printf ("RiceWaterBalance %f prevSMM %f smChange %f cropWR %f \n",riceWaterBalance, prevSoilMstDepl,smChange,cropWR);
					}
					else {
						totAvlWater  = (fldCap  -wltPnt) * rootDepth;
						readAvlWater = totAvlWater * cropDepletionFactor;
						curDepl  = prevSoilMstDepl - dailyEffPrecip + cropWR;
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
				 
						nonRiceWaterBalance=dailyEffPrecip+netIrrDemand-cropWR-deepPercolation-smChange;
						if (fabs(nonRiceWaterBalance) > 0.0001)printf ("NONRiceWaterBalance %f prevSMM %f smChange %f cropWR %f \n",nonRiceWaterBalance, prevSoilMstDepl,smChange,cropWR);
					}
					MFVarSetFloat(_MDOutCropDeficitIDs [i],itemID,curDepl);
				}
				totalNetIrrDemand+=netIrrDemand*relCropFraction;
				totalCropETP+=cropWR*relCropFraction;
				meanSMChange+=smChange*relCropFraction;
				totalIrrPercolation+=deepPercolation*relCropFraction;
	 		}
		}
		// Add Water Balance for bare soil
		    
		relCropFraction   = cropFraction [_MDNumberOfIrrCrops];		
  
		if (relCropFraction > 0.0) { //Crop is not currently grown. ET from bare soil is equal to ET (initial)
			netIrrDemand = 0.0;
			cropWR = 0.2 * refETP;
			
			prevSoilMstDepl= MFVarGetFloat (_MDOutCropDeficitIDs [_MDNumberOfIrrCrops],itemID, 0.0);
			totAvlWater  = (fldCap - wltPnt) * 250;	//assumed RD = 0.15 m
			deepPercolation = 0.0;
			curDepl  = prevSoilMstDepl - dailyEffPrecip + cropWR;
			if (curDepl < 0) {	
				curDepl = 0;	
				deepPercolation=dailyEffPrecip - prevSoilMstDepl -cropWR;
			}

			if (curDepl >= totAvlWater) {
				cropWR=totAvlWater -prevSoilMstDepl +dailyEffPrecip;
				deepPercolation=0;
				curDepl=totAvlWater;
				if (cropWR < 0)printf ("CropWR, item %i TAW %f PrevSM %f EffP %f\n",itemID,totAvlWater,prevSoilMstDepl,dailyEffPrecip);
//				if (itemID==4) printf("her FC %f WP %f curDepl %f prevDepl %f totAvlWater %f cropWR %f dailyEffPrecip %f\n",fldCap, wltPnt,curDepl,prevSoilMstDepl, totAvlWater, cropWR, dailyEffPrecip);								
			}
 			smChange = prevSoilMstDepl - curDepl;
			float bareSoilBalance;
			bareSoilBalance=dailyEffPrecip -smChange- cropWR - netIrrDemand -deepPercolation;
			if (fabs(bareSoilBalance >0.0001)) printf ("bare SMBalance!! precip %f cropWR %f smchange %f dp %f\n ",dailyEffPrecip , cropWR ,smChange, deepPercolation );
  			MFVarSetFloat (_MDOutCropDeficitIDs [_MDNumberOfIrrCrops], itemID, curDepl);
  			
		}
		totalNetIrrDemand   += netIrrDemand    * relCropFraction;
		totalCropETP        += cropWR          * relCropFraction;
		meanSMoist          += curDepl         * relCropFraction;
		meanSMChange        += smChange        * relCropFraction;
		totalIrrPercolation += deepPercolation * relCropFraction;

		totGrossDemand = getIrrGrossWaterDemand (totalNetIrrDemand, irrEffeciency);
		float loss = 0.0;
		loss = (totGrossDemand - totalNetIrrDemand) + (dailyPrecip - dailyEffPrecip);
		float OUT =totalCropETP+totalIrrPercolation+loss+meanSMChange;
		float IN = totGrossDemand+dailyPrecip;
//		if (itemID==6)printf("itemID %iGrossDemn %f DailyPrecip %f meanSMChange  %f \n",itemID,totGrossDemand,dailyPrecip,meanSMChange);
//		if (itemID==229)printf("ItemID %i ppt %f CropET %f perc %f loss %f \n",itemID,dailyPrecip,totalCropETP,totalIrrPercolation,loss);	
//		if (fabs(IN-OUT) > 0.1) CMmsgPrint (CMmsgAppError,"WaterBalance in MDIrrigation!!! IN %f OUT %f BALANCE %f LOSS %f %i DEMAND %f %i EffPrecip %f   itemID %i \n", IN, OUT, IN-OUT, loss, itemID, totGrossDemand, itemID, dailyEffPrecip,itemID);
		if (fabs(IN-OUT)>0.001) printf ("WaterBalance in MDIrrigation!!! IN %f OUT %f BALANCE %f LOSS %f %i DEMAND %f NET %f  EffPrecip %f dailyPrecip %f cropped %f bare %f itemID %i \n", IN, OUT, IN-OUT, loss, itemID, totGrossDemand, totalNetIrrDemand, dailyEffPrecip,dailyPrecip,croppedArea,cropFraction[_MDNumberOfIrrCrops],itemID);

		MFVarSetFloat(_MDOutIrrSMoistChgID,       itemID, meanSMChange        * irrAreaFrac);
		MFVarSetFloat(_MDOutIrrNetDemandID,       itemID, totalNetIrrDemand   * irrAreaFrac);
		MFVarSetFloat(_MDOutIrrGrossDemandID,     itemID, totGrossDemand      * irrAreaFrac);
		MFVarSetFloat(_MDOutIrrReturnFlowID,      itemID, (totalIrrPercolation + loss) * irrAreaFrac);
		MFVarSetFloat(_MDOutIrrEvapotranspID,     itemID, totalCropETP        * irrAreaFrac);	
	}
	else { // cell is not irrigated
		MFVarSetFloat(_MDOutIrrSoilMoistID,       itemID, 0.0);
		MFVarSetFloat(_MDOutIrrSMoistChgID,       itemID, 0.0);
 		MFVarSetFloat(_MDOutIrrNetDemandID,       itemID, 0.0);
		MFVarSetFloat(_MDOutIrrGrossDemandID,     itemID, 0.0);
		MFVarSetFloat(_MDOutIrrReturnFlowID,      itemID, 0.0);
		MFVarSetFloat(_MDOutIrrEvapotranspID,     itemID, 0.0);	
	}
}

enum { MDnone, MDinput, MDcalculate };
	 
int MDIrrGrossDemandDef() {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptIrrigation;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };
	int i;
	float par;
	char varname [20];

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	if ((optID == MDnone) ||(_MDOutIrrGrossDemandID != MFUnset)) return (_MDOutIrrGrossDemandID);
   
	MFDefEntering ("Irrigation Gross Demand");

	switch (optID) {
		case MDinput:
			 if(((_MDOutIrrGrossDemandID = MFVarGetID (MDVarIrrGrossDemand,        "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    	((_MDOutIrrReturnFlowID  = MFVarGetID (MDVarIrrReturnFlow,         "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDOutIrrEvapotranspID = MFVarGetID (MDVarIrrEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDcalculate:
			if (((optStr = MFOptionGet(MDParIrrDailyPercolationRate)) != (char *) NULL) && (sscanf(optStr, "%f", &par) == 1))
				_MDParIrrDailyPercolation = par;

			if ((optStr = MFOptionGet (MDParIrrigationCropFileName)) != (char *) NULL) CropParameterFileName = optStr;
			if (readCropParameters (CropParameterFileName) == CMfailed) {
				CMmsgPrint(CMmsgUsrError,"Error reading crop parameter file   : %s \n", CropParameterFileName);
				return CMfailed;
			}
			if (((_MDInPrecipID              = MDPrecipitationDef    ()) == CMfailed) ||	 		 
			    ((_MDInSPackChgID            = MDSPackChgDef         ()) == CMfailed) ||
			    ((_MDInIrrRefEvapotransID    = MDIrrRefEvapotransDef ()) == CMfailed) ||
			    ((_MDInAirTemperatureID      = MFVarGetID (MDVarAirTemperature,         "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInWltPntID              = MFVarGetID (MDVarSoilWiltingPoint,       "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInFldCapaID             = MFVarGetID (MDVarSoilFieldCapacity,      "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDGrowingSeason1ID        = MFVarGetID (MDVarIrrGrowingSeason1Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDGrowingSeason2ID        = MFVarGetID (MDVarIrrGrowingSeason2Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDGrowingSeason3ID        = MFVarGetID (MDVarIrrGrowingSeason3Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInIrrIntensityID        = MFVarGetID (MDVarIrrIntensity,           "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInIrrEfficiencyID       = MFVarGetID (MDVarIrrEfficiency,          "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInIrrAreaFracID         = MFVarGetID (MDVarIrrAreaFraction,        "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrGrossDemandID     = MFVarGetID (MDVarIrrGrossDemand,         "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrReturnFlowID      = MFVarGetID (MDVarIrrReturnFlow,          "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrNetDemandID       = MFVarGetID (MDVarIrrNetWaterDemand,      "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSoilMoistID       = MFVarGetID (MDVarIrrSoilMoisture,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSMoistChgID       = MFVarGetID (MDVarIrrSoilMoistChange,     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrEvapotranspID     = MFVarGetID (MDVarIrrEvapotranspiration,  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed))
				return (CMfailed);

			for (i = 0; i < _MDNumberOfIrrCrops; i++) {
				sprintf (varname, "CropFraction_%02d", i + 1); // Input Fraction of crop type per cell
				if ((_MDInCropFractionIDs [i] = MFVarGetID (varname, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
					CMmsgPrint (CMmsgUsrError, "CMfailed in MDInCropFractionID \n");
					return CMfailed;
				}
			}
			for (i=0;i<_MDNumberOfIrrCrops+1;i++) {
				sprintf (varname, "CropSMDeficiency%02d", i + 1);  // Output Soil Moisture Deficit per croptype
				if ((_MDOutCropDeficitIDs [i] = MFVarGetID (varname, "mm", MFOutput, MFState, MFInitial))  == CMfailed) {
					CMmsgPrint (CMmsgUsrError,"MFFAult in MDCropDeficit\n");
					return CMfailed;
				}
			}
			if (MFModelAddFunction (_MDIrrGrossDemand) == CMfailed) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving("Irrigation Gross Demand");
	return (_MDOutIrrGrossDemandID);
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
	float effPrecip=0;
	
	if (dailyPrecip == 0) return 0.0;
	if (dailyPrecip <= 0.2) return 0.0;
		
	
	if (dailyPrecip < 8.3)
		return (dailyPrecip * (4.17 - 0.2 * dailyPrecip) / 4.17);
	else
		effPrecip = dailyPrecip * .1 + 4.17;
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
	
		fgets (buffer,sizeof (buffer),inputCropFile);
		while (feof(inputCropFile) == 0) {
			_MDirrigCropStruct   = (MDIrrigatedCrop *) realloc(_MDirrigCropStruct, (i + 1) * sizeof(MDIrrigatedCrop));
			_MDInCropFractionIDs = (int *) realloc(_MDInCropFractionIDs, (i + 1) * sizeof(int));
			_MDOutCropDeficitIDs = (int *) realloc(_MDOutCropDeficitIDs, (i + 1) * sizeof(int));	
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
	//CMmsgPrint(CMmsgDebug,"Number of crops read: %i \n", i - 1);
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
