/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRunoff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInSurfaceROID = MFUnset;
static int _MDInBaseFlowID  = MFUnset;
// Output
static int _MDOutRunoffID   = MFUnset;

static void _MDRunoff (int itemID) {
// Input
	float baseFlow;
	float surfaceRO;

	baseFlow  = MFVarGetFloat (_MDInBaseFlowID,  itemID, 0.0);
	surfaceRO = MFVarGetFloat (_MDInSurfaceROID, itemID, 0.0);

	MFVarSetFloat (_MDOutRunoffID, itemID, baseFlow + surfaceRO);
}
 
enum { MDinput, MDcalculate };

int MDRunoffDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDVarRunoff;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRunoffID != MFUnset) return (_MDOutRunoffID);

	MFDefEntering ("Runoff");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput: _MDOutRunoffID = MFVarGetID (MDVarRunoff, "mm", MFInput, MFFlux, MFBoundary); break;
		case MDcalculate:
	
		
			if (((_MDInBaseFlowID  = MDBaseFlowDef ()) == CMfailed) ||
			    ((_MDInSurfaceROID = MFVarGetID (MDVarSurfaceRO, "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
				 ((_MDOutRunoffID  = MFVarGetID (MDVarRunoff,    "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				 (MFModelAddFunction (_MDRunoff) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Runoff");
	return (_MDOutRunoffID);
}
