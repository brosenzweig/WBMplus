/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSmallReservoirs.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

//Input
static int _MDInIrrAreaFracID         = MFUnset;
static int _MDInRainSurfRunoffID      = MFUnset;
static int _MDInIrrGrossDemandID      = MFUnset;
static int _MDInSmallResCapacityID    = MFUnset;
//Output
static int _MDOutSmallResReleaseID    = MFUnset;
static int _MDOutSmallResStorageID    = MFUnset;
static int _MDOutSmallResStorageChgID = MFUnset;
static int _MDOutSmallResUptakeID     = MFUnset;

static void _MDSmallReservoirRelease (int itemID) {
// Input    
	float irrAreaFraction;    // Irrigated area fraction
	float surfRunoff;         // Surface runoff over non irrigated area [mm/dt]
	float grossDemand ;       // Current irrigation water requirement [mm/dt]
	float smallResCapacity;   // maximum storage [mm]
// Output
	float smallResUptake;     // current release [mm/dt]
	float smallResRelease;    // current release [mm/dt]
	float smallResStorage;    // current storage [mm/dt]
	float smallResStorageChg; // current storage change [mm]
 
	if ((irrAreaFraction   = MFVarGetFloat (_MDInIrrAreaFracID,      itemID, 0.0)) > 0.0) {
		smallResCapacity   = MFVarGetFloat (_MDInSmallResCapacityID, itemID, 0.0); 
		smallResStorage    =
		smallResStorageChg = MFVarGetFloat (_MDOutSmallResStorageID, itemID, 0.0);
		surfRunoff         = MFVarGetFloat (_MDInRainSurfRunoffID,   itemID, 0.0) * (1.0 - irrAreaFraction);
		grossDemand        = MFVarGetFloat (_MDInIrrGrossDemandID,   itemID, 0.0);

		if (smallResCapacity < smallResStorage + surfRunoff) {
			smallResUptake  = smallResCapacity - smallResStorage;
			smallResStorage = smallResCapacity;
			surfRunoff = surfRunoff - (smallResCapacity - smallResStorage);
		}
		else {
			smallResUptake  = surfRunoff;
			smallResStorage = smallResStorage + surfRunoff;
			surfRunoff = 0.0;
		}
		if (smallResStorage > grossDemand) {
			smallResRelease = grossDemand;
			smallResStorage = smallResStorage - smallResRelease;
		}
		else {
			smallResRelease = smallResStorage;
			smallResStorage = 0.0;
		}
		smallResStorageChg = smallResStorage - smallResStorageChg;

		MFVarSetFloat (_MDOutSmallResUptakeID,     itemID, smallResUptake);
		MFVarSetFloat (_MDOutSmallResReleaseID,    itemID, smallResRelease);
		MFVarSetFloat (_MDOutSmallResStorageID,    itemID, smallResStorage);
		MFVarSetFloat (_MDOutSmallResStorageChgID, itemID, smallResStorageChg);

	}
	else {
		MFVarSetFloat (_MDOutSmallResUptakeID,     itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResReleaseID,    itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResStorageID,    itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResStorageChgID, itemID, 0.0);
	}
}

int MDSmallReservoirReleaseDef () {

	if (_MDOutSmallResReleaseID != MFUnset) return (_MDOutSmallResReleaseID);

	if (((_MDInIrrGrossDemandID   = MDIrrGrossDemandDef         ()) == CMfailed) ||
	    ((_MDInSmallResCapacityID = MDSmallReservoirCapacityDef ()) == CMfailed)) return (CMfailed);
	if ((_MDInIrrGrossDemandID == MFUnset) || (_MDInSmallResCapacityID == MFUnset))
		return (_MDOutSmallResReleaseID);

	MFDefEntering("Small Reservoirs");

    if (((_MDInRainSurfRunoffID      = MDRainSurfRunoffDef ()) == CMfailed) ||
        ((_MDOutSmallResUptakeID     = MFVarGetID (MDVarSmallResUptake,            "mm",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResReleaseID    = MFVarGetID (MDVarSmallResRelease,           "mm",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResStorageID    = MFVarGetID (MDVarSmallResStorage,           "mm",  MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutSmallResStorageChgID = MFVarGetID (MDVarSmallResStorageChange,     "mm",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((MFModelAddFunction (_MDSmallReservoirRelease) == CMfailed))) return (CMfailed);
	MFDefLeaving("Small Reservoirs");
	return (_MDOutSmallResReleaseID);
}

 



