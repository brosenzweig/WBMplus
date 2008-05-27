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
static int _MDInIrrAreaFracID       = MFUnset;
// Output
static int _MDOutRainWaterSurplusID = MFUnset;

static void _MDRainWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
	float irrAreaFrac;
// Output
	float surplus;
 
	irrAreaFrac = _MDInIrrAreaFracID != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0) : 0.0;
	sPackChg    = MFVarGetFloat (_MDInSPackChgID,       itemID, 0.0) * (1.0 - irrAreaFrac);
	sMoistChg   = MFVarGetFloat (_MDInRainSMoistChgID,  itemID, 0.0);
	evapoTrans  = MFVarGetFloat (_MDInRainEvapoTransID, itemID, 0.0);
	precip      = MFVarGetFloat (_MDInPrecipID,         itemID, 0.0) * (1.0 - irrAreaFrac);
	
	surplus = precip - sPackChg - evapoTrans - sMoistChg;
	
	//if (itemID == 10081) printf("surplus = %f, precip = %f, sPackChg = %f, evapoTrans = %f\n", surplus, precip, sPackChg, evapoTrans);

	MFVarSetFloat (_MDOutRainWaterSurplusID, itemID, surplus);
}

int MDRainWaterSurplusDef () {
	int ret = 0;

	if (_MDOutRainWaterSurplusID != MFUnset) return (_MDOutRainWaterSurplusID);

	MFDefEntering ("Rainfed Water Surplus");
	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset) &&
			  ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) )
	     return (CMfailed);	
	if (((_MDInPrecipID             = MDPrecipitationDef ()) == CMfailed) ||
		((_MDInSPackChgID           = MDSPackChgDef      ()) == CMfailed) ||
	    ((_MDInRainSMoistChgID      = MDRainSMoistChgDef ()) == CMfailed) ||
	    ((_MDInRainEvapoTransID     = MFVarGetID (MDVarRainEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutRainWaterSurplusID  = MFVarGetID (MDVarRainWaterSurplus,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainWaterSurplus) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Water Surplus");
	return (_MDOutRainWaterSurplusID);
}
