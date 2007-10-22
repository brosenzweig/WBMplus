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
static int _MDInIrrAreaID               = MFUnset;
static int _MDInSmallResReleaseID		= MFUnset;
// Output
static int _MDOutGrdWatID               = MFUnset;
static int _MDOutGrdWatChgID            = MFUnset;
static int _MDOutBaseFlowID             = MFUnset;
static int _MDOutIrrUptakeGrdWaterID    = MFUnset;
static int _MDOutIrrUptakeExternalID    = MFUnset;

static float _MDGroundWatBETA = 0.016666667;
static float _MDRecharge;

float _MDGroundWaterFunc (float t, float grdH2O) { return (_MDRecharge - _MDGroundWatBETA * grdH2O); }

static void _MDBaseFlow (int itemID) {
// Input
	float irrDemand        = 0.0; // Irrigation demand [mm/dt]
	float irrReturnFlow    = 0.0; // Irrigation return flow [mm/dt]
	float smallResRelease  = 0.0; // Release from small reservoirs that can be used for irrigation (preferentially!), mm/dt
// Output
	float grdWater          = 0.0; // Groundwater size   [mm]
	float grdWaterChg       = 0.0; // Groundwater change [mm/dt]
	float irrUptakeGrdWater = 0.0; // Irrigational water uptake from shallow groundwater [mm/dt]
	float IrrUptakeExt      = 0.0; // Unmet irrigational water demand [mm/dt]
// Local
	float baseFlow          = 0.0; // Base flow from groundwater [mm/dt]
	float irrAreaFraction   = 0.0;
	float baseFlowBalance   = 0.0;
	float prevGW;

	grdWaterChg = grdWater = prevGW = MFVarGetFloat (_MDOutGrdWatID,  itemID, 0.0);

	if ((_MDInIrrGrossDemandID != MFUnset) && (_MDInIrrReturnFlowID  != MFUnset)) {
	// Abstraction here..
		irrAreaFraction = MFVarGetFloat(_MDInIrrAreaID,itemID,0.0);
		irrDemand       = MFVarGetFloat (_MDInIrrGrossDemandID,  itemID, 0.0);
	 	irrReturnFlow   = MFVarGetFloat (_MDInIrrReturnFlowID,   itemID, 0.0);
		
		if (_MDInSmallResReleaseID != MFUnset) smallResRelease = MFVarGetFloat(_MDInSmallResReleaseID,itemID,0.0);
		
		if (smallResRelease < 0) printf ("SR release %f\n",smallResRelease);

		if (irrDemand <= smallResRelease) {
			IrrUptakeExt=0.0;
			irrUptakeGrdWater=0.0;
		}
				
		else if ((irrDemand) > grdWater+smallResRelease) {
			irrUptakeGrdWater = grdWater-(smallResRelease);
		    IrrUptakeExt      = (irrDemand - irrUptakeGrdWater-smallResRelease);
		}
		else   {
			irrUptakeGrdWater = irrDemand-smallResRelease;
			IrrUptakeExt = 0.0;
		}
		 
		
		grdWater = grdWater+irrReturnFlow-irrUptakeGrdWater;
		grdWaterChg = grdWaterChg + irrReturnFlow - irrUptakeGrdWater;
		baseFlowBalance= irrReturnFlow -irrUptakeGrdWater+ grdWaterChg;
		
		MFVarSetFloat (_MDOutIrrUptakeGrdWaterID, itemID, irrUptakeGrdWater);
		MFVarSetFloat (_MDOutIrrUptakeExternalID, itemID, IrrUptakeExt);
	}
 	baseFlow    = _MDRecharge = MFVarGetFloat (_MDInRechargeID, itemID, 0.0) ;

	if (grdWater + _MDRecharge > 0.0) {
		if ((grdWater = MFRungeKutta ((float) 0.0,1.0, grdWater,_MDGroundWaterFunc)) < 0.0) grdWater = 0.0;
		grdWaterChg = grdWater - grdWaterChg;
		baseFlow    = _MDRecharge = MFVarGetFloat (_MDInRechargeID, itemID, 0.0);
		
		baseFlow    = baseFlow - grdWaterChg;
	}
	else {
		grdWaterChg = baseFlow = 0.0;
	}

	baseFlowBalance = _MDRecharge - baseFlow - grdWaterChg;
	if (fabs(baseFlowBalance)>0.0001)printf ("BaseFlowwaterBalcne!= %f Recharge %f,baseflow %f  irrreturnflow %f irrUptakeGW %f GWChange %f itemID %i\n",baseFlowBalance,_MDRecharge,baseFlow,irrReturnFlow,irrUptakeGrdWater,grdWaterChg,itemID);
	if ((baseFlow)<-0.001)printf ("BaseFlowNEGASTIVE= %f Recharge %f,baseflow %f  irrreturnflow %f irrUptakeGW %f GWChange %f itemID %i\n",baseFlowBalance,_MDRecharge,baseFlow,irrReturnFlow,irrUptakeGrdWater,grdWaterChg,itemID);
	
	// in mm!
	MFVarSetFloat (_MDOutGrdWatID,            itemID, grdWater);
    MFVarSetFloat (_MDOutGrdWatChgID,         itemID, grdWaterChg);
	MFVarSetFloat (_MDOutBaseFlowID,          itemID, baseFlow);
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
		    ((_MDInIrrAreaID            = MFVarGetID (MDVarIrrAreaFraction,   "-",  MFInput,  MFState, MFBoundary)) == CMfailed))
			return CMfailed;
	}
	if (((_MDOutGrdWatID                = MFVarGetID (MDVarGroundWater,       "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutGrdWatChgID             = MFVarGetID (MDVarGroundWaterChange, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutBaseFlowID              = MFVarGetID (MDVarBaseFlow,          "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDBaseFlow) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Base flow ");
	return (_MDOutBaseFlowID);
}


 



