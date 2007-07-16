/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischReference.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAccumDischargeID;
static int _MDInAvgNStepsID;

static int _MDOutDischReferenceID = CMfailed;

static void _MDDischReference (int itemID) {
	int   nSteps;
	float accumDisch;
	float dischRef;

	if (MFVarTestMissingVal (_MDInAccumDischargeID,   itemID) ||
	    MFVarTestMissingVal (_MDInAvgNStepsID,       itemID) ||
	    MFVarTestMissingVal (_MDOutDischReferenceID, itemID)) MFVarSetFloat (_MDOutDischReferenceID, itemID, 0.0);
	else {
		accumDisch = MFVarGetFloat (_MDInAccumDischargeID,  itemID);
		nSteps     = MFVarGetInt   (_MDInAvgNStepsID,       itemID);
		dischRef   = MFVarGetFloat (_MDOutDischReferenceID, itemID);
		dischRef = (float) (((double) dischRef * (double) nSteps + accumDisch) / ((double) (nSteps + 1)));
		MFVarSetFloat (_MDOutDischReferenceID, itemID, dischRef);
	}
}

enum { MDhelp, MDinput, MDcalculate };

int MDDischReferenceDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDVarDischReference;
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutDischReferenceID != CMfailed) return (_MDOutDischReferenceID);
	MFDefEntering ("Reference Discharge");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutDischReferenceID  = MFVarGetID (MDVarDischReference, "m3/s", MFInput, MFState, true); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID      = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInAccumDischargeID = MDAccumRunoffDef ()) == CMfailed) ||
			    ((_MDOutDischReferenceID = MFVarGetID (MDVarDischReference, "m3/s",  MFOutput, MFState, true))  == CMfailed))
			return (CMfailed);
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Reference Discharge");
	return (MFVarSetFunction(_MDOutDischReferenceID,_MDDischReference));
}
