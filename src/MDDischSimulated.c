/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischSimulated.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInDischCalculatedID = MFUnset;
static int _MDInDischReleasedID   = MFUnset;

// Output
static int _MDOutDischSimulatedID = MFUnset;

static void _MDDischSimulated (int itemID) {
	float discharge;

	if (MFVarTestMissingVal (_MDInDischCalculatedID, itemID)) discharge = 0.0;
	else discharge = MFVarGetFloat (_MDInDischCalculatedID, itemID);

	if ((_MDInDischReleasedID != MFUnset) && (!MFVarTestMissingVal (_MDInDischReleasedID, itemID))
		discharge = MFVarGetFloat (_MDInDischReleasedID, itemID);
	else if (MFVarTestMissingVal (_MDInDischCalculatedID, itemID) discharge = 0.0;
	else discharge = MFVarGetFloat (_MDInDischCalculatedID, itemID);

	MFVarSetFloat (_MDOutDischSimulateID, itemID, discharge);
}

enum { MDhelp, MDinput, MDsimulated, MDcorrected };

int MDDischSimulatedDef() {
	int optID = MDaccumulate;
	const char *optStr, *optName = MDModReservoirs;
	const char *options [] = { MDnoneStr, (char *) NULL };

	MFDefEntering ("Discharge Simulated");
	if ((_MDInDischCalculatedID = MDDischCalculateDef ()) == CMfailed) return (CMfailed);

	if (((optStr = MFOptionGet (optName)) != (char *) NULL) && (CMoptLookup (options,optStr,true) != CMfailed)) {
		if ((_MDInDischRelaseID = _MDReservoirDef ()) == CMfailed) return (CMfailed);
	}
	if ((_MDOutDischSimulatedID = MFVarGetID (MDVarRiverDischSimulated, "m3/s",  MFOutput,  MFState, false)) == CMfailed) return (CMfailed);
	MFDefLeaving ("Discharge Simulated");
	return (MFVarSetFunction (_MDOutDischSimulatedID,_MDDischSimulated));
}
