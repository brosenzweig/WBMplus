/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDTempSurfRunoff.c

wil.wollheim@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAirTempID  = MFUnset;
static int _MDInPrecipID   = MFUnset;
static int _MDInSnowMeltID = MFUnset;
// Output
static int _MDOutWTempSurfROID   = MFUnset;

static void _MDWTempSurfRunoff (int itemID) {
	
}

int MDWTempSurfRunoffDef () {

	if (_MDOutWTempSurfROID != MFUnset) return (_MDOutWTempSurfROID);

	MFDefEntering ("Surface runoff temperature");

	if (((_MDInPrecipID       = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInSnowMeltID     = MDSPackMeltDef     ()) == CMfailed) ||
	    ((_MDInAirTempID      = MFVarGetID (MDVarAirTemperature,  "degC", MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutWTempSurfROID = MFVarGetID (MDVarWTempSurfRunoff, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	
	    (MFModelAddFunction (_MDWTempSurfRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Surface runoff temperature");
	return (_MDOutWTempSurfROID);
}
