/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDIrrGrossDemand.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInIrrAreaFractionID       = MFUnset;
// Output
static int _MDOutIrrGrossDemandID       = MFUnset;
static int _MDOutIrrReturnFlowID        = MFUnset;

static void _MDIrrGrossDemand (int itemID) {
// Input
	float irrAreaFrac;        // Irrigated area fraction [-]
// Output
	float irrGrossDemand;     // Irrigational water demand [mm]
	float IrrReturnFlow;      // Irrigation return flow [mm]

	irrAreaFrac = irrGrossDemand = IrrReturnFlow = itemID; //dummy 
}

enum { MDinput, MDcalculate };

int MDIrrGrossDemandDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarIrrGrossDemand;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };
	
	if (_MDOutIrrGrossDemandID != MFUnset) return (_MDOutIrrGrossDemandID);

	MFDefEntering ("Irrigation Gross Demand");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:
			if (((_MDInIrrAreaFractionID    = MFVarGetID (MDVarIrrAreaFraction,   "%",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrGrossDemandID    = MFVarGetID (MDVarIrrGrossDemand,    "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    	((_MDOutIrrReturnFlowID     = MFVarGetID (MDVarIrrReturnFlow,     "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDcalculate:
			if (((_MDInIrrAreaFractionID    = MFVarGetID (MDVarIrrAreaFraction,   "%",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrGrossDemandID    = MFVarGetID (MDVarIrrGrossDemand,    "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    	((_MDOutIrrReturnFlowID     = MFVarGetID (MDVarIrrReturnFlow,     "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed))
		    	return (CMfailed);
		    _MDOutIrrGrossDemandID = MFVarSetFunction (_MDOutIrrGrossDemandID,_MDIrrGrossDemand);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}	
	MFDefLeaving ("Irrigation Gross Demand");
	return (_MDOutIrrGrossDemandID);
}
