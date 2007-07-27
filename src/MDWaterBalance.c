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
static int _MDInIrrSoilMoistChgID         = MFUnset;
static int _MDInGrdWatChgID               = MFUnset;
static int _MDInRunoffID                  = MFUnset;
static int _MDInIrrAreaID                 = MFUnset;
static int _MDInCropEtpID                 = MFUnset;
static int _MDInDischargeAbstractionID    = MFUnset;

static int _MDInInfiltrationID            = MFUnset;
static int _MDInGrdWaterAbstractionID     = MFUnset;
static int _MDInGrossIrrDemandID          = MFUnset;
static int _MDInIrrReturnFlowID           = MFUnset;
static int _MDInExcessAbstractionID       = MFUnset;
static int _MDDischargeID                 = MFUnset;
static int _MDBgcRoutingID                = MFUnset;
//Output
static int _MDOutWaterBalanceID           = MFUnset;
static int _MDInAirTemperatureID          = MFUnset;
static int _MDOutTotalEvapotranpirationID = MFUnset;
static int _MDIsGrowingSeasonID           = MFUnset;
static int _MDInRefETPID                  = MFUnset;

float etpThreshold=0.5;

static void _MDWaterBalance(int itemID) {

	if ((MFVarTestMissingVal (_MDInPrecipID,          itemID)) ||
	    (MFVarTestMissingVal (_MDInEvaptrsID,         itemID)) ||
	    (MFVarTestMissingVal (_MDInSnowPackChgID,     itemID)) ||
	    (MFVarTestMissingVal (_MDInSoilMoistChgID,    itemID)) ||
	    (MFVarTestMissingVal (_MDInRunoffID,          itemID)) ||
	    (MFVarTestMissingVal (_MDInIrrSoilMoistChgID, itemID)) ||
	    (MFVarTestMissingVal (_MDInGrdWatChgID,       itemID)) ||
	    (MFVarTestMissingVal (_MDInInfiltrationID,itemID))) {
//	printf ("Missing Variables in MDWaterBalance!\n");
	MFVarSetMissingVal(_MDOutWaterBalanceID,itemID);
	return;
	}
	
  
	
	float ppt              = MFVarGetFloat(_MDInPrecipID,               itemID, 0.0);
	float etp              = MFVarGetFloat(_MDInEvaptrsID,              itemID, 0.0);	
	float snowPChange      = MFVarGetFloat(_MDInSnowPackChgID,          itemID, 0.0);	
	float soilMChange      = MFVarGetFloat(_MDInSoilMoistChgID,         itemID, 0.0);
	float runoffPoolChange = MFVarGetFloat(_MDInGrdWatChgID,            itemID, 0.0);
	float cropETP          = MFVarGetFloat(_MDInCropEtpID,              itemID, 0.0);
	float runoff           = MFVarGetFloat(_MDInRunoffID,               itemID, 0.0);
	float irrAreaFraction  = MFVarGetFloat(_MDInIrrAreaID,              itemID, 0.0);
	float waterbalance;
	float infiltration     = MFVarGetFloat(_MDInInfiltrationID,         itemID, 0.0);
	float irrReturnFlow    = MFVarGetFloat(_MDInIrrReturnFlowID,        itemID, 0.0);
	float gwAbstraction    = MFVarGetFloat(_MDInGrdWaterAbstractionID,  itemID, 0.0);
	float disAbstraction   = MFVarGetFloat(_MDInDischargeAbstractionID, itemID, 0.0);
	float exAbstraction    = MFVarGetFloat(_MDInExcessAbstractionID,    itemID, 0.0);
	float irrDemand        = MFVarGetFloat(_MDInGrossIrrDemandID,       itemID, 0.0);
	float refETP           = MFVarGetFloat(_MDInRefETPID,               itemID, 0.0);





	float irrBalance;
	int isGrowingSeason=0;
	float totETP=0;
	float airTemp = MFVarGetFloat(_MDInAirTemperatureID,itemID, 0.0);
	if (irrAreaFraction == 0) {
		waterbalance= ppt-etp - snowPChange-soilMChange-runoffPoolChange-runoff;
		totETP=etp;	
		if ((airTemp > 5) && (etp > refETP * etpThreshold)) isGrowingSeason=1;
	}
	else {
		irrBalance = irrDemand - gwAbstraction - disAbstraction - exAbstraction;
		if (fabs(irrBalance) > 0.1) printf ("IrrBalance %f Demand %f gwAb %f disAbstr %f  exAbstr %f itemID=%i\n",  irrBalance,  irrDemand, gwAbstraction, disAbstraction, exAbstraction,itemID);
	 	if (fabs(disAbstraction) < 0.0) printf ("IrrBalance %f Demand %f gwAb %f disAbstr %f  exAbstr %f itemID=%i\n",  irrBalance,  irrDemand, gwAbstraction, disAbstraction, exAbstraction,itemID);
	 
		
		cropETP=cropETP/irrAreaFraction;
		float irrSMChange=MFVarGetFloat(_MDInIrrSoilMoistChgID,itemID, 0.0);
	//	if (MFVarTestMissingVal(_MDInActIrrAreaID,itemID)) irrAreaFraction=0;
		totETP = cropETP * irrAreaFraction + etp * (1-irrAreaFraction);
		//if (itemID==2001) printf("totETPID =%i\n", _MDOutTotalEvapotranpirationID);
		
		float totSMChange;
		totSMChange=irrSMChange*irrAreaFraction+(1-irrAreaFraction)*soilMChange;
		//snowPChange=snowPChange*(1-irrAreaFraction);

		float in = ppt+exAbstraction+disAbstraction;
		float out = totETP + runoffPoolChange+ totSMChange + snowPChange+runoff;
//if (disAbstraction <0)printf ("DiscAbstr < 0?? %f\n", disAbstraction);
		waterbalance=in -out;
		if (fabs(waterbalance) >0.01 ) {
		//printf ("WaterBalance= %f in= %f out= %f exe= %f disAbstr=%f itmeID=%i,snowChange=%f\n", waterbalance, in, out, exAbstraction,disAbstraction,itemID,snowPChange);
		}
	}
	MFVarSetFloat (_MDOutWaterBalanceID,itemID,waterbalance);
	MFVarSetFloat (_MDInInfiltrationID,itemID, infiltration);
	MFVarSetFloat (_MDOutTotalEvapotranpirationID,itemID,totETP);
	MFVarSetFloat (_MDIsGrowingSeasonID ,itemID,isGrowingSeason);
}

