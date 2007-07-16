/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDRunoff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

/* Input */
static int _MDInSurfaceROID, _MDInBaseFlowID;
/* Output */
static int _MDOutRunoffID = CMfailed;

static void _MDRunoff (int itemID) {
// if (MFVarTestMissingVal (_MDInBaseFlowID, itemID)) printf("missing baseflow in runoff!\n");
	/* Input */
	float baseFlow;
	float surfaceRO;

	if (MFVarTestMissingVal (_MDInSurfaceROID, itemID) && MFVarTestMissingVal (_MDInBaseFlowID, itemID))
	 	MFVarSetMissingVal (_MDOutRunoffID, itemID);
	else {
		baseFlow  = MFVarGetFloat (_MDInBaseFlowID,itemID);
		surfaceRO = MFVarGetFloat (_MDInSurfaceROID,itemID);
		MFVarSetFloat (_MDOutRunoffID, itemID, baseFlow + surfaceRO);
	}
}
 
enum { MDhelp, MDinput, MDcalculate };

int MDRunoffDef () {
	int  optID = MDinput;
	const char *optStr, *optName = "Runoff";
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };
	

	if (_MDOutRunoffID != CMfailed) return (_MDOutRunoffID);

	MFDefEntering ("Runoff");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput: _MDOutRunoffID = MFVarGetID (MDVarRunoff, "mm", MFInput, MFFlux, false); break;
		case MDcalculate:
	
		
			if (((_MDInBaseFlowID  = MDBaseFlowDef ()) == CMfailed) ||
			    ((_MDInSurfaceROID = MFVarGetID (MDVarSurfaceRO, "mm", MFInput,  MFFlux, false)) == CMfailed) ||
				 ((_MDOutRunoffID   = MFVarGetID (MDVarRunoff,    "mm", MFOutput, MFFlux, false)) == CMfailed))
				return (CMfailed);
			_MDOutRunoffID = MFVarSetFunction(_MDOutRunoffID,_MDRunoff);
			break;
		default:
			
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving  ("Runoff");
	return (_MDOutRunoffID);
}
