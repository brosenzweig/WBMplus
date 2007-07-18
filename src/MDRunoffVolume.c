/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRunoffVolume.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

/* Input */
static int _MDInRunoffID        = MFUnset;
/* Output */
static int _MDOutRunoffVolumeID = MFUnset;

static void _MDRunoffVolume (int itemID) {
// if (MFVarTestMissingVal (_MDInBaseFlowID, itemID)) printf("missing baseflow in runoff!\n");
	/* Input */
	float runoff;

	runoff = MFVarGetFloat (_MDInRunoffID, itemID, 0.0) * MFModelGetArea (itemID) / 86400000.0;
	MFVarSetFloat (_MDOutRunoffVolumeID, itemID, runoff);
}
 
enum { MDinput, MDcalculate };

int MDRunoffVolumeDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarRunoffVolume;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };
		if (_MDOutRunoffVolumeID != MFUnset) return (_MDOutRunoffVolumeID);

	MFDefEntering ("Runoff Volume");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			_MDOutRunoffVolumeID = MFVarGetID (MDVarRunoffVolume, "m3/s", MFInput, MFState, MFBoundary);
			break;
		case MDcalculate:
			if (((_MDInRunoffID        = MDRunoffDef ()) == CMfailed) ||
			    ((_MDOutRunoffVolumeID = MFVarGetID (MDVarRunoffVolume, "m3/s", MFOutput, MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
			_MDOutRunoffVolumeID = MFVarSetFunction(_MDOutRunoffVolumeID,_MDRunoffVolume);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Runoff Volume");
	return (_MDOutRunoffVolumeID);
}
