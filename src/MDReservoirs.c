/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDReservoirs.c

dominik.wisser@unh.edu

*******************************************************************************/


#include<cm.h>
#include<MF.h>
#include<MD.h>
 
// Input
static int _MDInDischargeID      = MFUnset;
static int _MDInDischMeanID      = MFUnset;
static int _MDInResCapacityID    = MFUnset;
// Output
static int _MDOutResStorageID    = MFUnset;
static int _MDOutResStorageChgID = MFUnset;
static int _MDOutResReleaseID    = MFUnset;

static void _MDReservoir (int itemID) {

// Input
	float discharge;      // Current discharge [m3/s]
	float meanDischarge;  // Long-term mean annual discharge [m3/s]
	float resCapacity;    // Reservoir capacity [km3]
// Output
	float resStorage;     // Reservoir storage [km3]
	float resStorageChg;  // Reservoir storage change [km3/dt]
	float resRelease;     // Reservoir release [m3/s] 
// local
	float prevResStorage; // Reservoir storage from the previous time step [km3]
	float dt;             // Time step length [s]
// Parameters
	float drySeasonPct = .6;
	float wetSeasonPct = 0.16;
	
	if (MFVarTestMissingVal (_MDInDischargeID, itemID) ||
	    MFVarTestMissingVal (_MDInDischMeanID, itemID)) discharge = meanDischarge = 0.0;
	else {
		discharge     = MFVarGetFloat (_MDInDischargeID, itemID);
		meanDischarge = MFVarGetFloat (_MDInDischMeanID, itemID);
	}
	if (MFVarTestMissingVal (_MDInResCapacityID,     itemID) ||
	    ((resCapacity = MFVarGetFloat (_MDInResCapacityID, itemID)) <= 0.0)) { 
		MFVarSetFloat (_MDOutResStorageID,    itemID, 0.0); 
		MFVarSetFloat (_MDOutResStorageChgID, itemID, 0.0); 
		MFVarSetFloat (_MDOutResReleaseID,    itemID, discharge);
		return;
	}

	dt = MFModelGet_dt ();
	if (MFVarTestMissingVal (_MDOutResStorageID, itemID)) prevResStorage = 0.0;
	else prevResStorage = MFVarGetFloat(_MDOutResStorageID, itemID);

	resRelease = discharge > meanDischarge ?
		         wetSeasonPct * discharge  :
		         drySeasonPct * discharge + (meanDischarge - discharge);

 	resStorage = prevResStorage + (discharge - resRelease) * 86400.0 / 1e9;
	if (resStorage > resCapacity) {
		resRelease = discharge * dt / 1e9 + prevResStorage - resCapacity;
		resRelease = resRelease * 1e9 / dt;
		resStorage = resCapacity;
	}
	else if (resStorage < 0.0) {
		resRelease = prevResStorage + discharge  * dt / 1e9;
		resRelease = resRelease * 1e9 / dt;
		resStorage=0;
	}
			
	resStorageChg = resStorage - prevResStorage;
	MFVarSetFloat (_MDOutResStorageID,    itemID, resStorage);
	MFVarSetFloat (_MDOutResStorageChgID, itemID, resStorageChg);
	MFVarSetFloat (_MDOutResReleaseID,    itemID, resRelease);
}

enum { MDnone, MDcalculate };

int MDReservoirDef () {
	const char *optStr, *optName = MDOptReservoirs;
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };
  
	if (_MDOutResReleaseID != MFUnset) return (_MDOutResReleaseID);

	MFDefEntering ("Reservoirs");
	if (((optStr = MFOptionGet (optName)) != (char *) NULL) && (CMoptLookup (options, optStr, true) != MDcalculate)) {
		MFOptionMessage (optName, optStr, options);
		return (CMfailed);
	}

	if (((_MDInDischMeanID      = MDDischMeanDef ())   == CMfailed) ||
	    ((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
	    ((_MDInResCapacityID    = MFVarGetID (MDVarReservoirCapacity,      "km3",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutResStorageID    = MFVarGetID (MDVarReservoirStorage,       "km3",  MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutResStorageChgID = MFVarGetID (MDVarReservoirStorageChange, "km3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutResReleaseID    = MFVarGetID (MDVarReservoirRelease,       "m3/s", MFOutput, MFFlux,  MFBoundary)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Reservoirs");
	return (MFVarSetFunction(_MDOutResReleaseID,_MDReservoir)); 
}
