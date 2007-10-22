/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDWaterSurplus.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInSPackChgID          = MFUnset;
static int _MDInRainSMoistChgID     = MFUnset;
static int _MDInRainEvapoTransID    = MFUnset;
static int _MDInPrecipID            = MFUnset;
// Output
static int _MDOutRainWaterSurplusID = MFUnset;

static void _MDRainWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
// Output
	float surplus;
 
	sPackChg   = MFVarGetFloat (_MDInSPackChgID,       itemID, 0.0);
	sMoistChg  = MFVarGetFloat (_MDInRainSMoistChgID,  itemID, 0.0);
	evapoTrans = MFVarGetFloat (_MDInRainEvapoTransID, itemID, 0.0);
	precip     = MFVarGetFloat (_MDInPrecipID,         itemID, 0.0);
	
	surplus = precip - sPackChg - evapoTrans - sMoistChg;

	if (surplus < 0.0) surplus = 0.0;
 
	MFVarSetFloat (_MDOutRainWaterSurplusID, itemID, surplus);
}

int MDRainWaterSurplusDef () {
	
	if (_MDOutRainWaterSurplusID != MFUnset) return (_MDOutRainWaterSurplusID);

	MFDefEntering ("Rainfed Water Surplus");
	if (((_MDInPrecipID             = MDPrecipitationDef ()) == CMfailed) ||
		((_MDInSPackChgID           = MDSPackChgDef      ()) == CMfailed) ||
	    ((_MDInRainSMoistChgID      = MDRainSMoistChgDef ()) == CMfailed) ||
	    ((_MDInRainEvapoTransID     = MFVarGetID (MDVarRainEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutRainWaterSurplusID  = MFVarGetID (MDVarRainWaterSurplus,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainWaterSurplus) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Water Surplus");
	return (_MDOutRainWaterSurplusID);
}
