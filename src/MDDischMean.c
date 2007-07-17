/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischMean.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAccumDischargeID;
static int _MDInAvgNStepsID;

static int _MDOutDischMeanID = CMfailed;

static void _MDDischMean (int itemID) {
	int   nSteps;
	float accumDisch;
	float dischMean;

	if (MFVarTestMissingVal (_MDInAccumDischargeID,   itemID) ||
	    MFVarTestMissingVal (_MDInAvgNStepsID,       itemID) ||
	    MFVarTestMissingVal (_MDOutDischMeanID, itemID)) MFVarSetFloat (_MDOutDischMeanID, itemID, 0.0);
	else {
		accumDisch = MFVarGetFloat (_MDInAccumDischargeID,  itemID);
		nSteps     = MFVarGetInt   (_MDInAvgNStepsID,       itemID);
		dischMean  = MFVarGetFloat (_MDOutDischMeanID, itemID);
		dischMean  = (float) (((double) dischMean * (double) nSteps + accumDisch) / ((double) (nSteps + 1)));
		MFVarSetFloat (_MDOutDischMeanID, itemID, dischMean);
	}
}

enum { MDhelp, MDinput, MDcalculate };

int MDDischMeanDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDVarDischMean;
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutDischMeanID != CMfailed) return (_MDOutDischMeanID);
	MFDefEntering ("Discharge Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutDischMeanID  = MFVarGetID (MDVarDischMean, "m3/s", MFInput,  MFState, false); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID       = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInAccumDischargeID  = MDAccumRunoffDef ()) == CMfailed) ||
			    ((_MDOutDischMeanID      = MFVarGetID (MDVarDischMean, "m3/s", MFOutput, MFState, true))  == CMfailed))
				return (CMfailed);
			_MDOutDischMeanID =	MFVarSetFunction(_MDOutDischMeanID,_MDDischMean)
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Discharge Mean");
	return (_MDOutDischMeanID);
}
