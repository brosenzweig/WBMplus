/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPotETJensen.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAtMeanID, _MDInSolRadID;
static int _MDOutPetID = CMfailed;

static void _MDPotETJensen (int itemID) {
/* Jensen-Haise (1963) PE in mm for day  */
/* Input */
	float airT;		/* air temperatur [degree C] */
	float solRad;  /* daily solar radiation on horizontal [MJ/m2] */
/* Output */ 
	float pet;
	
	if ((MFVarTestMissingVal (_MDInAtMeanID, itemID)) ||
		 (MFVarTestMissingVal (_MDInSolRadID, itemID))) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	airT   = MFVarGetFloat (_MDInAtMeanID, itemID);
	solRad = MFVarGetFloat (_MDInSolRadID, itemID);

	pet = 0.41 * (0.025 * airT + .078) * solRad;
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETJensenDef () {
	if (_MDOutPetID != CMfailed) return (_MDOutPetID);

	MFDefEntering ("PotET Jensen");
	if (((_MDInSolRadID = MDSolarRadDef ()) == CMfailed) ||
		 ((_MDInAtMeanID = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDOutPetID   = MFVarGetID (MDVarPotEvapotrans,  "mm",    MFOutput, MFFlux,  false)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Jensen");
	return (MFVarSetFunction (_MDOutPetID,_MDPotETJensen));
}
