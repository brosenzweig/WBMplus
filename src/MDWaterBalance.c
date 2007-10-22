/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDWaterBalance.c

dominik.wisser@unh.edu
This is meant to check the vertical water balance for each grid cell. It does 
NOT include any water that is flowing laterally and should not be used to call BCG....
*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
//Water In;

static int _MDInPrecipID                  = MFUnset;
static int _MDInEvaptrsID                 = MFUnset;
static int _MDInSnowPackChgID             = MFUnset;
static int _MDInSoilMoistChgID            = MFUnset;
static int _MDInInfiltrationID            = MFUnset;
static int _MDInGrdWatChgID               = MFUnset;
static int _MDInRunoffID                  = MFUnset;
static int _MDInDischargeID               = MFUnset;

static int _MDInIrrSoilMoistChgID         = MFUnset;
static int _MDInIrrAreaID                 = MFUnset;
static int _MDInCropEtpID                 = MFUnset;
static int _MDInDischargeAbstractionID    = MFUnset;

static int _MDInGrdWaterAbstractionID     = MFUnset;
static int _MDInGrossIrrDemandID          = MFUnset;
static int _MDInIrrReturnFlowID           = MFUnset;
static int _MDInExcessAbstractionID       = MFUnset;
//Output
static int _MDOutWaterBalanceID           = MFUnset;
static int _MDOutTotalEvapotranpirationID = MFUnset;
static int _MDOutSmallResStorageChangeID  =  MFUnset;
static int _MDOutSmallResReleaseID       =  MFUnset;



static void _MDWaterBalance(int itemID) {

	float ppt              = MFVarGetFloat(_MDInPrecipID,         itemID, 0.0);
	float etp              = MFVarGetFloat(_MDInEvaptrsID,        itemID, 0.0);	
	float snowPChange      = MFVarGetFloat(_MDInSnowPackChgID,    itemID, 0.0);	
	float soilMChange      = MFVarGetFloat(_MDInSoilMoistChgID,   itemID, 0.0);
	float runoffPoolChange = MFVarGetFloat(_MDInGrdWatChgID,      itemID, 0.0);
	float runoff           = MFVarGetFloat(_MDInRunoffID,         itemID, 0.0);
 	float waterbalance;
	float totETP = 0.0;
	float smallResStorageChange=0; 
	float cropETP=0;    
	float irrSmallResRelease=0;  
	float		gwAbstraction=0;   
	float		disAbstraction=0;  
	float		exAbstraction=0;  
	float		irrDemand=0;       
	float		irrSMChange=0;     
	float		irrAreaFraction=0; 
	float 		irrReturnFlow=0;
	if ((_MDInGrossIrrDemandID)!=MFUnset)
	{
		 
		cropETP         = MFVarGetFloat (_MDInCropEtpID,              itemID, 0.0);
		gwAbstraction   = MFVarGetFloat (_MDInGrdWaterAbstractionID,  itemID, 0.0);
		disAbstraction  = MFVarGetFloat (_MDInDischargeAbstractionID, itemID, 0.0);
		exAbstraction   = MFVarGetFloat (_MDInExcessAbstractionID,    itemID, 0.0);
		irrReturnFlow   = MFVarGetFloat (_MDInIrrReturnFlowID,        itemID, 0.0);
		irrDemand       = MFVarGetFloat (_MDInGrossIrrDemandID,       itemID, 0.0);
		irrSMChange     = MFVarGetFloat (_MDInIrrSoilMoistChgID,      itemID, 0.0);
		irrAreaFraction = MFVarGetFloat (_MDInIrrAreaID,		      itemID, 0.0);
		if (_MDOutSmallResStorageChangeID!=MFUnset)
			{
			irrSmallResRelease= MFVarGetFloat(_MDOutSmallResReleaseID,itemID,0.0);
			smallResStorageChange= MFVarGetFloat(_MDOutSmallResStorageChangeID,itemID,0.0);
			}
	}	
	 
	float irrBalance;
 
	float totSMChange, in, out;
	
	 
	// Irrigation Water Balance Check! 
		irrBalance = irrDemand - gwAbstraction - disAbstraction - exAbstraction- irrSmallResRelease;
//	if (fabs(irrBalance) > 0.001)     printf ("IrrBalance %f Demand %f gwAb %f disAbstr %f  exAbstr %f smallResRelease%f itemID=%i\n",  irrBalance,  irrDemand, gwAbstraction, disAbstraction, exAbstraction,irrSmallResRelease,itemID);

	//Non irrigated part water balance
	in = ppt;
	out = etp + runoffPoolChange + snowPChange + soilMChange + runoff+smallResStorageChange;
	waterbalance = in -out;
	if (fabs(waterbalance) > 0.001 ) {
		if (irrAreaFraction==0)printf ("non irr Water Balance! WaterBalance= %f in= %f out= %f exe= %f disAbstr=%f itmeID=%i,snowChange=%f irrARea =%f totSMChange %f\n", waterbalance, in, out, exAbstraction,disAbstraction,itemID,snowPChange,irrAreaFraction,totSMChange);
	//	printf ("totETP %f dRun %f totSM %f dSnow %f RO %f dsmallRes%f\n", totETP,runoffPoolChange,totSMChange, snowPChange,runoff,smallResStorageChange);
		}
	//=========================================================
	if (irrAreaFraction >0){
//		if (itemID==6 && ppt ==0)  printf ("IrrDeltaSM %f CropETP %f DeltaSM %f etp %f snow %f\n" ,  irrSMChange, cropETP,soilMChange,etp,snowPChange);
//		if (itemID==6)printf ("runoffPooChange %f runoff %f \n", runoffPoolChange,runoff);
	 		cropETP=cropETP/irrAreaFraction;
	 	 
	 		if (irrAreaFraction==1.0) etp=0;else etp=etp/(1-irrAreaFraction);
	 		
	 	   irrSMChange=irrSMChange/irrAreaFraction;
	 	if (irrAreaFraction==1.0) soilMChange =0;else soilMChange=soilMChange/(1-irrAreaFraction);
	 	}
		totETP = cropETP * irrAreaFraction + etp * (1-irrAreaFraction);
	//	if (itemID==6)printf ("totET= %f etp = %f cropET = %f dSMIrr %f\n", totETP,etp,cropETP,irrSMChange);
		totSMChange=(irrSMChange*irrAreaFraction+(1-irrAreaFraction)*soilMChange);
		runoffPoolChange=runoffPoolChange;//*(1-irrAreaFraction);
		runoff=runoff ;
	

//		in  = ppt+exAbstraction+disAbstraction;
//		out = totETP + runoffPoolChange+ totSMChange + snowPChange+runoff+smallResStorageChange;
		waterbalance=in -out;
		if (fabs(waterbalance) > .011  && disAbstraction==0) {
//	printf ("waterBalance for itemID %i at DoY %i ===========================\n",itemID,MFDateGetDayOfYear ());		
//	printf ("BAL = %f IN = %f ppt = %f disAbstr =%f exAbstr= %f irrArea %f\n",waterbalance,in, ppt,disAbstraction,exAbstraction,irrAreaFraction);
//	printf ("BAL = %f OUT = %f etp = %f dR = %f dSM = %f dSnow = %f R = %f smallRes = %f irrReturnFlow %f %i \n",waterbalance, out,totETP,runoffPoolChange,totSMChange,snowPChange,runoff,smallResStorageChange,irrReturnFlow,itemID);
	
		}
	MFVarSetFloat (_MDOutWaterBalanceID, itemID, waterbalance);
	if (_MDOutTotalEvapotranpirationID != MFUnset) MFVarSetFloat (_MDOutTotalEvapotranpirationID,itemID,totETP);
}

