/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDRunoff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

/* Input */
static int _MDInRunoffID;
/* Output */
static int _MDOutRunoffVolumeID = CMfailed;

static void _MDRunoffVolume (int itemID) {
// if (MFVarTestMissingVal (_MDInBaseFlowID, itemID)) printf("missing baseflow in runoff!\n");
	/* Input */
	float runoff;

	if (MFVarTestMissingVal (_MDInRunoffID, itemID))
	 	MFVarSetMissingVal (_MDOutRunoffVolumeID, itemID);
	else {
		runoff = MFVarGetFloat (_MDInRunoffID,itemID) * MFModelGetArea (itemID) / 86400000.0;
		MFVarSetFloat (_MDOutRunoffVolumeID, itemID, runoff);
	}
}
 
enum { MDhelp, MDinput, MDcalculate };

int MDRunoffVolumeDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarRunoffVolume;
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };
		if (_MDOutRunoffVolumeID != CMfailed) return (_MDOutRunoffVolumeID);

	MFDefEntering ("Runoff Volume");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			_MDOutRunoffVolumeID = MFVarGetID (MDVarRunoffVolume, "m3/s", MFInput, MFState, false);
			break;
		case MDcalculate:
			if (((_MDInRunoffID        = MDRunoffDef ()) == CMfailed) ||
			    ((_MDOutRunoffVolumeID = MFVarGetID (MDVarRunoffVolume, "m3/s", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutRunoffVolumeID = MFVarSetFunction(_MDOutRunoffVolumeID,_MDRunoffVolume);
			break;
		default:
			
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving  ("Runoff Volume");
	return (_MDOutRunoffVolumeID);
}
