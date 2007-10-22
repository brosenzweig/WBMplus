/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDInfiltration.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainWaterSurplusID  = MFUnset;
// Output
static int _MDOutRainSurfRunoffID   = MFUnset;
static int _MDOutRainInfiltrationID = MFUnset;
static float _MDInfiltrationFrac = 0.5;

static void _MDRainInfiltrationSimple (int itemID) {

	float surplus;

	surplus = MFVarGetFloat(_MDInRainWaterSurplusID, itemID, 0.0);
	MFVarSetFloat (_MDOutRainSurfRunoffID,   itemID, (1.0 - _MDInfiltrationFrac) * surplus);
	MFVarSetFloat (_MDOutRainInfiltrationID, itemID, _MDInfiltrationFrac         * surplus);
}

enum { MDinput, MDsimple, MDvarying };

int MDRainInfiltrationDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarRainInfiltration;
	const char *options [] = { MDInputStr, "simple", "varying", (char *) NULL };
	float par;

	if (_MDOutRainInfiltrationID != MFUnset) return (_MDOutRainInfiltrationID);
	
	MFDefEntering ("Rainfed Infiltration");
 	
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	switch (optID) {
		case MDinput:
			_MDOutRainInfiltrationID = MFVarGetID (MDVarRainInfiltration, "mm", MFInput, MFFlux, MFBoundary);
			break;
		case MDsimple:
		case MDvarying:	
			if ((_MDInRainWaterSurplusID = MDRainWaterSurplusDef ()) == CMfailed) return (CMfailed);
			if (((optStr = MFOptionGet (MDParInfiltrationFrac))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
				_MDInfiltrationFrac = par;
			if (((_MDOutRainSurfRunoffID   = MFVarGetID (MDVarRainSurfRunoff,   "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    ((_MDOutRainInfiltrationID = MFVarGetID (MDVarRainInfiltration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDRainInfiltrationSimple) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Rainfed Infiltration");
	return (_MDOutRainInfiltrationID);
}
