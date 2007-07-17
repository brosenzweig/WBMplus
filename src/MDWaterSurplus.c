/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDWaterSurplus.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInSPackChgID              = MFUnset;
static int _MDInSMoistChgID             = MFUnset;
static int _MDInEvapoTransID            = MFUnset;
static int _MDInPrecipID                = MFUnset;
static int _MDInActIrrAreaID            = MFUnset;
static int _MDInGrossIrrigationDemandID = MFUnset;
// Output
static int _MDOutWaterSurplusID         = MFUnset;

static void _MDWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
	float actIrrArea;
// Output
	float surplus;

	if (MFVarTestMissingVal (_MDInPrecipID,        itemID)) {
// 		printf( "Missing Precip at  lat %f lon %f \n", MFModelGetLatitude(itemID), MFModelGetLongitude(itemID) );
	}
	if (MFVarTestMissingVal (_MDInSMoistChgID,      itemID))
		printf ("Missing scyahge lat %f lon %f \n", MFModelGetLatitude(itemID), MFModelGetLongitude(itemID));	
	if (MFVarTestMissingVal (_MDInEvapoTransID,     itemID))
		printf ("Missing eva09 lat %f lon %f \n",   MFModelGetLatitude(itemID), MFModelGetLongitude(itemID));	
	if (MFVarTestMissingVal (_MDInSPackChgID,       itemID))
		printf ("Missing spacklat %f lon %f \n",    MFModelGetLatitude(itemID), MFModelGetLongitude(itemID));	
	
	if (MFVarTestMissingVal (_MDInSPackChgID,      itemID) ||
		 MFVarTestMissingVal (_MDInSMoistChgID,     itemID) ||
		 MFVarTestMissingVal (_MDInEvapoTransID,    itemID) ||
		MFVarTestMissingVal(_MDInActIrrAreaID,itemID)||
		 MFVarTestMissingVal (_MDInPrecipID,        itemID)) {
		MFVarSetMissingVal (_MDOutWaterSurplusID,itemID); 
// 		printf("Down with the framework\n");
		return;
	}
	actIrrArea = MFVarGetFloat(_MDInActIrrAreaID,itemID);
	sPackChg   = MFVarGetFloat (_MDInSPackChgID,   itemID);
	sMoistChg  = MFVarGetFloat (_MDInSMoistChgID,  itemID);
	evapoTrans = MFVarGetFloat (_MDInEvapoTransID, itemID);
	precip     = MFVarGetFloat (_MDInPrecipID,     itemID);

	surplus = precip - sPackChg - evapoTrans - sMoistChg;
	//related to non-irrigated part of the grid cell:
	surplus=surplus*(1-actIrrArea); 
	if (surplus < 0.0) surplus = 0.0;
	MFVarSetFloat (_MDOutWaterSurplusID,itemID,surplus);

//if (itemID==2001)printf("ActuAreaInMDSurplus=%f\n",actIrrArea);
}

int MDWaterSurplusDef () {
	if (_MDOutWaterSurplusID != MFUnset) return (_MDOutWaterSurplusID);
	MFDefEntering ("Water Surplus");
	
	if ((_MDInGrossIrrigationDemandID = MDIrrigationDef()) == CMfailed) return CMfailed;
    if ((_MDInActIrrAreaID	= MFVarGetID (MDVarActuallyIrrArea,    "-",  MFOutput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
	if (((_MDInSPackChgID   = MDSPackChgDef    ()) == CMfailed) ||
	    ((_MDInSMoistChgID  = MDSMoistChgDef   ()) == CMfailed) ||
	    ((_MDInPrecipID     = MFVarGetID (MDVarPrecipitation,      "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInEvapoTransID = MFVarGetID (MDVarEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	
	    ((_MDOutWaterSurplusID = MFVarGetID (MDVarWaterSurplus,    "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed)) {
		return (CMfailed);
	}
	MFDefLeaving ("Water Surplus");
	return (_MDOutWaterSurplusID = MFVarSetFunction (_MDOutWaterSurplusID,_MDWaterSurplus));
}
