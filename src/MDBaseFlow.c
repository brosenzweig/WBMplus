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
static int _MDInRainSurfRunoffID        = MFUnset;
static int _MDInRechargeID              = MFUnset;
static int _MDInIrrGrossDemandID        = MFUnset;
static int _MDInIrrReturnFlowID         = MFUnset;
static int _MDInIrrAreaFracID           = MFUnset;
static int _MDInSmallResReleaseID		= MFUnset;
// Output
static int _MDOutSurfRunoffID           = MFUnset;
static int _MDOutGrdWatID               = MFUnset;
static int _MDOutGrdWatChgID            = MFUnset;
static int _MDOutBaseFlowID             = MFUnset;
static int _MDOutIrrUptakeGrdWaterID    = MFUnset;
static int _MDOutIrrUptakeExternalID    = MFUnset;

static float _MDGroundWatBETA = 0.016666667;

static void _MDBaseFlow (int itemID) {
// Input
		float recharge;          // Rainfed groundwater recharge over the non-irrigated area [mm/dt]
	float irrDemand;         // Irrigation demand [mm/dt]
	float irrAreaFraction;   // Irrigated area fraction
	float smallResRelease;   // Release from small reservoirs that can be used for irrigation (preferentially!), mm/dt
// Output
	float surfRunoff;        // Surface runoff [mm/dt]
	float grdWater;          // Groundwater size   [mm]
	float grdWaterChg;       // Groundwater change [mm/dt]
	float baseFlow;          // Base flow from groundwater [mm/dt]
	float irrUptakeGrdWater; // Irrigational water uptake from shallow groundwater [mm/dt]
	float irrUptakeExt;      // Unmet irrigational water demand [mm/dt]

	surfRunoff  = MFVarGetFloat (_MDInRainSurfRunoffID, itemID, 0.0);
 	recharge    = MFVarGetFloat (_MDInRechargeID,       itemID, 0.0);
	grdWaterChg = grdWater = MFVarGetFloat (_MDOutGrdWatID, itemID, 0.0);

	if ((_MDInIrrGrossDemandID != MFUnset) &&
	    (_MDInIrrReturnFlowID  != MFUnset) &&
	    (_MDInIrrAreaFracID    != MFUnset) &&
		((irrAreaFraction = MFVarGetFloat (_MDInIrrAreaFracID,     itemID, 0.0)) > 0.0)) {
		surfRunoff = surfRunoff * (1.0 - irrAreaFraction);
		recharge   = recharge   * (1.0 - irrAreaFraction);
	 	recharge   = recharge + MFVarGetFloat (_MDInIrrReturnFlowID,   itemID, 0.0);
		irrDemand  = MFVarGetFloat (_MDInIrrGrossDemandID,  itemID, 0.0);
		
		smallResRelease = _MDInSmallResReleaseID != MFUnset ? MFVarGetFloat(_MDInSmallResReleaseID,itemID,0.0) : 0.0;

		if (irrDemand < grdWater + smallResRelease) {
			// Irrigation demand is satisfied from small reservoir and groundwater storage 
			irrUptakeGrdWater = irrDemand - smallResRelease;
			irrUptakeExt = 0.0;
			grdWater = grdWater - irrUptakeGrdWater;
		}
		else {
			// Irrigation demand needs external source
			irrUptakeGrdWater = grdWater;
		    irrUptakeExt      = irrDemand - irrUptakeGrdWater - smallResRelease;
		    grdWater = 0.0;
		}		
		MFVarSetFloat (_MDOutIrrUptakeGrdWaterID, itemID, irrUptakeGrdWater);
		MFVarSetFloat (_MDOutIrrUptakeExternalID, itemID, irrUptakeExt);
	}

	if (grdWater + recharge > 0.0) {
		baseFlow = (grdWater + recharge) * exp (-_MDGroundWatBETA);
		grdWater = grdWater + recharge - baseFlow;
		grdWaterChg = grdWater - grdWaterChg;
	}
	else {
		grdWaterChg = baseFlow = 0.0;
	}
	MFVarSetFloat (_MDOutSurfRunoffID, itemID, surfRunoff);
	MFVarSetFloat (_MDOutGrdWatID,     itemID, grdWater);
    MFVarSetFloat (_MDOutGrdWatChgID,  itemID, grdWaterChg);
	MFVarSetFloat (_MDOutBaseFlowID,   itemID, baseFlow);
}

int MDBaseFlowDef () {
	float par;
	const char *optStr;

	if (_MDOutBaseFlowID != MFUnset) return (_MDOutBaseFlowID);

	MFDefEntering ("Base flow");
	if (((optStr = MFOptionGet (MDParGroundWatBETA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDGroundWatBETA = par;

	if (((_MDInRechargeID       = MDRainInfiltrationDef ()) == CMfailed) ||
	    ((_MDInIrrGrossDemandID = MDIrrGrossDemandDef   ()) == CMfailed)) return (CMfailed);
	if ( _MDInIrrGrossDemandID != MFUnset) {
		if (((_MDInSmallResReleaseID    = MDSmallReservoirReleaseDef ()) == CMfailed) ||
		    ((_MDInIrrReturnFlowID      = MFVarGetID (MDVarIrrReturnFlow,     "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeGrdWaterID = MFVarGetID (MDVarIrrUptakeGrdWater, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDOutIrrUptakeExternalID = MFVarGetID (MDVarIrrUptakeExternal, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
		    ((_MDInIrrAreaFracID        = MFVarGetID (MDVarIrrAreaFraction,   "-",  MFInput,  MFState, MFBoundary)) == CMfailed))
			return CMfailed;
	}
	if (((_MDInRainSurfRunoffID         = MFVarGetID (MDVarRainSurfRunoff,    "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSurfRunoffID            = MFVarGetID (MDVarSurfRunoff,        "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutGrdWatID                = MFVarGetID (MDVarGroundWater,       "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutGrdWatChgID             = MFVarGetID (MDVarGroundWaterChange, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutBaseFlowID              = MFVarGetID (MDVarBaseFlow,          "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDBaseFlow) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Base flow ");
	return (_MDOutBaseFlowID);
}


 



