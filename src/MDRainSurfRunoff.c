/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRainSurfRunoff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Output
static int _MDOutRainInfiltrationID = MFUnset;
static int _MDOutRainSurfRunoffID   = MFUnset;

enum { MDinput, MDcalculate };

int MDRainSurfRunoffDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = "SurfRunoff";
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRainSurfRunoffID != MFUnset) return (_MDOutRainSurfRunoffID);
	
	MFDefEntering ("Rainfed Surface Runoff");
 	
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	switch (optID) {
		case MDinput:
			_MDOutRainSurfRunoffID         = MFVarGetID (MDVarRainSurfRunoff, "mm", MFInput,  MFFlux, MFBoundary);
			break;
		case MDcalculate:	
			if (((_MDOutRainInfiltrationID = MDRainInfiltrationDef ()) == CMfailed) ||
			    ((_MDOutRainSurfRunoffID   = MFVarGetID (MDVarRainSurfRunoff, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Rainfed Infiltration");
	return (_MDOutRainSurfRunoffID);
}
