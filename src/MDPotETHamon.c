/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPotETHamon.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInDayLengthID, _MDInAtMeanID;
static int _MDOutPetID = CMfailed;

static void _MDPotETHamon (int itemID) {
/* Hamon (1963) PE in mm for day */
/* Input */
	float dayLen;  /* daylength in fraction of day */
	float airT;		/* air temperatur [degree C] */
/* Local */
	float rhoSat;	/* saturated vapor density [kg/m3] */
/* Output */
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID, itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,    itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	dayLen = MFVarGetFloat (_MDInDayLengthID, itemID);
	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID);

   rhoSat = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
   pet = 165.1 * 2.0 * dayLen * rhoSat; /* 2 * DAYLEN = daylength as fraction of 12 hours */
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETHamonDef () {
	if (_MDOutPetID != CMfailed) return (_MDOutPetID);

	MFDefEntering ("PotET Hamon");
	if (((_MDInDayLengthID = MDSRadDayLengthDef ()) == CMfailed) ||
		 ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature, "degC", MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDOutPetID      = MFVarGetID (MDVarPotEvapotrans,  "mm",   MFOutput, MFFlux,  false)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Hamon");
	return (MFVarSetFunction (_MDOutPetID,_MDPotETHamon));
}
