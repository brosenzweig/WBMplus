/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDTempRiver.c

wil.wollheim@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfRunoffID     = MFUnset;
static int _MDInBaseFlowID       = MFUnset;
static int _MDInWTempSurfRunoffID = MFUnset;
static int _MDInWTempGrdWaterID   = MFUnset;
// Output
static int _MDOutWTempRiverID     = MFUnset;

static void _MDWTempRiver (int itemID) {
	
}

int MDWTempRiverDef () {

	if (_MDOutWTempRiverID != MFUnset) return (_MDOutWTempRiverID);

	MFDefEntering ("River temperature");

	if (((_MDInSurfRunoffID      = MDRainSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInBaseFlowID        = MDBaseFlowDef       ()) == CMfailed) ||
	    ((_MDInWTempSurfRunoffID = MDWTempSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInWTempGrdWaterID   = MDWTempGrdWaterDef   ()) == CMfailed) ||
	    ((_MDOutWTempRiverID     = MFVarGetID (MDVarWTempRiver, "degC", MFOutput, MFState, MFInitial)) == CMfailed) ||	
	    (MFModelAddFunction (_MDWTempRiver) == CMfailed)) return (CMfailed);

	MFDefLeaving ("River temperature");
	return (_MDOutWTempRiverID);
}
