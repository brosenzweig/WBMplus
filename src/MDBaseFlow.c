/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDBaseFlow.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRechargeID              = MFUnset;
static int _MDInIrrGrossDemandID        = MFUnset;
static int _MDInIrrReturnFlowID         = MFUnset;
// Output
static int _MDOutGrdWatID               = MFUnset;
static int _MDOutGrdWatChgID            = MFUnset;
static int _MDOutBaseFlowID             = MFUnset;
static int _MDOutIrrUptakeGrdWaterID    = MFUnset;
static int _MDOutIrrUptakeExternalID    = MFUnset;

static float _MDGroundWatBETA  = 0.016666667;
 
static float _MDRecharge;

float _MDGroundWaterFunc (float t, float grdH2O) { return (_MDRecharge - _MDGroundWatBETA * grdH2O); }

static void _MDBaseFlow (int itemID) {
// Input
	float irrDemand;          // Irrigation demand [mm/dt]
	float irrReturnFlow;      // Irrigation return flow [mm/dt]
// Output
	float grdWater;           // Groundwater size   [mm]
	float grdWaterChg;        // Groundwater change [mm/dt]
	float irrUptakeGrdWater;  // Irrigational water uptake from shallow groundwater [mm/dt]
	float IrrUptakeExt;       // Unmet irrigational water demand [mm/dt]
// Local
	float baseFlow;           // Base flow from groundwater [mm/dt]

	grdWaterChg = grdWater    = MFVarGetFloat (_MDOutGrdWatID,  itemID, 0.0);

	if ((_MDInIrrGrossDemandID != MFUnset) && (_MDInIrrReturnFlowID  != MFUnset)) {
	// Abstraction here.....	 
		irrDemand      = MFVarGetFloat (_MDInIrrGrossDemandID,  itemID, 0.0);
		irrReturnFlow  = MFVarGetFloat (_MDInIrrReturnFlowID,   itemID, 0.0);

		if (irrDemand >= grdWater) {
			irrUptakeGrdWater = grdWater;
			IrrUptakeExt      = irrDemand - grdWater;
			grdWater = 0.0;
		}
		else {
			irrUptakeGrdWater = irrDemand;
			grdWater = grdWater - irrDemand;
			IrrUptakeExt = 0.0;
		}
		grdWater += irrReturnFlow;
		grdWaterChg = grdWaterChg + irrReturnFlow - irrUptakeGrdWater;
		MFVarSetFloat (_MDOutIrrUptakeGrdWaterID, itemID, irrUptakeGrdWater);
		MFVarSetFloat (_MDOutIrrUptakeExternalID, itemID, IrrUptakeExt);
	}

	baseFlow    = _MDRecharge = MFVarGetFloat (_MDInRechargeID, itemID, 0.0) ;
	if (grdWater + _MDRecharge > MFMathEpsilon) {
		if ((grdWater = MFRungeKutta ((float) 0.0,1.0, grdWater,_MDGroundWaterFunc)) < 0.0) grdWater = 0.0;
		grdWaterChg = grdWater - grdWaterChg;
		baseFlow    = baseFlow - grdWaterChg;
	}
	else grdWaterChg = baseFlow = 0.0;

	// in mm!
	MFVarSetFloat (_MDOutGrdWatID,            itemID, grdWater);
	MFVarSetFloat (_MDOutGrdWatChgID,         itemID, grdWaterChg);
	MFVarSetFloat (_MDOutBaseFlowID,          itemID, baseFlow);
}

int MDBaseFlowDef () {
	const char *optStr;
	const char *options [] = { MDNoneStr, (char *) NULL };
	float par;

	if (_MDOutBaseFlowID != MFUnset) return (_MDOutBaseFlowID);

	MFDefEntering ("Base flow");
	if ((_MDInRechargeID        = MDInfiltrationDef ())   == CMfailed) return (CMfailed);

	if (((optStr = MFOptionGet (MDParGroundWatBETA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDGroundWatBETA = par;

	if (((optStr = MFOptionGet (MDOptIrrigation)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((_MDInIrrGrossDemandID     = MDIrrGrossDemandDef ()) == CMfailed) ||
		    ((_MDInIrrReturnFlowID      = MFVarGetID (MDVarIrrReturnFlow,     "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeGrdWaterID = MFVarGetID (MDVarIrrUptakeGrdWater, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeExternalID = MFVarGetID (MDVarIrrUptakeExternal, "mm", MFOutput, MFState, MFBoundary)) == CMfailed))
			return (CMfailed);
	}
	if (((_MDOutGrdWatID                = MFVarGetID (MDVarGroundWater,       "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutGrdWatChgID             = MFVarGetID (MDVarGroundWaterChange, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutBaseFlowID              = MFVarGetID (MDVarBaseFlow,          "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDBaseFlow) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Base flow ");
	return (_MDOutBaseFlowID);
}
