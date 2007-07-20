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
// Output
static int _MDOutWaterSurplusID         = MFUnset;

static void _MDWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
// Output
	float surplus;

	sPackChg   = MFVarGetFloat (_MDInSPackChgID,   itemID, 0.0);
	sMoistChg  = MFVarGetFloat (_MDInSMoistChgID,  itemID, 0.0);
	evapoTrans = MFVarGetFloat (_MDInEvapoTransID, itemID, 0.0);
	precip     = MFVarGetFloat (_MDInPrecipID,     itemID, 0.0);

	surplus = precip - sPackChg - evapoTrans - sMoistChg;
 	if (surplus < 0.0) surplus = 0.0;
	MFVarSetFloat (_MDOutWaterSurplusID,itemID,surplus);
}

int MDWaterSurplusDef () {
	if (_MDOutWaterSurplusID != MFUnset) return (_MDOutWaterSurplusID);
	MFDefEntering ("Water Surplus");
	
	if (((_MDInSPackChgID       = MDSPackChgDef    ()) == CMfailed) ||
	    ((_MDInSMoistChgID      = MDSMoistChgDef   ()) == CMfailed) ||
	    ((_MDInPrecipID         = MFVarGetID (MDVarPrecipitation,      "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInEvapoTransID     = MFVarGetID (MDVarEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterSurplusID  = MFVarGetID (MDVarWaterSurplus,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDWaterSurplus) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Water Surplus");
	return (_MDOutWaterSurplusID);
}
