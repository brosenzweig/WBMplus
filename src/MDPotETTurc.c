/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDPotETTurc.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAtMeanID = MFUnset;
static int _MDInSolRadID = MFUnset;
static int _MDOutPetID   = MFUnset;

static void _MDPotETTurc (int itemID) {
/* Turc (1961) PE in mm for day */
/* Input */
	float airT;		/* air temperatur [degree C] */
	float solRad;  /* daily solar radiation on horizontal [MJ/m2] */
/* Output */ 
	float pet;
	
	if ((MFVarTestMissingVal (_MDInAtMeanID, itemID)) ||
		 (MFVarTestMissingVal (_MDInSolRadID, itemID))) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	airT   = MFVarGetFloat (_MDInAtMeanID, itemID);
	solRad = MFVarGetFloat (_MDInSolRadID, itemID);

	pet = airT > 0.0 ? 0.313 * airT * (solRad + 2.1) / (airT + 15) : 0.0;
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETTurcDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("PotET Turc");
	if (((_MDInSolRadID = MDSolarRadDef ()) == CMfailed) ||
		 ((_MDInAtMeanID = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDOutPetID   = MFVarGetID (MDVarPotEvapotrans,  "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Turc");
	return (MFVarSetFunction (_MDOutPetID,_MDPotETTurc));
}
