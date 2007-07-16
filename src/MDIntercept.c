/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDIntercept.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDOutInterceptID = CMfailed;
static void _MDInterceptDummy (int itemID) { MFVarSetFloat (_MDOutInterceptID,itemID,0.0); }
static int  _MDInSPackChgID, _MDInPetID, _MDInPrecipID, _MDInCParamCHeightID, _MDInLeafAreaIndexID, _MDInStemAreaIndexID;
static void _MDIntercept (int itemID) {
/* Input */
	float precip;  /* daily precipitation [mm/day] */
	float sPackChg;/* snow pack change [mm/day] */
	float pet;     /* daily potential evapotranspiration [mm/day] */
	float height;  /* canopy height [m] */
 	float lai;     /* projected leaf area index */
	float sai;     /* projected stem area index */
/* Local */
	float epi;     /* daily potential interception [mm/day] */
	float eis;     /* maximum amount of evaporated interception during "storm" [mm] */
	float c;       /* canopy storage capacity [mm] */
/* Output */
	float intercept; /* estimated interception [mm] */ 
	if (MFVarTestMissingVal (_MDInPrecipID,       itemID) ||
		 MFVarTestMissingVal (_MDInSPackChgID,     itemID) ||
		 MFVarTestMissingVal (_MDInPetID,          itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID,itemID) ||
		 MFVarTestMissingVal (_MDInLeafAreaIndexID,itemID) ||
   	 MFVarTestMissingVal (_MDInStemAreaIndexID,itemID)) { MFVarSetMissingVal (_MDOutInterceptID,itemID); return; }
	precip   = MFVarGetFloat (_MDInPrecipID,        itemID);
	pet      = MFVarGetFloat (_MDInPetID,           itemID);

	intercept = 0.0;
	if ((pet > 0.0) && (precip > 0.0)) {
		lai      = MFVarGetFloat (_MDInLeafAreaIndexID, itemID);
		sai      = MFVarGetFloat (_MDInStemAreaIndexID, itemID);
		c = MDConstInterceptCI * (lai + sai) / 2.0;
		if (c > 0.0) {
			sPackChg = MFVarGetFloat (_MDInSPackChgID,      itemID);
			height   = MFVarGetFloat (_MDInCParamCHeightID, itemID);
			if (sPackChg > 0.0) precip = precip - sPackChg;
			epi = pet * (height < MDConstInterceptCH ? 1.0 + height / MDConstInterceptCH : 2.0);
			eis = MDConstInterceptD * epi;
			intercept = precip < (eis + c) ?  precip /* capacity is not reached */ : (eis + c); /* capacity exceeded */
			if (intercept > pet) intercept = pet; /* FBM Addition */
	}
	}
	MFVarSetFloat (_MDOutInterceptID,itemID, intercept);	
}

enum { MDhelp, MDinput, MDnone, MDcalc };

int MDInterceptDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarInterception;
	const char *options [] = { MDHelpStr, MDInputStr, "none", MDCalculateStr, (char *) NULL };

	if (_MDOutInterceptID != CMfailed) return (_MDOutInterceptID);

	MFDefEntering ("Intercept");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutInterceptID = MFVarGetID (MDVarInterception,  "mm", MFInput, MFFlux, false); break;
		case MDnone:
			if	((_MDOutInterceptID     = MFVarGetID (MDVarInterception, "mm",     MFOutput,  MFFlux, false)) == CMfailed)
				return (CMfailed);
			_MDOutInterceptID = MFVarSetFunction (_MDOutInterceptID,_MDInterceptDummy); 
			break;
		case MDcalc:
			if	(((_MDInPrecipID        = MDPrecipitationDef ()) == CMfailed) ||
	    		 ((_MDInSPackChgID      = MFVarGetID (MDVarSnowPackChange, "mm",     MFInput,  MFFlux,  false)) == CMfailed) ||
				 ((_MDInPetID           = MFVarGetID (MDVarPotEvapotrans,  "mm",     MFInput,  MFFlux,  false)) == CMfailed) ||
				 ((_MDInLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDInStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDOutInterceptID    = MFVarGetID (MDVarInterception,   "mm",     MFOutput, MFFlux,  false)) == CMfailed))
				return (CMfailed);
			_MDOutInterceptID = MFVarSetFunction (_MDOutInterceptID,_MDIntercept); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Intercept");
	return (_MDOutInterceptID); 
}
