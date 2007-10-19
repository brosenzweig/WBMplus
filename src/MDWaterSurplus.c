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
static int _MDInSPackChgID              = MFUnset;
static int _MDInSMoistChgID             = MFUnset;
static int _MDInEvapoTransID            = MFUnset;
static int _MDInPrecipID                = MFUnset;
static int _MDInIrrAreaID				=MFUnset;
// Output
static int _MDOutWaterSurplusID         = MFUnset;

static void _MDWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
	float irrAreaFraction=0; 
// Output
	float surplus;
 
	sPackChg   = MFVarGetFloat (_MDInSPackChgID,   itemID, 0.0);
	sMoistChg  = MFVarGetFloat (_MDInSMoistChgID,  itemID, 0.0);
	evapoTrans = MFVarGetFloat (_MDInEvapoTransID, itemID, 0.0);
	precip     = MFVarGetFloat (_MDInPrecipID,     itemID, 0.0);
	if (_MDInIrrAreaID != MFUnset)irrAreaFraction = MFVarGetFloat(_MDInIrrAreaID,       itemID, 0.0);
	
	surplus = precip*(1-irrAreaFraction) - sPackChg *(1-irrAreaFraction)- evapoTrans - sMoistChg;
 	surplus = surplus;// /(1-irrAreaFraction);
 	
	if (surplus < 0.0) surplus = 0.0;
 
	MFVarSetFloat (_MDOutWaterSurplusID,itemID,surplus);
}

int MDWaterSurplusDef () {
	
	if (_MDOutWaterSurplusID != MFUnset) return (_MDOutWaterSurplusID);
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };
	
	MFDefEntering ("Water Surplus");
	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
			//    if (((_MDInIrrGrossDemandID     = MDIrrGrossDemandDef ()) == CMfailed) ||
			 if( ((_MDInIrrAreaID          = MFVarGetID (MDVarIrrAreaFraction,          "-",    MFInput,  MFState, MFBoundary)) == CMfailed))return CMfailed;
	}
	if (((_MDInSPackChgID       = MDSPackChgDef    ()) == CMfailed) ||
	    ((_MDInSMoistChgID      = MDSMoistChgDef   ()) == CMfailed) ||
	    ((_MDInPrecipID         = MDPrecipitationDef ()) == CMfailed) ||
	  
	    ((_MDInEvapoTransID     = MFVarGetID (MDVarEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterSurplusID  = MFVarGetID (MDVarWaterSurplus,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDWaterSurplus) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Water Surplus");
	return (_MDOutWaterSurplusID);
}
