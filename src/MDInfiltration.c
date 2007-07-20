/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDInfiltration.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<string.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

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

enum { MDsimple, MDvarying };


int MDInfiltrationDef () {
	int  optID = MDsimple;
	const char *optStr, *optName = "Infiltration";
	const char *options [] = { "simple", "varying", (char *) NULL };
	float par;

	if (_MDOutInfiltrationID != MFUnset) return (_MDOutInfiltrationID);
	
	MFDefEntering ("Infiltration");
 	
	if ((_MDInWaterSurplusID = MDWaterSurplusDef ()) == CMfailed) return (CMfailed);
	
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	switch (optID) {
		case MDsimple: 		
			if (((optStr = MFOptionGet (MDParInfiltrationFrac))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
				_MDInfiltrationFrac = par;
			if (((_MDOutSurfaceROID    = MFVarGetID (MDVarSurfaceRO,    "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    ((_MDOutInfiltrationID = MFVarGetID (MDVarInfiltration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDInfiltrationSimple) == CMfailed)) return (CMfailed);
		case MDvarying:
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Infiltration");
	return (_MDOutInfiltrationID);
}
