/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSMoistChg.c

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
static int _MDInAirTMeanID      = MFUnset;
static int _MDInPrecipID        = MFUnset;
static int _MDInPotETID         = MFUnset;
static int _MDInInterceptID     = MFUnset;
static int _MDInSPackChgID      = MFUnset;
static int _MDInFldCapaID       = MFUnset;
static int _MDInWltPntID        = MFUnset;
static int _MDInRootDepthID     = MFUnset;
// Output
static int _MDOutSoilMoistID    = MFUnset;
static int _MDOutSMoistChgID    = MFUnset;
static int _MDOutEvaptrsID      = MFUnset;
static int _MDOutRelSoilMoistID = MFUnset;
static int _MDInIrrAreaID       = MFUnset;

static void _MDSMoistChg (int itemID) {	
// Input
	float airT;            // Air temperature [degreeC]
	float precip;          // Precipitation [mm/dt]
	float pet;             // Potential evapotranspiration [mm/dt]
	float intercept;       // Interception (when the interception module is turned on) [mm/dt]
	float sPackChg;        // Snow pack change [mm/dt]
	float rootDepth;       // Root depth [mm]
	float wltPnt;          // Wilting point [mm/m]
	float fldCap;          // Field capacity [mm/m]
	float irrAreaFraction; // Irrigated area fraction 
// Output
	float sMoist;          // Soil moisture [mm/dt]
	float sMoistChg;       // Soil moisture change [mm/dt]
	float transp;          // Transpiration [mm]
	float evapotrans;
	
	airT      = MFVarGetFloat (_MDInAirTMeanID,   itemID, 0.0);
	precip    = MFVarGetFloat (_MDInPrecipID,     itemID, 0.0);
	sPackChg  = MFVarGetFloat (_MDInSPackChgID,   itemID, 0.0);
	fldCap    = MFVarGetFloat (_MDInFldCapaID,    itemID, 0.0);
	rootDepth = MFVarGetFloat (_MDInRootDepthID,  itemID, 0.0);  
	pet       = MFVarGetFloat (_MDInPotETID,	  itemID, 0.0);
	wltPnt    = MFVarGetFloat (_MDInWltPntID,	  itemID, 0.0);
	sMoist    = MFVarGetFloat (_MDOutSoilMoistID, itemID, 0.0);
	intercept       = _MDInInterceptID != MFUnset ? MFVarGetFloat (_MDInInterceptID, itemID, 0.0) : 0.0;
	irrAreaFraction = _MDInIrrAreaID   != MFUnset ? MFVarGetFloat (_MDInIrrAreaID,   itemID, 0.0) : 0.0;

	_MDAWCap = (fldCap - wltPnt) * rootDepth;
	MFVarSetFloat (_MDInFldCapaID,    itemID, fldCap);
	MFVarSetFloat (_MDInWltPntID,    itemID, wltPnt);
	
	//if (itemID==46627)printf("Smvo=%f\n",sMoist);
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

	MFVarSetFloat (_MDOutSoilMoistID, itemID, sMoist    * (1.0 - irrAreaFraction));
	MFVarSetFloat (_MDOutSMoistChgID, itemID, sMoistChg * (1.0 - irrAreaFraction));
	evapotrans = (intercept + transp) * (1.0 - irrAreaFraction);
	MFVarSetFloat (_MDOutEvaptrsID,   itemID, evapotrans);
	//if ((intercept + transp) <0) printf ("Hier!ETtrans=%f intercept=%f precip %f snowPckch=%f SM%f dSM%f itemID%i\n", transp,intercept,precip,sPackChg,sMoist,sMoistChg,itemID);

	MFVarSetFloat (_MDOutRelSoilMoistID, itemID, _MDAWCap > 0.0 ? sMoist / _MDAWCap : 0.0);
}

int MDSMoistChgDef () {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

	MFDefEntering ("Soil Moisture");
	
	if (((_MDInPrecipID        = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInPotETID         = MDPotETDef         ()) == CMfailed)) return (CMfailed);
	if (((optStr = MFOptionGet (MDVarInterception)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed) && 
	    ((_MDInInterceptID     = MDInterceptDef     ()) == CMfailed)) return (CMfailed);
	if (((_MDInSPackChgID      = MDSPackChgDef      ()) == CMfailed) ||
	    ((_MDInFldCapaID       = MFVarGetID (MDVarFieldCapacity,         "mm/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInWltPntID        = MFVarGetID (MDVarWiltingPoint,          "mm/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRootDepthID     = MFVarGetID (MDVarRootingDepth,          "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTMeanID      = MFVarGetID (MDVarAirTemperature,        "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilMoistID    = MFVarGetID (MDVarSoilMoisture,          "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutSMoistChgID    = MFVarGetID (MDVarSoilMoistChange,       "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutEvaptrsID      = MFVarGetID (MDVarEvapotranspiration,    "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Soil Moisture");
	return (_MDOutSMoistChgID);
}
