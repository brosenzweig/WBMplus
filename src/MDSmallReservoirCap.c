/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSmallReservoirs.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

//Input
static int _MDInIrrAreaID                   = MFUnset;
static int _MDInRainSurfaceROID             = MFUnset;
static int _MDInGrossDemandID               = MFUnset;
//Output
static int _MDOutRainSurfaceROAccumulateID  = MFUnset;
static int _MDOutIrrGrossDemandAccumulateID = MFUnset;
static int _MDOutSmallResCapacityID         = MFUnset;

static int _MDSmallReservoirCapacity (int itemID) {
// Input
	float irrAreaFraction;      // Irrigated arrea fraction
	float accumSurfaceRO;       // Accumulated surface runoff [mm]
	float accumIrrDemand;       // Accumulated irrigational water demand [mm]
// Output
 	float smallResCapacity;     // maximum storage, m3
 // Local
 	float potResCapacity;

 	irrAreaFraction  = MFVarGetFloat (_MDInIrrAreaID,           itemID, 0.0);
 	accumSurfaceRO   = MFVarGetFloat (_MDInRainSurfaceROID,     itemID, 0.0) * (1.0 - irrAreaFraction);
 	accumIrrDemand   = MFVarGetFloat (_MDInIrrGrossDemandID,    itemID, 0.0) * irrAreaFraction;
	smallResCapacity = MFVarGetFloat (_MDOutSmallResCapacityID, itemID, 0.0); 

 	if (MFDateGetDayOfYear () > 1) { 
 		accumSurfaceRO   += MFVarGetFloat (_MDOutSurfaceROAccumulatedID,  itemID, 0.0);
 		accumIrrDemand   += MFVarGetFloat (_MDOutGrossDemandAccumulateID, itemID, 0.0);
	}
 
 	potResCapacity   = accumSurfaceRO   < accumIrrDemand ? accumSurfaceRO   : accumIrrDemand;
 	smallResCapacity = smallResCapacity > potResCapacity ? smallResCapacity : potResCapacity;
 
 	MFVarSetFloat (_MDOutRainSurfaceROAccumulatedID, itemID, accumSurfaceRO);
 	MFVarSetFloat (_MDOutIrrGrossDemandAccumlatedID, itemID, accumIrrDemand);
 	MFVarSetFloat (_MDOutSmallResCapacityID,         itemID, smallResCapacity);
}

enum { MDnone, MDinput, MDcalculate };

int MDSmallReservoirCapacityDef () {

	int  optID = MFUnset;
	const char *optStr, *optName = MDVarSmallResCapacity;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutSmallResCapacityID != MFUnset)) return (_MDOutSmallResCapacityID);

	MFDefEntering("SmallReservoirCapacity");
	if ((_MDInGrossDemandID = MDIrrGrossDemandDef  ()) != MFUnset) {
		switch (optID) {
			case MDinput:
			    if ((_MDOutSmallResCapacityID       = MFVarGetID (MDVarSmallResCapacity,          "mm",   MFInput, MFState, MFInitial))  == CMfailed)
			    	return (CMfailed);
			    break;
			case MDcalculate:
				if ((_MDInIrrGrossDemandID == CMfailed) ||
				    ((_MDInIrrAreaID                   = MFVarGetID (MDVarIrrAreaFraction,           "-",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
				    ((_MDInRainSurfaceROID             = MFVarGetID (MDVarSurfaceRO,                 "mm",  MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				    ((_MDOutRainSurfaceROAccumulateID  = MFVarGetID ("__SurfaceROAccumulated",       "mm",  MFOutput, MFFlux,  MFInitial))  == CMfailed) ||
				    ((_MDOutIrrGrossDemandAccumulateID = MFVarGetID ("__GrossDemandAccumulated",     "mm",  MFOutput, MFFlux,  MFInitial))  == CMfailed) ||
				    ((_MDOutSmallResCapacityID         = MFVarGetID (MDVarSmallResCapacity,          "mm",  MFOutput, MFState, MFInitial))  == CMfailed) ||
				    (MFModelAddFunction (_MDSmallReservoirCapacity) == CMfailed)) return (CMfailed);
				break;
			default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	}
	MFDefLeaving("SmallReservoirCapacity");
	return (_MDOutSmallResCapacityID);
}