/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSoilAvailWaterCap.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInSoilFieldCapacityID  = MFUnset;
static int _MDInSoilWiltingPointID   = MFUnset;
static int _MDInSoilRootingDepthID   = MFUnset;
// Output
static int _MDOutSoilAvailWaterCapID = MFUnset;

static void _MDSoilAvailWaterCap (int itemID) {
	float fieldCapacity; // Field capacity [mm/m]
	float wiltingPoint;  // Wilting point [mm/m]
	float rootingDepth;  // Rooting depth [m]

	fieldCapacity = MFVarGetFloat (_MDInSoilFieldCapacityID, itemID, 0.0);
	wiltingPoint  = MFVarGetFloat (_MDInSoilWiltingPointID,  itemID, 0.0);
	rootingDepth  = MFVarGetFloat (_MDInSoilRootingDepthID,   itemID, 0.0);
	if (fieldCapacity < wiltingPoint) fieldCapacity = wiltingPoint;
	
	MFVarSetFloat (_MDOutSoilAvailWaterCapID, itemID, rootingDepth * (fieldCapacity - wiltingPoint));
}

int MDSoilAvailWaterCapDef () {

	if (_MDOutSoilAvailWaterCapID!= MFUnset) return (_MDOutSoilAvailWaterCapID);

	MFDefEntering ("Soil available water capacity");
	
	if (((_MDInSoilFieldCapacityID  = MFVarGetID (MDVarSoilFieldCapacity, "mm/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSoilWiltingPointID   = MFVarGetID (MDVarSoilWiltingPoint,  "mm/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSoilRootingDepthID   = MFVarGetID (MDVarSoilRootingDepth,  "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilAvailWaterCapID = MFVarGetID (MDVarSoilAvailWaterCap, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDSoilAvailWaterCap) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Soil available water capacity");
	return (_MDOutSoilAvailWaterCapID);
}
