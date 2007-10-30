/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSPackMelt.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Output
static int _MDOutSnowMeltID = MFUnset;

int MDSPackMeltDef () {

	if (_MDOutSnowMeltID != MFUnset) return (_MDOutSnowMeltID);
	
	if ((MDSPackChgDef () == CMfailed) ||
	    ((_MDOutSnowMeltID   = MFVarGetID (MDVarSnowMelt, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed))
		return (CMfailed);
	return (_MDOutSnowMeltID);
}
