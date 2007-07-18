/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSMoistChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

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
static int _MDInFldCapaID    = MFUnset;
static int _MDInWltPntID     = MFUnset;
static int _MDInAirTMeanID   = MFUnset;
static int _MDInPotETID      = MFUnset;
static int _MDInInterceptID  = MFUnset;
static int _MDInPrecipID     = MFUnset;
static int _MDInSPackChgID   = MFUnset;
static int _MDInRootDepthID  = MFUnset;
// Output
static int _MDOutSoilMoistID = MFUnset;
static int _MDOutSMoistChgID = MFUnset;
static int _MDOutEvaptrsID   = MFUnset;
static int _MDOutAvCapID        = MFUnset;

static void _MDSMoistChg (int itemID) {
	float wltPnt, fldCap,airT, precip, intercept, pet, sPackChg, sMoist, sMoistChg = 0.0, transp;
	float rootDepth;

	if (MFVarTestMissingVal (_MDInPotETID,       itemID) ||
		 MFVarTestMissingVal (_MDInInterceptID,   itemID) ||
		 MFVarTestMissingVal (_MDInFldCapaID,     itemID) ||
		 MFVarTestMissingVal (_MDInRootDepthID,   itemID) ||
		 MFVarTestMissingVal (_MDInWltPntID,      itemID) ||
	//	 MFVarTestMissingVal (_MDInAirTMeanID,    itemID) ||
		 MFVarTestMissingVal (_MDInSPackChgID,    itemID) ||
		 MFVarTestMissingVal (_MDInPrecipID,      itemID) ||
		 MFVarTestMissingVal (_MDOutSoilMoistID,  itemID)) { MFVarSetFloat (_MDOutSMoistChgID,itemID,sMoistChg); return; }

	fldCap    = MFVarGetFloat (_MDInFldCapaID,    itemID, 0.0);
	rootDepth = MFVarGetFloat (_MDInRootDepthID,  itemID, 0.0);  
	pet       = MFVarGetFloat (_MDInPotETID,	  itemID, 0.0);
	intercept = MFVarGetFloat (_MDInInterceptID,  itemID, 0.0);
	wltPnt    = MFVarGetFloat (_MDInWltPntID,	  itemID, 0.0);
	airT      = MFVarGetFloat (_MDInAirTMeanID,   itemID, 0.0);
	precip    = MFVarGetFloat (_MDInPrecipID,     itemID, 0.0);
	sPackChg  = MFVarGetFloat (_MDInSPackChgID,   itemID, 0.0);
	sMoist    = MFVarGetFloat (_MDOutSoilMoistID, itemID, 0.0);

	//this has been changed by dw. 	
	_MDAWCap =(fldCap-wltPnt)*rootDepth;	
	MFVarSetFloat(_MDOutAvCapID,itemID,_MDAWCap);
	
	// if (_MDAWCap < 0) printf("AWCAp %f FC %f PWP %f RD %f LON %f LAT %f\n",_MDAWCap,fldCap,wltPnt,rootDepth, MFModelGetLongitude(itemID),MFModelGetLatitude(itemID));	
	//printf ("RD %f\n" , rootDepth);
	if (sMoist < 0.0) sMoist = 0.0;

	if (airT > 0.0) {
		if (pet < 0.0) pet = 0.0;
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

	MFVarSetFloat (_MDOutSoilMoistID,itemID,sMoist);
	MFVarSetFloat (_MDOutSMoistChgID,itemID,sMoistChg);
	MFVarSetFloat (_MDOutEvaptrsID,  itemID,intercept + transp);
}

int MDSMoistChgDef () {
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

	MFDefEntering ("Soil Moisture");
	
//	if (((_MDInAWCapID      = MDSoilPropDef    ()) == CMfailed) ||
	if (((_MDInPrecipID     = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInPotETID      = MDPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID  = MDInterceptDef     ()) == CMfailed) ||
	    ((_MDInFldCapaID    = MFVarGetID (MDVarFieldCapacity,      "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInWltPntID     = MFVarGetID (MDVarWiltingPoint,       "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRootDepthID  = MFVarGetID (MDVarRootingDepth,       "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTMeanID   = MFVarGetID (MDVarAirTemperature,     "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSPackChgID   = MFVarGetID (MDVarSnowPackChange,     "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutAvCapID     = MFVarGetID (MDVarAvailWatCap,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilMoistID = MFVarGetID (MDVarSoilMoisture,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutSMoistChgID = MFVarGetID (MDVarSoilMoistChange,    "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutEvaptrsID   = MFVarGetID (MDVarEvapotranspiration, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed))
		return (CMfailed);
	MFDefLeaving ("Soil Moisture");
	//	printf("in soil moisture def,_MDOutSMoistChgID= %i \n", _MDOutSMoistChgID);
	return (MFVarSetFunction (_MDOutSMoistChgID,_MDSMoistChg));
}
