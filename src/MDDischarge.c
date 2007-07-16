/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischarge.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInDischSimulatedID = MFUnset;
static int _MDInDischObservedID  = MFUnset;

// Output
static int _MDOutDischargeID     = MFUnset;

static void _MDDischarge (int itemID) {
	float discharge;

	if (MFVarTestMissingVal (_MDInDischSimulatedID, itemID)) discharge = 0.0;
	else discharge = MFVarGetFloat (_MDInDischSimulatedID, itemID);

	if ((_MDInDischObservedID != MFUnset) && (!MFVarTestMissingVal (_MDInDischObservedID, itemID)))
		discharge = MFVarGetFloat (_MDInDischObservedID, itemID);
	else if (MFVarTestMissingVal (_MDInDischSimulatedID, itemID)) discharge = 0.0;
	else discharge = MFVarGetFloat (_MDInDischSimulatedID, itemID);

	MFVarSetFloat (_MDOutDischargeID, itemID, discharge);
}

enum { MDhelp, MDinput, MDsimulated, MDcorrected };

int MDDischargeDef() {
	int optID = MDsimulated;
	const char *optStr, *optName = MDVarDischarge;
	const char *options [] = { MDHelpStr, MDInputStr, "simulated", "corrected", (char *) NULL };

	MFDefEntering ("Discharge");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutDischargeID = MFVarGetID (MDVarDischarge,     "m3/s",  MFInput,  MFState, false); break;
		case MDcorrected:
			if ((_MDInDischObservedID   = MFVarGetID (MDVarDischObserved, "m3/s",  MFInput,  MFState, false)) == CMfailed)
				return (CMfailed);
		case MDsimulated:
			if (((_MDOutDischargeID     = MFVarGetID (MDVarDischarge,     "m3/s",  MFRoute,  MFState, false)) == CMfailed) ||
				((_MDInDischSimulatedID = MDDischSimulatedDef ()) == CMfailed))
			    return (CMfailed);
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Discharge");
	return (MFVarSetFunction(_MDOutDischargeID,_MDDischarge));
}
