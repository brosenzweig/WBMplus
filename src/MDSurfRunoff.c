/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSurfRunoff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSurfRunoffID = MFUnset;
static int _MDInSmallResUptakeID = MFUnset;
// Output
static int _MDOutSurfRunoffID    = MFUnset;

static void _MDSurfRunoff (int itemID) {	
// Input
	float surfRunoff;    // Surface runoff [mm/dt]
	
	surfRunoff = MFVarGetFloat (_MDInRainSurfRunoffID,     itemID, 0.0)
	           + (_MDInSmallResUptakeID != MFUnset ? MFVarGetFloat (_MDInSmallResUptakeID, itemID, 0.0) : 0.0);
	MFVarSetFloat (_MDOutSurfRunoffID,  itemID, surfRunoff);
}

int MDSurfRunoffDef () {
	int ret;
	if (_MDOutSurfRunoffID != MFUnset) return (_MDOutSurfRunoffID);

	MFDefEntering ("Soil Moisture");

	if (((ret = MDSmallReservoirReleaseDef ()) != MFUnset) &&
	    ((ret == CMfailed) ||
	     ((_MDInSmallResUptakeID = MFVarGetID (MDVarSmallResUptake, "mm",   MFInput,  MFState, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	if (((_MDInRainSurfRunoffID  = MFVarGetID (MDVarRainSurfRunoff, "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSurfRunoffID     = MFVarGetID (MDVarSurfRunoff,     "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
       (MFModelAddFunction (_MDSurfRunoff) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Soil Moisture");
	return (_MDOutSurfRunoffID);
}
