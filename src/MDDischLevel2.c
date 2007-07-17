/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischCalculated.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Inputs
static int _MDInDischargeID        = MFUnset;
static int _MDInIrrGrossDemandID   = MFUnset;
// Outputs
static int _MDOutIrrUptakeRiverID  = MFUnset;
static int _MDOutIrrUptakeExcessID = MFUnset;
static int _MDOutDischLevel2ID  = MFUnset;

static void _MDDischLevel2 (int itemID) {
// Inputs
	float dishRouted;      // Routed discharge [m3/s]
	float irrGrossDemand;  // Total irrigational water uptake [mm/dt]
// Outputs
	float irrUptakeRiver;  // Irrigational water uptake from river [m3/s]
	float irrUptakeExcess; // Irrigational water uptake from unsustainable source [mm/dt]
	float dicharge;        // Calculated discharge [m3/s]

	if (MFVarTestMissingVal (_MDInDischargeID, itemID)) {
		MFVarSetFloat (_MDOutDischCalculatedID,  itemID, 0.0);
	}
	else {
		discharge = MFVarGetFloat (_MDInDischargeID,  itemID);
		if (_MDInIrrGrossDemandID != MFUnset) {
			if (MFVarTestMissingVal (_MDInIrrGrossDemandID, itemID)) {
				irrUptakeRiver  = 0.0;
				irrUptakeExcess = 0.0;
			}
			else {
				irrTotalUptake = MFVarGetFloat (_MDInIrrGrossDemandID,  itemID);
				if (discharge > irrGrossDemand) {
					irrUptakeRiver  = irrGrossDemand;
					irrUptakeExcess = 0.0;
					discharge = discharge - irrUptakeRiver;
				}
				else {
					irrUptakeRiver  = discharge;
					irrUptakeExcess = irrGrossDemand - discharge;
					discharge = 0.0;
				}
			}
			MFVarSetFloat (_MDOutIrrUptakeRiverID,  itemID, irrUptakeRiver);
			MFVarSetFloat (_MDOutIrrUptakeExcessID, itemID, irrUptakeExcess);
		}
		MFVarSetFloat (_MDOutDischLevel2ID,  itemID, discharge);
	}
}

int MDDischLevel2Def() {
	int optID = MDinput;
	const char *optStr, *optName = MDModIrrigation;
	const char *options [] = { MDnoneStr, (char *) NULL };

	if (_MDOutDischLevel2ID != MFUnset) return (_MDOutDischLevel2ID);

	MFDefEntering ("Discharge Level 2");
	if (((_MDInDischLevel3ID  = MDDischLevel3Def ()) == CMfailed) ||
	    ((_MDOutDischLevel2ID = MFVarGetID (MDVarDischLevel2,  "m/3", MFOutput, MFState, false)) == CMfailed))
	    return (CMfailed);
	if (((optStr = MFOptionGet (optName)) != (char *) NULL) && (CMoptLookup (options,optStr,true) != CMfailed)) {
		if (((_MDInIrrGrossDemandID   = MFVarGetID (MDVarIrrGrossDemand,  "mm",  MFInput,  MFFlux,  false)) == CMfailed) ||
		    ((_MDOutIrrUptakeRiverID  = MFVarGetID (MDVarIrrUptakeRiver,  "m/3", MFInput,  MFState, false)) == CMfailed) ||
		    ((_MDOutIrrUptakeExcessID = MFVarGetID (MDVarIrrUptakeExcess, "mm",  MFOutput, MFFlux,  false)) == CMfailed))
			return (CMfailed);
	}
	MFDefLeaving ("Discharge Level 2");
	return (MFVarSetFunction(_MDOutDischLevel2ID,_MDDischLevel2));
}
