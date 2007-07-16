/******************************************************************************

GHAAS Water Balance Model Library V1.0
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
static int _MDOutDischCalculateID  = MFUnset;

static void _MDDischCalculate (int itemID) {
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
		MFVarSetFloat (_MDOutRoutedDischargeID,  itemID, discharge);
	}
}

int MDDischCalculateDef() {
	int optID = MDinput;
	const char *optStr, *optName = MDModIrrigation;
	const char *options [] = { MDnoneStr, (char *) NULL };

	if (_MDOutDischCalculatedID != MFUnset) return (_MDOutDischCalculatedID);

	MFDefEntering ("Discharge Calculated");
	if (((_MDInDischRoutedID = MDDischRouteDef ()) == CMfailed) ||
	    ((_MDOutDischCalculatedID = MFVarGetID (MDVarDischCalculated,  "m/3", MFOutput, MFState, false)) == CMfailed))
	    return (CMfailed);
	if (((optStr = MFOptionGet (optName)) != (char *) NULL) && (CMoptLookup (options,optStr,true) != CMfailed)) {
		if (((_MDInIrrGrossDemandID   = MFVarGetID (MDVarIrrGrossDemand,  "mm",  MFInput,  MFFlux,  false)) == CMfailed) ||
		    ((_MDOutIrrUptakeRiverID  = MFVarGetID (MDVarIrrUptakeRiver,  "m/3", MFInput,  MFState, false)) == CMfailed) ||
		    ((_MDOutIrrUptakeExcessID = MFVarGetID (MDVarIrrUptakeExcess, "mm",  MFOutput, MFFlux,  false)) == CMfailed))
			return (CMfailed);
	}
	MFDefLeaving ("Discharge Calculated");
	return (MFVarSetFunction(_MDOutDischCalculatedID,_MDDischCalculate));
}
