/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPotETPsTaylor.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInDayLengthID, _MDInI0HDayID;
static int _MDInCParamAlbedoID;

static int _MDInAtMeanID, _MDInSolRadID, _MDInVPressID;
static int _MDOutPetID = CMfailed;

static void _MDPotETPsTaylor (int itemID) {
/* Priestley and Taylor (1972) PE in mm for day */
/* Input */
	float dayLen; /* daylength in fraction of day */
	float i0hDay; /* daily potential insolation on horizontal [MJ/m2] */
	float albedo; /* albedo */
	float airT;   /* air temperatur [degree C] */
	float solRad; /* daily solar radiation on horizontal [MJ/m2] */
	float vPress; /* daily average vapor pressure [kPa] */
	float sHeat = 0.0;  /* average subsurface heat storage for day [W/m2] */
/* Local */	
	float solNet;  /* average net solar radiation for daytime [W/m2] */
	float lngNet;	/* average net longwave radiation for day  [W/m2] */
	float aa;		/* available energy [W/m2] */
	float es;      /* vapor pressure at airT [kPa] */
	float delta;   /* dEsat/dTair [kPa/K] */
 	float dd;      /* vapor pressure deficit [kPa] */
	float le;		/* latent heat [W/m2] */
/* Output */
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID,    itemID) ||
		 MFVarTestMissingVal (_MDInI0HDayID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamAlbedoID, itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,       itemID) ||
		 MFVarTestMissingVal (_MDInSolRadID,       itemID) ||
		 MFVarTestMissingVal (_MDInVPressID,       itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID); return; }

	dayLen  = MFVarGetFloat (_MDInDayLengthID,    itemID);
	i0hDay  = MFVarGetFloat (_MDInI0HDayID,       itemID);
	albedo  = MFVarGetFloat (_MDInCParamAlbedoID, itemID);
	airT    = MFVarGetFloat (_MDInAtMeanID,       itemID);
	solRad  = MFVarGetFloat (_MDInSolRadID,       itemID);
	vPress  = MFVarGetFloat (_MDInVPressID,       itemID);

	solNet = (1.0 - albedo) * solRad / MDConstIGRATE;
	lngNet = MDSRadNETLong (i0hDay,airT,solRad,vPress);

	aa     = solNet + lngNet - sHeat;
	es     = MDPETlibVPressSat (airT);
	delta  = MDPETlibVPressDelta (airT);

	dd     = es - vPress; 
   le     = MDConstPTALPHA * delta * aa / (delta + MDConstPSGAMMA);

	pet = MDConstEtoM * MDConstIGRATE * le; 
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETPsTaylorDef () {
	if (_MDOutPetID != CMfailed) return (_MDOutPetID);

	MFDefEntering ("PotET Priestley - Taylor Definition");
	if (((_MDInDayLengthID     = MDSRadDayLengthDef ()) == CMfailed) ||
		 ((_MDInI0HDayID        = MDSRadI0HDayDef    ()) == CMfailed) ||
	    ((_MDInCParamAlbedoID  = MDCParamAlbedoDef  ()) == CMfailed) ||
		 ((_MDInSolRadID        = MDSolarRadDef      ()) == CMfailed) ||
		 ((_MDInAtMeanID  = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDInVPressID  = MFVarGetID (MDVarVaporPressure,  "kPa",   MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDOutPetID    = MFVarGetID (MDVarPotEvapotrans,  "mm",    MFOutput, MFFlux,  false)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Priestley - Taylor Definition");
	return (MFVarSetFunction (_MDOutPetID,_MDPotETPsTaylor));
}