int MDWaterBalanceDef() {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };
 
	MFDefEntering ("WaterBalance");
	if ((                                  MDAccumBalanceDef  ()  == CMfailed) ||
	    ((_MDInPrecipID                  = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInDischargeID               = MDDischargeDef     ()) == CMfailed) ||
	    ((_MDInSnowPackChgID             = MDSPackChgDef      ()) == CMfailed) ||
	    ((_MDInSoilMoistChgID            = MDRainSMoistChgDef     ()) == CMfailed) ||
	    ((_MDInRunoffID                  = MDRunoffDef        ()) == CMfailed) ||
	    ((_MDInInfiltrationID            = MDRainInfiltrationDef  ()) == CMfailed) ||
	    ((_MDInEvaptrsID                 = MFVarGetID (MDVarEvapotranspiration,            "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInGrdWatChgID               = MFVarGetID (MDVarGroundWaterChange,             "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterBalanceID           = MFVarGetID (MDVarWaterBalance,                  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed))
	    return (CMfailed);
	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((_MDInCropEtpID                 = MFVarGetID (MDVarIrrCropETP,                    "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrSoilMoistChgID         = MFVarGetID (MDVarIrrSoilMoistureChange,         "mm",   MFInput,  MFState,  MFBoundary)) == CMfailed) ||
	    	((_MDInDischargeAbstractionID    = MFVarGetID (MDVarIrrUptakeRiver,                "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrReturnFlowID           = MFVarGetID (MDVarIrrReturnFlow,                 "mm",   MFInput,  MFFlux, MFBoundary)) == CMfailed) || 
	    	((_MDInGrdWaterAbstractionID     = MFVarGetID (MDVarIrrUptakeGrdWater,             "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrAreaID                 = MFVarGetID (MDVarIrrAreaFraction,               "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    	((_MDInExcessAbstractionID       = MFVarGetID (MDVarIrrUptakeExcess,               "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDOutTotalEvapotranpirationID = MFVarGetID (MDVarCombinedEvapotranspiration,    "mm",   MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
	    	((_MDInGrossIrrDemandID          = MFVarGetID (MDVarIrrGrossDemand,                "mm",   MFInput,  MFFlux, MFBoundary)) == CMfailed))
	    	return (CMfailed);
		
		if (((optStr = MFOptionGet (MDOptIrrSmallReservoirs)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
 
			if ((_MDOutSmallResReleaseID= MFVarGetID (MDVarSmallResRelease,       "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) return (CMfailed);
			if ((_MDOutSmallResStorageChangeID= MFVarGetID (MDVarSmallResStorageChange,       "mm", MFInput, MFState, MFInitial)) == CMfailed) return (CMfailed);
		}
	}
	if (MFModelAddFunction(_MDWaterBalance) == CMfailed) return (CMfailed);
	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
