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
static int _MDInWaterSurplusID  = MFUnset;
// Output
static int _MDOutSurfaceROID    = MFUnset;
static int _MDOutInfiltrationID = MFUnset;
static float _MDInfiltrationFrac = 0.5;

static void _MDInfiltrationSimple (int itemID) {

	float surplus;

	surplus = MFVarGetFloat(_MDInWaterSurplusID, itemID, 0.0);
	MFVarSetFloat (_MDOutSurfaceROID,    itemID, (1.0 - _MDInfiltrationFrac) * surplus);
	MFVarSetFloat (_MDOutInfiltrationID, itemID,  _MDInfiltrationFrac        * surplus);
}

enum { MDinput, MDsimple, MDvarying };


int MDInfiltrationDef () {
	int  optID = MDsimple;
	const char *optStr, *optName = "Infiltration";
	const char *options [] = { MDInputStr, "simple", "varying", (char *) NULL };
	float par;

	if (_MDOutInfiltrationID != MFUnset) return (_MDOutInfiltrationID);
	
	MFDefEntering ("Infiltration");
 	
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	switch (optID) {
		case MDinput:
			_MDOutInfiltrationID = MFVarGetID (MDVarInfiltration, "mm", MFInput, MFFlux, MFBoundary);
			break;
		case MDsimple:
		case MDvarying:	
			if ((_MDInWaterSurplusID = MDWaterSurplusDef ()) == CMfailed) return (CMfailed);
			if (((optStr = MFOptionGet (MDParInfiltrationFrac))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
				_MDInfiltrationFrac = par;
			if (((_MDOutSurfaceROID    = MFVarGetID (MDVarSurfaceRO,    "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    ((_MDOutInfiltrationID = MFVarGetID (MDVarInfiltration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDInfiltrationSimple) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	

		
		
	MFDefLeaving  ("Infiltration");
	return (_MDOutInfiltrationID);
}
