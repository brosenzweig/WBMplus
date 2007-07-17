/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDAccumulate.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

//Input;
static int _MDInPrecipID     = MFUnset;

//Output
static int _MDOutAccPrecipID = MFUnset;

static void _MDAccumPrecip (int itemID) {
	float accum;
	
	accum = MFVarTestMissingVal (_MDInPrecipID, itemID) ? 0.0 : MFVarGetFloat(_MDInPrecipID, itemID);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccPrecipID, itemID, MFVarGetFloat (_MDOutAccPrecipID, itemID) + accum);
}

int MDAccumPrecipDef() {

	if (_MDOutAccPrecipID != MFUnset) return (_MDOutAccPrecipID);

	MFDefEntering ("Accumulate Precipitation");
	if ((_MDInPrecipID     = MDPrecipitationDef ()) == CMfailed) return CMfailed;
	if ((_MDOutAccPrecipID = MFVarGetID (MDVarAccPrecipitation, "m3/s",   MFRoute,  MFState, MFInitial)) == CMfailed) return CMfailed;
	_MDOutAccPrecipID = MFVarSetFunction(_MDOutAccPrecipID,_MDAccumPrecip);

	MFDefLeaving ("Accumulate Precipitation");
	return (_MDOutAccPrecipID);	
}

//Input;
static int _MDInEvapID      = MFUnset;

//Output
static int _MDOutAccEvapID  = MFUnset;

static void _MDAccumEvap (int itemID) {
	float accum;
	
	accum = MFVarTestMissingVal (_MDInEvapID, itemID) ? 0.0 : MFVarGetFloat(_MDInEvapID, itemID);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccEvapID, itemID, MFVarGetFloat (_MDOutAccEvapID, itemID) + accum);
}

int MDAccumEvapDef() {

	if (_MDOutAccEvapID != MFUnset) return (_MDOutAccEvapID);

	MFDefEntering ("Accumulate Evapotranspiration");
	if ((_MDInEvapID     = MFVarGetID (MDVarEvapotranspiration,    "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutAccEvapID = MFVarGetID (MDVarAccEvapotranspiration, "m3/s", MFRoute, MFState, MFInitial))  == CMfailed) return CMfailed;
	_MDOutAccEvapID = MFVarSetFunction(_MDOutAccEvapID,_MDAccumEvap);

	MFDefLeaving ("Accumulate Evapotranspiration");
	return (_MDOutAccEvapID);	
}

//Input;
static int _MDInSMoistChgID     = MFUnset;

//Output
static int _MDOutAccSMoistChgID = MFUnset;

static void _MDAccumSMoistChg (int itemID) {
	float accum;
	
	accum = MFVarTestMissingVal (_MDInSMoistChgID, itemID) ? 0.0 : MFVarGetFloat(_MDInSMoistChgID, itemID);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccSMoistChgID, itemID, MFVarGetFloat (_MDOutAccSMoistChgID, itemID) + accum);
}

int MDAccumSMoistChgDef() {

	if (_MDOutAccSMoistChgID != MFUnset) return (_MDOutAccSMoistChgID);

	MFDefEntering ("Accumulate Soil Moisture Change");
	if (((_MDInSMoistChgID     = MDSMoistChgDef ()) == CMfailed) ||
	    ((_MDOutAccSMoistChgID = MFVarGetID (MDVarAccSoilMoistChange, "m3/s", MFRoute, MFState, MFInitial))  == CMfailed)) return (CMfailed);
	_MDOutAccSMoistChgID = MFVarSetFunction(_MDOutAccSMoistChgID,_MDAccumSMoistChg);

	MFDefLeaving ("Accumulate Soil Moisture Change");
	return (_MDOutAccSMoistChgID);	
}

//Input;
static int _MDInGrdWatChgID     = MFUnset;

//Output
static int _MDOutAccGrdWatChgID = MFUnset;

static void _MDAccumGrdWatChg (int itemID) {
	float accum;
	
	accum = MFVarTestMissingVal (_MDInGrdWatChgID, itemID) ? 0.0 : MFVarGetFloat(_MDInGrdWatChgID, itemID);
	accum = accum * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat(_MDOutAccGrdWatChgID, itemID, MFVarGetFloat (_MDOutAccGrdWatChgID, itemID) + accum);
}

int MDAccumGrdWatChgDef() {

	if (_MDOutAccGrdWatChgID != MFUnset) return (_MDOutAccGrdWatChgID);

	MFDefEntering ("Accumulate Groundwater Change");
	if (((_MDInGrdWatChgID     = MFVarGetID (MDVarGroundWaterChange,    "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutAccGrdWatChgID = MFVarGetID (MDVarAccGroundWaterChange, "m3/s", MFRoute,  MFState, MFInitial))  == CMfailed))
		return (CMfailed);
	_MDOutAccGrdWatChgID = MFVarSetFunction(_MDOutAccGrdWatChgID,_MDAccumGrdWatChg);

	MFDefLeaving ("Accumulate Groundwater Change");
	return (_MDOutAccGrdWatChgID);	
}


//Input;
static int _MDInRunoffVolumeID  = MFUnset;

//Output
static int _MDOutAccRunoffID    = MFUnset;

static void _MDAccumRunoff (int itemID) {
	float accum;
	
	accum = MFVarTestMissingVal (_MDInRunoffVolumeID, itemID) ? 0.0 : MFVarGetFloat(_MDInRunoffVolumeID, itemID);
	MFVarSetFloat(_MDOutAccRunoffID, itemID, MFVarGetFloat (_MDOutAccRunoffID, itemID) + accum);
}

int MDAccumRunoffDef() {

	if (_MDOutAccRunoffID != MFUnset) return (_MDOutAccRunoffID);

	MFDefEntering ("Accumulate Runoff");
	if (((_MDInRunoffVolumeID  = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDOutAccRunoffID    = MFVarGetID (MDVarAccRunoff, "m3/s", MFRoute,  MFState, MFInitial)) == CMfailed)) return (CMfailed);
	_MDOutAccRunoffID = MFVarSetFunction(_MDOutAccRunoffID,_MDAccumRunoff);

	MFDefLeaving ("Accumulate Runoff");
	return (_MDOutAccRunoffID);	
}