int MDWaterBalanceDef() {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };

	MFDefEntering ("WaterBalance");
	if ((                                  MDAccumBalanceDef ()   == CMfailed) ||
	    ((_MDInPrecipID                  = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDDischargeID                 = MDDischargeDef ())     == CMfailed) ||
	  //  ((_MDBgcRoutingID                = MDBgcRoutingDef ())    == CMfailed) ||
	    ((_MDInEvaptrsID                 = MFVarGetID (MDVarEvapotranspiration,            "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInSnowPackChgID             = MFVarGetID (MDVarSnowPackChange,                "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||

	    ((_MDInSoilMoistChgID            = MFVarGetID (MDVarSoilMoistChange,               "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffID                  = MFVarGetID (MDVarRunoff,                        "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInGrdWatChgID               = MFVarGetID (MDVarGroundWaterChange,             "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInInfiltrationID            = MFVarGetID (MDVarInfiltration,                  "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterBalanceID           = MFVarGetID (MDVarWaterBalance,                  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed))
	    return (CMfailed);
	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((_MDInRefETPID                  = MFVarGetID (MDVarPotEvapotrans,                 "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    	((_MDInCropEtpID                 = MFVarGetID (MDVarIrrCropETP,                    "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrSoilMoistChgID         = MFVarGetID (MDVarIrrSoilMoistureChange,         "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInDischargeAbstractionID    = MFVarGetID (MDVarIrrUptakeRiver,                "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrReturnFlowID           = MFVarGetID (MDVarIrrReturnFlow,                 "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) || 
	    	((_MDInGrdWaterAbstractionID     = MFVarGetID (MDVarIrrUptakeGrdWater,             "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDInIrrAreaID                 = MFVarGetID (MDVarIrrAreaFraction,               "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    	((_MDInExcessAbstractionID       = MFVarGetID (MDVarIrrUptakeExcess,               "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    	((_MDOutTotalEvapotranpirationID = MFVarGetID (MDVarCombinedEvapotranspiration,    "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    	((_MDIsGrowingSeasonID           = MFVarGetID (MDVarIsGrowingSeasonCalc,           "DoY",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    	((_MDInGrossIrrDemandID          = MFVarGetID (MDVarIrrGrossDemand, "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    	((_MDInAirTemperatureID          = MFVarGetID (MDVarAirTemperature,                "degC", MFInput,  MFState, MFBoundary)) == CMfailed))
	    	return (CMfailed);
	}
	
 
	
	
	if (MFModelAddFunction(_MDWaterBalance) == CMfailed) return (CMfailed);
	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
