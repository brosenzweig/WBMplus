/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDBaseFlow.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

// Input
static int _MDInRechargeID              = MFUnset;
static int _MDInIrrGrossDemandID        = MFUnset;
static int _MDInIrrReturnFlowID         = MFUnset;
static int _MDInIrrAreaFractionID       = MFUnset;
// Output
static int _MDOutGrdWatID               = MFUnset;
static int _MDOutGrdWatChgID            = MFUnset;
static int _MDOutBaseFlowID             = MFUnset;
static int _MDOutIrrUptakeGrdWaterID    = MFUnset;
static int _MDOutIrrUptakeExtID         = MFUnset;

static float _MDGroundWatBETA  = 0.016666667;
 
static float _MDRecharge;

float _MDGroundWaterFunc (float t, float grdH2O) { return (_MDRecharge - _MDGroundWatBETA * grdH2O); }

static void _MDBaseFlow (int itemID) {
// Input
	float irrDemand;
	float irrReturnFlow;
// Output
	float grdWater;
	float grdWaterChg = 0.0; 
	float groundWaterAbstraction;
	floIrrUptakeExtion;
// Local
	float baseFlow;
	float irrArea;

	if ((MFVarTestMissingVal (_MDInRechargeID,              itemID)) ||
	    (MFVarTestMissingVal (_MDInIrrGrossDemandID, itemID)) ||
	    (MFVarTestMissingVal (_MDInIrrReturnFlowID,         itemID)) ||
	    (MFVarTestMissingVal (_MDOutGrdWatID,               itemID))) {
		MFVarSetMissingVal (_MDOutGrdWatID,               itemID);
		MFVarSetMissingVal (_MDOutGrdWatChgID,            itemID);
		MFVarSetMissingVal (_MDOutBaseFlowID,             itemID);
		MFVarSetMissingVal (_MDOutIrrUptakeGrdWaterID,  itemID);
		MFVarSetMissingVal (_MDOutIrrUptakeExtID, itemID);
		return;
	}
	if (  MFVarTestMissingVal(_MDInIrrAreaFractionID, itemID)) irrArea = 0.0;
	else irrArea = MFVarGetFloat(_MDInIrrAreaFractionID,itemID) * MFModalGetArea (itemID);
	baseFlow    = _MDRecharge = MFVarGetFloat (_MDInRechargeID, itemID);
	grdWaterChg = grdWater    = MFVarGetFloat (_MDOutGrdWatID,  itemID);

	if (grdWater + _MDRecharge > MFMathEpsilon) {
		if ((grdWater = MFRungeKutta ((float) 0.0,1.0, grdWater,_MDGroundWaterFunc)) < 0.0) grdWater = 0.0;
		grdWaterChg = grdWater - grdWaterChg;
		baseFlow    = baseFlow - grdWaterChg;
	}
	else grdWaterChg = baseFlow = 0.0;
	
// Abstraction here..........................		
	if (irrArea > 0) {
//	grdWaterChg=grdWater;
	irrDemand            = MFVarGetFloat (_MDInIrrGrossDemandID, itemID);
	irrReturnFlow        = MFVarGetFloat (_MDInIrrReturnFlowID,         itemID);

	//	if (itemID==2001)printf("InBaseFlow:gwAbst %f demand %f grdWater %f Return=%f \n", groundWaterAbstraction,irrDemand,grdWater,irrReturnFlow);
	if (irrDemand >= grdWater) {
		groundWaterAbstraction = grdWater;
		IrrUptakeExt   = irrDemand - grdWater;
		grdWater = 0.0;
	}
	else {
		groundWaterAbstraction = irrDemand;
		grdWater = grdWater - irrDemand;
		IrrUptakeExt = 0.0;
	}

	grdWater+=irrReturnFlow;
	grdWaterChg=grdWaterChg + irrReturnFlow - groundWaterAbstraction;



}
else
{
	IrrUptakeExt=0.0;
	groundWaterAbstraction=0.0;
	irrReturnFlow=0.0;
	}

//if (itemID==2001) printf ("BASEFLOW:grdWaterChg %f, gWAbstr %f DisAbst%f irrArea=%f grdWater=%f irrDemand=%f\n",grdWaterChg, groundWaterAbstraction,IrrUptakeExt,irrArea,grdWater,irrDemand);


//if (itemID==4412) printf ("in BaseFlow: irrDemand %f irrReturnFlow %f grdWater %f grdWaterChg %f, gWAbstr %f DisAbst%f \n",irrDemand, irrReturnFlow,grdWater,grdWaterChg, groundWaterAbstraction,IrrUptakeExt);


	if (IrrUptakeExt <0)printf ("DischAbstr negativ in MDBaseFlow! %f\n", IrrUptakeExt);

float in = _MDRecharge + irrReturnFlow; 
float out = baseFlow + groundWaterAbstraction;
//grdWaterChg=in-out;
float watBal=in-out-grdWaterChg;
if (fabs(watBal)>0.1){
printf ("BaseFlowWaterBalance!!!=%f  in = %f out=%f GWChange %f Return %f gwAbstr %f irrArea %f item %i  baseFlow %f \n",watBal,in,out,grdWaterChg, irrReturnFlow,groundWaterAbstraction,irrArea,itemID,baseFlow);
}


	//alles in mm!
	MFVarSetFloat (_MDOutGrdWatID,               itemID, grdWater);
	MFVarSetFloat (_MDOutGrdWatChgID,            itemID, grdWaterChg);
	MFVarSetFloat (_MDOutBaseFlowID,             itemID, baseFlow);
	MFVarSetFloat (_MDOutIrrUptakeGrdWaterID,  itemID, groundWaterAbstraction);
	MFVarSetFloat (_MDOutIrrUptakeExtID, itemID, IrrUptakeExt); 
}

int MDBaseFlowDef () {
	const char *optStr;
	float par;

	if (_MDOutBaseFlowID != MFUnset) return (_MDOutBaseFlowID);
	MFDefEntering ("Base flow");

	if ((_MDInRechargeID        = MDInfiltrationDef ())   == CMfailed) return (CMfailed);
	 
	if (((optStr = MFOptionGet (MDParGroundWatBETA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDGroundWatBETA = par;
//	if ((_MDInIrrGrossDemandID = MDIrrigationWaterDef()) == CMfailed)return CMfailed;
	if ((_MDInIrrGrossDemandID     = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand,          "mm", MFInput, MFState, false)) == CMfailed) return (CMfailed);
	if ((_MDInIrrReturnFlowID      = MFVarGetID (MDVarIrrPercolationWater,    "mm", MFInput,  MFState, MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutGrdWatID            = MFVarGetID (MDVarGroundWater,            "mm", MFOutput, MFState, MFInitial))  == CMfailed) return CMfailed;
	if ((_MDOutGrdWatChgID         = MFVarGetID (MDVarGroundWaterChange,      "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutBaseFlowID          = MFVarGetID (MDVarBaseFlow,               "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutIrrUptakeGrdWaterID = MFVarGetID (MDVarGroundWaterAbstraction, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) return CMfailed;
	if ((_MDOutIrrUptakeExtID      = MFVarGetID (MDVarIrrUptakeExt,           "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) return CMfailed;
    if ((_MDInIrrAreaFractionID    = MFVarGetID (MDVarIrrAreaFraction,        "-",  MFInput,  MFState, MFBoundary)) == CMfailed) return (CMfailed);
	_MDOutBaseFlowID = MFVarSetFunction (_MDOutBaseFlowID,_MDBaseFlow);
			
	MFDefLeaving ("Base flow");
	return (_MDOutBaseFlowID);
}
