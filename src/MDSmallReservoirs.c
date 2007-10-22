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
static int _MDInRainSurfRunoffID         = MFUnset;
static int _MDInIrrGrossDemandID         = MFUnset;
static int _MDInSmallResCapacityID       = MFUnset;
static int _MDInIrrAreaID                = MFUnset;
//Output
static int _MDOutSmallResReleaseID       = MFUnset;
static int _MDOutSmallResStorageID       = MFUnset;
static int _MDOutSmallResStorageChangeID = MFUnset;

static void _MDSmallReservoirRelease (int itemID) {
	    
	float irrAreaFraction; 
	float surfaceRO;             //local runoff (Surface RO + baseflow, mm)
	float smallResStorage;       // current storage, m3
	float smallResCapacity;      // maximum storage, m3
	float smallResRelease;		 // current release, m3/s
	float smallResStorageChange; // current storage change, m3
	float grossIrrDemand ;       // current irrigation water requirement, mm 
	float prevSmallResStorage;   // PreviousStorage
	float remainingSurfaceRO = 0.0;
    float smallResWaterBalance;
    float rest = 0.0;
 
	irrAreaFraction     = MFVarGetFloat (_MDInIrrAreaID,                itemID, 0.0);
	if (irrAreaFraction > 0.0) {
	
		smallResCapacity    = MFVarGetFloat (_MDInSmallResCapacityID, itemID, 0.0); 
		prevSmallResStorage = MFVarGetFloat (_MDOutSmallResStorageID,   itemID, 0.0);
		grossIrrDemand      = MFVarGetFloat (_MDInIrrGrossDemandID,     itemID, 0.0);
		surfaceRO           = MFVarGetFloat (_MDInRainSurfRunoffID,     itemID, 0.0);

		prevSmallResStorage = MFVarGetFloat(_MDOutSmallResStorageID, itemID, 0.0);
		smallResRelease = grossIrrDemand;
	
		smallResStorage = prevSmallResStorage + surfaceRO - smallResRelease;

		remainingSurfaceRO = 0.0;
	
		if (smallResStorage < 0.0) {
			smallResRelease    = prevSmallResStorage + surfaceRO;	
			smallResStorage    = 0.0;	
			remainingSurfaceRO = 0.0;	
		}

		if (smallResStorage > smallResCapacity) {
			smallResStorage = smallResCapacity;	
			rest = prevSmallResStorage + surfaceRO - smallResCapacity;	
			if (rest> grossIrrDemand) {
				smallResRelease = grossIrrDemand;
			}
			else {
				smallResRelease = grossIrrDemand - rest;
				remainingSurfaceRO = 0.0;
			}
		}

		if (smallResRelease < 0.0) printf ("smallRes ! = %f\n",smallResRelease);
		smallResStorageChange = smallResStorage-prevSmallResStorage;
	
		MFVarSetFloat(_MDOutSmallResReleaseID,itemID,smallResRelease);
		MFVarSetFloat(_MDOutSmallResStorageID,itemID,smallResStorage);
		MFVarSetFloat(_MDOutSmallResStorageChangeID,itemID,smallResStorageChange);
		MFVarSetFloat(_MDInRainSurfRunoffID,itemID,remainingSurfaceRO); 
//		if (itemID==348)printf ("AccumDemand %f maxCap = %f storage %f release %f surfaceRO %f \n",accumIrrDemand, smallResCapacity, smallResStorage,smallResRelease, surfaceRO);
		smallResWaterBalance = surfaceRO - smallResRelease - remainingSurfaceRO - smallResStorageChange;
//		if (itemID==16172)printf("Stor %f Cap %f surfaceRO %f release %f rest %f demand %f\n",smallResStorage,smallResCapacity,surfaceRO,smallResRelease,remainingSurfaceRO,grossIrrDemand);

		if (fabs(smallResWaterBalance) > 0.001) {
			printf ("SmallRes Water Balance Error at itemID %i\n",itemID);
			printf("Stor %f Cap %f surfaceRO %f release %f rest %f demand %f\n",smallResStorage,smallResCapacity,surfaceRO,smallResRelease,remainingSurfaceRO,smallResStorageChange);
		}
	}
	else {
		
	}
}

int MDSmallReservoirsDef () {

	if (_MDOutSmallResReleaseID != MFUnset)	return (_MDOutSmallResReleaseID);
	if (((_MDInIrrGrossDemandID   = MDIrrGrossDemandDef         ()) == CMfailed) ||
	    ((_MDInSmallResCapacityID = MDSmallReservoirCapacityDef ()) == CMfailed)) return (CMfailed);
	if ((_MDInIrrGrossDemandID == MFUnset) || (_MDInSmallResCapacityID == MFUnset))
		return (_MDOutSmallResReleaseID);

	MFDefEntering("SmallReservoirs");

    if (((_MDInRainSurfRunoffID         = MDRainSurfRunoffDef ()) == CMfailed) ||
        ((_MDOutSmallResReleaseID       = MFVarGetID (MDVarSmallResRelease,           "mm",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResStorageID       = MFVarGetID (MDVarSmallResStorage,           "mm",  MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutSmallResStorageChangeID = MFVarGetID (MDVarSmallResStorageChange,     "mm",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((MFModelAddFunction (_MDSmallReservoirRelease) == CMfailed))) return (CMfailed);
	MFDefLeaving("SmallReservoirs");
	return (_MDOutSmallResReleaseID);
}

 



