/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDIntercept.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDInSPackChgID      = MFUnset;
static int _MDInPetID           = MFUnset;
static int _MDInPrecipID        = MFUnset;
static int _MDInCParamCHeightID = MFUnset;
static int _MDInLeafAreaIndexID = MFUnset;
static int _MDInStemAreaIndexID = MFUnset;

static int _MDOutInterceptID    = MFUnset;

static void _MDIntercept (int itemID) {
// Input
	float precip;  // daily precipitation [mm/day]
	float sPackChg;// snow pack change [mm/day]
	float pet;     // daily potential evapotranspiration [mm/day]
	float height;  // canopy height [m]
 	float lai;     // projected leaf area index
	float sai;     // projected stem area index
// Local
	float epi;     // daily potential interception [mm/day]
	float eis;     // maximum amount of evaporated interception during "storm" [mm]
	float c;       // canopy storage capacity [mm]
// Output
	float intercept; // estimated interception [mm] 

	precip   = MFVarGetFloat (_MDInPrecipID,        itemID, 0.0);
	pet      = MFVarGetFloat (_MDInPetID,           itemID, 0.0);

	intercept = 0.0;
	if ((pet > 0.0) && (precip > 0.0)) {
		lai      = MFVarGetFloat (_MDInLeafAreaIndexID, itemID, 0.0);
		sai      = MFVarGetFloat (_MDInStemAreaIndexID, itemID, 0.0);
		c = MDConstInterceptCI * (lai + sai) / 2.0;
		if (c > 0.0) {
			sPackChg = MFVarGetFloat (_MDInSPackChgID,      itemID, 0.0);
			height   = MFVarGetFloat (_MDInCParamCHeightID, itemID, 0.0);
			if (sPackChg > 0.0) precip = precip - sPackChg;
			epi = pet * (height < MDConstInterceptCH ? 1.0 + height / MDConstInterceptCH : 2.0);
			eis = MDConstInterceptD * epi;
			intercept = precip < (eis + c) ? /* capacity is not reached */ precip : /* capacity exceeded */ (eis + c);
			if (intercept > pet) intercept = pet; // FBM Addition
		}
	}
	MFVarSetFloat (_MDOutInterceptID,itemID, intercept);	
}

enum { MDinput, MDnone, MDcalc };

int MDInterceptDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarInterception;
	const char *options [] = { MDInputStr, MDNoneStr, MDCalculateStr, (char *) NULL };

	if (_MDOutInterceptID != MFUnset) return (_MDOutInterceptID);

	MFDefEntering ("Intercept");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutInterceptID = MFVarGetID (MDVarInterception,  "mm", MFInput, MFFlux, false); break;
		case MDcalc:
			if (((_MDInPrecipID        = MDPrecipitationDef ()) == CMfailed) ||
	    	    ((_MDInSPackChgID      = MDSPackChgDef      ()) == CMfailed) ||
	    	    ((_MDInPetID           = MFVarGetID (MDVarPotEvapotrans,  "mm",     MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDInLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex,  MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex,  MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutInterceptID    = MFVarGetID (MDVarInterception,   "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDIntercept) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Intercept");
	return (_MDOutInterceptID); 
}

