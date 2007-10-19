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
static int _MDOutSurfaceROID               	  = MFUnset;
static int _MDInGrossIrrigationDemandID     = MFUnset;
static int _MDInIrrAreaID         	      = MFUnset;
static int _MDGrowingSeason1ID				=MFUnset;
static int _MDInGrossDemandAccumulateID=MFUnset;
//Output
static int _MDOutSmallResReleaseID          = MFUnset;
static int _MDOutSmallResStorageID	 			  = MFUnset;
static int _MDOutSmallResCapacityID 			  = MFUnset;
static int _MDOutSmallResStorageChangeID		=MFUnset;


static void _MDSmallReservoirRelease(int itemID) {
	    
	float irrAreaFraction; 
	float surfaceRO;				//local runoff (Surface RO + baseflow, mm)
	float smallResStorage;		// current storage, m3
	float smallResCapacity=0;		// maximum storage, m3
	float smallResRelease;		// current release, m3/s
	float smallResStorageChange;	// current storage change, m3
	float grossIrrDemand ;	// current irrigation water requirement, mm 
	float prevSmallResStorage;	//PreviousStorage
	float remainingSurfaceRO=0;
	float accumIrrDemand;
    float smallResWaterBalance;
    float rest =0;
 
	irrAreaFraction = MFVarGetFloat(_MDInIrrAreaID,       itemID, 0.0);

	prevSmallResStorage= MFVarGetFloat(_MDOutSmallResStorageID,itemID,0.0);
	
/*	if (irrAreaFraction == 0.0) { // small reservoirs are only needed for irrigation
		MFVarSetFloat(_MDOutSmallResReleaseID,itemID,0.0);
		MFVarSetFloat(_MDOutSmallResStorageID,itemID,0.0);
		MFVarSetFloat(_MDOutSmallResCapacityID,itemID,0.0);
		MFVarSetFloat(_MDOutSmallResStorageChangeID,itemID,0.0);
		return;		
	}*/
	
	grossIrrDemand = MFVarGetFloat(_MDInGrossIrrigationDemandID,       itemID, 0.0);

	surfaceRO = MFVarGetFloat(_MDOutSurfaceROID,itemID,0.0);
	
	// Compute Capacity needed for one year after growing season starts
	accumIrrDemand=MFVarGetFloat(_MDInGrossDemandAccumulateID,itemID,0.0);
	accumIrrDemand +=grossIrrDemand; //mm
	MFVarSetFloat(_MDInGrossDemandAccumulateID,itemID,accumIrrDemand);
	if (MFDateGetDayOfYear ()==1)
		{
		 
		smallResCapacity=MFVarGetFloat(_MDInGrossDemandAccumulateID,itemID,0.0); 
		MFVarSetFloat(_MDOutSmallResCapacityID,itemID,smallResCapacity);
		MFVarSetFloat(_MDInGrossDemandAccumulateID,itemID,0.0);
		}
	
 
	smallResCapacity=MFVarGetFloat(_MDOutSmallResCapacityID,itemID,0.0); 
// --------smallResCapacity--------------------------------------------
	//Compute Storage
	prevSmallResStorage = MFVarGetFloat(_MDOutSmallResStorageID, itemID, 0.0);
    smallResRelease = grossIrrDemand;
	
	smallResStorage = prevSmallResStorage + surfaceRO-smallResRelease;

	remainingSurfaceRO=0;
	
	if (smallResStorage< 0)
	{
	smallResRelease =prevSmallResStorage + surfaceRO;	
	smallResStorage =0;	
	remainingSurfaceRO =0;	
	}
	

	
	if (smallResStorage > smallResCapacity)
	{
	smallResStorage = smallResCapacity;	
	rest= prevSmallResStorage + surfaceRO - smallResCapacity;	
	if (rest> grossIrrDemand){
		smallResRelease = grossIrrDemand;
		

	}
	else
	{
	smallResRelease =grossIrrDemand-rest;
	remainingSurfaceRO=0;
		
	}
	}
		
	if (smallResRelease < 0)printf("smallRes ! = %f\n",smallResRelease);
	smallResStorageChange= smallResStorage-prevSmallResStorage;
	
	MFVarSetFloat(_MDOutSmallResReleaseID,itemID,smallResRelease);
	MFVarSetFloat(_MDOutSmallResStorageID,itemID,smallResStorage);
	MFVarSetFloat(_MDOutSmallResStorageChangeID,itemID,smallResStorageChange);
	MFVarSetFloat(_MDOutSurfaceROID,itemID,remainingSurfaceRO); 
//	if (itemID==348)printf ("AccumDemand %f maxCap = %f storage %f release %f surfaceRO %f \n",accumIrrDemand, smallResCapacity, smallResStorage,smallResRelease, surfaceRO);
	smallResWaterBalance= surfaceRO-smallResRelease - remainingSurfaceRO - smallResStorageChange;
//	if (itemID==16172)printf("Stor %f Cap %f surfaceRO %f release %f rest %f demand %f\n",smallResStorage,smallResCapacity,surfaceRO,smallResRelease,remainingSurfaceRO,grossIrrDemand);

	if (fabs(smallResWaterBalance) > 0.001)
	{
		printf ("SmallRes Water Balance Error at itemID %i\n",itemID);
		printf("Stor %f Cap %f surfaceRO %f release %f rest %f demand %f\n",smallResStorage,smallResCapacity,surfaceRO,smallResRelease,remainingSurfaceRO,smallResStorageChange);
	}
}
int MDSmallReservoirsDef() {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };
	if (_MDOutSmallResReleaseID != MFUnset)	return (_MDOutSmallResReleaseID);
 	MFDefEntering("SmallReservoirs");

 	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
	 
	if ((_MDInIrrAreaID          = MFVarGetID (MDVarIrrAreaFraction,          "-",    MFInput,  MFState, MFBoundary)) == CMfailed) return CMfailed;
 	if ((_MDOutSmallResReleaseID = MFVarGetID (MDVarSmallResRelease,              "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) return (CMfailed);
	if ((_MDOutSmallResStorageID = MFVarGetID (MDVarSmallResStorage, "mm", MFOutput, MFState, MFInitial)) == CMfailed) return (CMfailed);
	if ((_MDOutSmallResCapacityID= MFVarGetID (MDVarSmallResCapacity,       "mm", MFOutput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
	if ((_MDInGrossDemandAccumulateID= MFVarGetID (MDVarGrossIrrDemandAccumulated,       "mm", MFOutput, MFFlux, MFInitial)) == CMfailed) return (CMfailed);
 	if ((_MDGrowingSeason1ID         = MFVarGetID (MDVarIrrGrowingSeason1Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutSmallResStorageChangeID= MFVarGetID (MDVarSmallResStorageChange,       "mm", MFOutput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
	if ((_MDInGrossIrrigationDemandID     = MFVarGetID (MDVarIrrGrossDemand,              "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) return (CMfailed);
	if ((_MDOutSurfaceROID    = MFVarGetID (MDVarSurfaceRO,    "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) return CMfailed;
	if (MFModelAddFunction (_MDSmallReservoirRelease) == CMfailed) return (CMfailed);
	}
	MFDefLeaving("SmallReservoirs");
	return (_MDOutSmallResReleaseID);
}

 



