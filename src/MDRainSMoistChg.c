/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRainSMoistChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static float _MDAWCap, _MDPet, _MDWaterIn;

static float _MDDryingFunc (float time, float sMoist) {
	float gm, def, sMoistChg;

	if (_MDAWCap > sMoist) {
		gm = (1.0 - exp (- 5.0 * sMoist / _MDAWCap)) / (1.0 - exp (-5.0));
		def = _MDPet + _MDAWCap - sMoist;
	}
	else { gm = 1.0; def = _MDPet; }

	if (_MDWaterIn > 0.0) 
		sMoistChg  = _MDWaterIn * (gm + ((1.0 - gm) *  exp (-_MDPet / _MDWaterIn)) - exp (-def / _MDWaterIn));
	else
		sMoistChg = 0.0;

	sMoistChg -= _MDPet * gm;
	return (sMoistChg);
}

// Input
static int _MDInAirTMeanID          = MFUnset;
static int _MDInPrecipID            = MFUnset;
static int _MDInPotETID             = MFUnset;
static int _MDInInterceptID         = MFUnset;
static int _MDInSPackChgID          = MFUnset;
static int _MDInSoilAvailWaterCapID = MFUnset;
// Output
static int _MDOutEvaptrsID          = MFUnset;
static int _MDOutSoilMoistID        = MFUnset;
static int _MDOutSMoistChgID        = MFUnset;

static void _MDRainSMoistChg (int itemID) {	
// Input
	float airT;            // Air temperature [degreeC]
	float precip;          // Precipitation [mm/dt]
	float pet;             // Potential evapotranspiration [mm/dt]
	float intercept;       // Interception (when the interception module is turned on) [mm/dt]
	float sPackChg;        // Snow pack change [mm/dt]
// Output
	float sMoist;          // Soil moisture [mm/dt]
	float sMoistChg;       // Soil moisture change [mm/dt]
	float transp;          // Transpiration [mm]
	float evapotrans;
	
	airT      = MFVarGetFloat (_MDInAirTMeanID,          itemID, 0.0);
	precip    = MFVarGetFloat (_MDInPrecipID,            itemID, 0.0);
	sPackChg  = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	pet       = MFVarGetFloat (_MDInPotETID,             itemID, 0.0);
	_MDAWCap  = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	sMoist    = MFVarGetFloat (_MDOutSoilMoistID,        itemID, 0.0);
	intercept = _MDInInterceptID != MFUnset ? MFVarGetFloat (_MDInInterceptID, itemID, 0.0) : 0.0;

	if (sMoist < 0.0) sMoist = 0.0;
	sMoistChg=0;
	 
	if (airT > 0.0) {
		if (pet < 0.0) pet = 0.0; // Why not in the respective ETPot Function?????
		if (_MDAWCap > 0.0) {	
			sMoistChg  = sMoist;
			_MDWaterIn = precip - intercept - sPackChg;
			_MDPet     = pet;
			sMoist     = MFRungeKutta ((float) 0.0,1.0,sMoist,_MDDryingFunc);
		
			if (sMoist < MFPrecision)  sMoist = 0.0;
			if (sMoist > _MDAWCap)     sMoist = _MDAWCap;
			sMoistChg = sMoist - sMoistChg;
		}
		transp = precip - intercept - sPackChg - sMoistChg;
		transp = pet < transp ? pet : transp;
		
	}
	else { transp = sMoistChg = 0.0; }

	evapotrans = intercept + transp;
	MFVarSetFloat (_MDOutEvaptrsID,   itemID, evapotrans);
	MFVarSetFloat (_MDOutSoilMoistID, itemID, sMoist);
	MFVarSetFloat (_MDOutSMoistChgID, itemID, sMoistChg);
}

int MDRainSMoistChgDef () {

	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

	MFDefEntering ("Rainfed Soil Moisture");
	
	if (((_MDInPrecipID            = MDPrecipitationDef     ()) == CMfailed) ||
	    ((_MDInSPackChgID          = MDSPackChgDef          ()) == CMfailed) ||
	    ((_MDInPotETID             = MDRainPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID         = MDRainInterceptDef     ()) == CMfailed) ||
	    ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInAirTMeanID   = MFVarGetID (MDVarAirTemperature,            "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutEvaptrsID   = MFVarGetID (MDVarRainEvapotranspiration,    "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilMoistID = MFVarGetID (MDVarRainSoilMoisture,          "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutSMoistChgID = MFVarGetID (MDVarRainSoilMoistChange,       "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Soil Moisture");
	return (_MDOutSMoistChgID);
}
