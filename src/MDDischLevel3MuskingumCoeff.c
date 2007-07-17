/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDichRouteMuskCoeff.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInDischargeID             = MFUnset;
static int _MDInRiverbedAvgDepthMeanID  = MFUnset;
static int _MDInRiverbedWidthMeanID     = MFUnset;
static int _MDInRiverbedShapeExponentID = MFUnset;
static int _MDInRiverbedSlopeID         = MFUnset;
// Output
static int _MDOutMuskingumC0ID          = MFUnset;
static int _MDOutMuskingumC1ID          = MFUnset;
static int _MDOutMuskingumC2ID          = MFUnset;

static void _MDDischRouteMuskingumCoeff (int itemID) {
// Input
	float discharge;        // Discharge [m3/s]
	float yMean;            // Average depth at mean discharge [m]
	float wMean;            // River width at mean discharge [m]
	float beta;             // Riverbed shape exponent []
	float slope;            // Riverbed slope [m/km]
// Output
	float C0;               // Muskingum C0 coefficient (current inflow)
	float C1;               // Muskingum C1 coefficient (previous inflow)
	float C2;               // MUskingum C2 coefficient (previous outflow)
// Local
	float xi;               // Flood-wave/flow velocity ratio
	float C;                // Cell Courant number;
	float D;                // Cell Reynolds number;
	float dt;               // time step length [s]
	float dL;               // Cell length [m]
	
	dL = MFModelGetLength (itemID);
	if (MFMathEqualValues (dL, 0.0) ||                                                            // Zero length river reach
	    MFVarTestMissingVal (_MDInRiverbedSlopeID,         itemID) ||                             // Missing slope
	    MFVarTestMissingVal (_MDInDischargeID,             itemID) ||                             // Missing discharge
	    MFVarTestMissingVal (_MDInRiverbedAvgDepthMeanID,  itemID) ||                             // Missing average depth at mean discharge
	    MFVarTestMissingVal (_MDInRiverbedWidthMeanID,     itemID) ||                             // Missing width at mean discharge
	    MFVarTestMissingVal (_MDInRiverbedShapeExponentID, itemID) ||                             // Missing riverbed shape exponent 
	    MFMathEqualValues (discharge = fabs(MFVarGetFloat(_MDInDischargeID, itemID)),     0.0) || // Zero discharge
	    MFMathEqualValues (slope = MFVarGetFloat(_MDInRiverbedSlopeID, itemID) / 1000.0,  0.0) || // Zero slope
	    MFMathEqualValues (yMean = MFVarGetFloat(_MDInRiverbedAvgDepthMeanID,    itemID), 0.0) || // Zero average depth at mean discharge
	    MFMathEqualValues (wMean = MFVarGetFloat(_MDInRiverbedWidthMeanID,       itemID), 0.0) || // Zero width at mean discharge
	    ((beta = MFVarGetFloat(_MDInRiverbedShapeExponentID, itemID)) < 0.0)) {                   // Negative riverbed shape exponent
	    // Falling back to flow-accumulation
		MFVarSetFloat (_MDOutMuskingumC0ID, itemID, 1.0);
		MFVarSetFloat (_MDOutMuskingumC1ID, itemID, 0.0);
		MFVarSetFloat (_MDOutMuskingumC2ID, itemID, 0.0);
		return;
	}
	dt = MFModelGet_dt (); 

	xi = 1 + beta * (2.0 / 3.0) / (beta + 1);
	C = xi * (discharge / (yMean * wMean)) * dt / dL;
	D = yMean / (dL * slope * xi);

	C0 = (-1 + C + D) / (1 + C + D);
	C1 = ( 1 + C - D) / (1 + C + D);
	C2 = ( 1 - C + D) / (1 + C + D);
	if ((C0 < 0) || (C1 < 0) || (C2 < 0)) { C0 = 1.0; C1 = 0; C2 = 0; } // We need to investigat this
	C0 = 1.0; C1 = 0; C2 = 0; // temporary for testing
	MFVarSetFloat (_MDOutMuskingumC0ID, itemID, C0);
	MFVarSetFloat (_MDOutMuskingumC1ID, itemID, C1);
	MFVarSetFloat (_MDOutMuskingumC2ID, itemID, C2);
}

enum { MDinput, MDstatic, MDdynamic };

int MDDischLevel3MuskingumCoeffDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDOptMuskingum;
	const char *options [] = { MDInputStr, "static", "dynamic", (char *) NULL };

	if (_MDOutMuskingumC0ID != MFUnset) return (_MDOutMuskingumC0ID);

	MFDefEntering ("Muskingum Coefficients");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			if (((_MDOutMuskingumC0ID = MFVarGetID (MDVarMuskingumC0, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC1ID = MFVarGetID (MDVarMuskingumC1, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC2ID = MFVarGetID (MDVarMuskingumC2, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDstatic:
			if ((_MDInDischargeID = MDDischMeanDef ()) == CMfailed) return (CMfailed);
		case MDdynamic:
			if (_MDInDischargeID == MFUnset) {
				_MDInDischargeID = MFVarGetID (MDVarDischarge,                      "m3/s", MFInput,  MFState, false);
			}
			if ((_MDInDischargeID == CMfailed) || 
			    ((_MDInRiverbedShapeExponentID  = MDRiverbedShapeExponentDef ()) == CMfailed) ||
			    ((_MDInRiverbedWidthMeanID      = MFVarGetID (MDVarRiverbedWidthMean,    "m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRiverbedAvgDepthMeanID   = MFVarGetID (MDVarRiverbedAvgDepthMean, "m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRiverbedSlopeID          = MFVarGetID (MDVarRiverbedSlope,     "m/km", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC0ID           = MFVarGetID (MDVarMuskingumC0,     MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC1ID           = MFVarGetID (MDVarMuskingumC1,     MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC2ID           = MFVarGetID (MDVarMuskingumC2,     MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
				_MDOutMuskingumC0ID = MFVarSetFunction(_MDOutMuskingumC0ID,_MDDischRouteMuskingumCoeff);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Muskingum Coefficients");
	return (_MDOutMuskingumC0ID);
}
