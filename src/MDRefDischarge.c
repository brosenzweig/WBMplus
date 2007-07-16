/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRefDischarge.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAccumDischargeID;
static int _MDInAvgNStepsID;

static int _MDOutRefDischargeID = CMfailed;

static void _MDRefDischarge (int itemID) {
	int   nSteps;
	float accumDisch;
	float refDisch;

	if (MFVarTestMissingVal (_MDInAccumDischargeID, itemID) ||
		 MFVarTestMissingVal (_MDInAvgNStepsID,      itemID) ||
		 MFVarTestMissingVal (_MDOutRefDischargeID,  itemID))
		MFVarSetFloat (_MDOutRefDischargeID, itemID, 0.0);
	else {
		accumDisch = MFVarGetFloat (_MDInAccumDischargeID,itemID);
		nSteps     = MFVarGetInt   (_MDInAvgNStepsID,     itemID);
		refDisch   = MFVarGetFloat (_MDOutRefDischargeID, itemID);
		refDisch = (refDisch * (float) nSteps + accumDisch) / ((float) (nSteps + 1));
		MFVarSetFloat (_MDOutRefDischargeID, itemID, refDisch);
	}
}

enum { MDhelp, MDinput, MDcalculate };

int MDRefDischargeDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDVarRefDischarge;
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRefDischargeID != CMfailed) return (_MDOutRefDischargeID);
	MFDefEntering ("Reference Discharge");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutRefDischargeID  = MFVarGetID (MDVarRefDischarge, "m3/s", MFInput, MFState, true); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID      = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInAccumDischargeID = MDAccumRunoffDef ()) == CMfailed) ||
			    ((_MDOutRefDischargeID  = MFVarGetID (MDVarRefDischarge, "m3/s",  MFOutput, MFState, true))  == CMfailed))
			return (CMfailed);
			_MDOutRefDischargeID = MFVarSetFunction(_MDOutRefDischargeID,_MDRefDischarge);
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Reference Discharge");
	return (_MDOutRefDischargeID);
}
