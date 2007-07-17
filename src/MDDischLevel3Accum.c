/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDichAccumulate.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInRunoffID       = MFUnset;
static int _MDInDischargeID    = MFUnset;
// Output
static int _MDOutDischLevel3ID = MFUnset;

static void _MDDischLevel3Accumulate (int itemID) {
// Input
	float runoff;     // Local runoff volume [m3/s]
	float discharge;  // Discharge from upstream [m3/s]

	if (MFVarTestMissingVal (_MDInRunoffID,    itemID)) runoff    = 0.0;
	else runoff    = MFVarGetFloat(_MDInRunoffID,    itemID);
	if (MFVarTestMissingVal (_MDInDischargeID, itemID)) discharge = 0.0;
	else discharge = MFVarGetFloat(_MDInDischargeID, itemID);

	MFVarSetFloat(_MDOutDischLevel3ID, itemID, discharge + runoff);
}

int MDDischLevel3AccumulateDef () {

	if (_MDOutDischLevel3ID != MFUnset) return (_MDOutDischLevel3ID);

	MFDefEntering ("Discharge Level 3 - Accumulate");
	if (((_MDInRunoffID       = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInDischargeID    = MFVarGetID (MDVarDischarge,   "m3/s", MFInput,  MFState, true))  == CMfailed) ||
	    ((_MDOutDischLevel3ID = MFVarGetID (MDVarDischLevel3, "m3/s", MFOutput, MFState, false)) == CMfailed))
	     return CMfailed;
	MFDefLeaving ("Discharge Accumulate");
	return (MDOutDischLevel3ID = MFVarSetFunction(_MDOutDischLevel3ID,_MDDischLevel3Accumulate));
}
