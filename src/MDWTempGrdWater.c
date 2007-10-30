/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDTempGrdWater.c

wil.wollheim@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInWTempSurfRunoffID = MFUnset;
static int _MDInRainRechargeID    = MFUnset;
static int _MDInIrrReturnFlowID   = MFUnset;
// Output
static int _MDOutWTempGrdWaterID  = MFUnset;

static void _MDWTempGrdWater (int itemID) {
	
}

int MDWTempGrdWaterDef () {

	if (_MDOutWTempGrdWaterID != MFUnset) return (_MDOutWTempGrdWaterID);

	MFDefEntering ("Groundwater temperature");

	if (((_MDInWTempSurfRunoffID = MDWTempSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInRainRechargeID    = MDRainInfiltrationDef ()) == CMfailed) ||
	    ((_MDInIrrReturnFlowID   = MDIrrReturnFlowDef    ()) == CMfailed) ||
	    ((_MDOutWTempGrdWaterID  = MFVarGetID (MDVarWTempGrdWater, "degC", MFOutput, MFState,  MFInitial)) == CMfailed) ||	
	    (MFModelAddFunction (_MDWTempGrdWater) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Groundwater temperature");
	return (_MDOutWTempGrdWaterID);
}
