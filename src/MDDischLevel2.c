/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischCalculated.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Inputs
static int _MDInDischLevel3ID       = MFUnset;
static int _MDInIrrUptakeExternalID = MFUnset;
// Outputs
static int _MDOutIrrUptakeRiverID   = MFUnset;
static int _MDOutIrrUptakeExcessID  = MFUnset;
static int _MDOutDischLevel2ID      = MFUnset;

static void _MDDischLevel2 (int itemID) {
// Inputs
	float discharge;       // Discharge [m3/s]
	float irrUptakeExt;    // External irrigational water uptake [mm/dt]
// Outputs
	float irrUptakeRiver;  // Irrigational water uptake from river [m3/s]
	float irrUptakeExcess; // Irrigational water uptake from unsustainable source [mm/dt]

	discharge = MFVarGetFloat (_MDInDischLevel3ID, itemID, 0.0);
	if (_MDInIrrUptakeExternalID != MFUnset) {
		irrUptakeExt = MFVarGetFloat (_MDInIrrUptakeExternalID, itemID, 0.0);
		if (discharge > irrUptakeExt) {
			irrUptakeRiver  = irrUptakeExt;
			irrUptakeExcess = 0.0;
			discharge = discharge - irrUptakeRiver;
		}
		else {
			irrUptakeRiver  = discharge;
			irrUptakeExcess = irrUptakeExt - discharge;
			discharge = 0.0;
		}
		MFVarSetFloat (_MDOutIrrUptakeRiverID,  itemID, irrUptakeRiver);
		MFVarSetFloat (_MDOutIrrUptakeExcessID, itemID, irrUptakeExcess);
	}
	MFVarSetFloat (_MDOutDischLevel2ID,  itemID, discharge);
}

int MDDischLevel2Def() {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDOutDischLevel2ID != MFUnset) return (_MDOutDischLevel2ID);

	MFDefEntering ("Discharge Level 2");
	if (((_MDInDischLevel3ID  = MDDischLevel3Def ()) == CMfailed) ||
	    ((_MDOutDischLevel2ID = MFVarGetID (MDVarDischLevel2,  "m/3", MFOutput, MFState, false)) == CMfailed))
	    return (CMfailed);
	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((MDIrrGrossDemandDef ()) == CMfailed) ||
		    ((_MDInIrrUptakeExternalID = MFVarGetID (MDVarIrrUptakeExternal, "m/3", MFInput,  MFState, MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeRiverID   = MFVarGetID (MDVarIrrUptakeRiver,    "m/3", MFInput,  MFState, MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeExcessID  = MFVarGetID (MDVarIrrUptakeExcess,   "mm",  MFOutput, MFFlux,  MFBoundary)) == CMfailed))
			return (CMfailed);
	}
	MFDefLeaving ("Discharge Level 2");
	return (MFVarSetFunction(_MDOutDischLevel2ID,_MDDischLevel2));
}
