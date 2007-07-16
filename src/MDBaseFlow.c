/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDBaseFlow.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

/* Input */
static int _MDInRechargeID;
static int _MDInGrossIrrigationDemandID;
static int _MDInIrrReturnFlowID;
static int _MDInActIrrAreaID;
/* Output */
static int _MDOutGrdWatID, _MDOutGrdWatChgID, _MDOutBaseFlowID = CMfailed;
static int _MDOutGrdWaterAbstractionID;
static int _MDOutDischargeAbstractionID;


static float _MDGroundWatBETA  = 0.016666667;
 
static float _MDRecharge;

float _MDGroundWaterFunc (float t,float grdH2O) { return (_MDRecharge - _MDGroundWatBETA * grdH2O); }

static void _MDBaseFlow (int itemID) {
/* Input */
	float irrDemand;
	float irrReturnFlow;
/* Output */
	float grdWater;
	float grdWaterChg = 0.0; 
	float groundWaterAbstraction;
	float dischargeAbstraction;
/* Local */
	float baseFlow;
	float irrArea;
/*
	  if (MFVarTestMissingVal (_MDInRechargeID,              itemID) )printf ("Missing _MDInRechargeID\n");
	  if (   MFVarTestMissingVal (_MDInGrossIrrigationDemandID, itemID)) printf ("Missing _MDInGrossIrrigationDemandID\n");
	  if (   MFVarTestMissingVal (_MDInIrrReturnFlowID,         itemID)) printf ("Missing _MDInIrrReturnFlowID\n");
	  if (   MFVarTestMissingVal (_MDOutGrdWatID,               itemID)) printf ("Missing _MDOutGrdWatID\n");
	  if (  MFVarTestMissingVal (_MDOutDischargeAbstractionID, itemID))printf ("Missing _MDOutDischargeAbstractionID\n"); 
	*/
	
	
		 
	if ((MFVarTestMissingVal (_MDInRechargeID,              itemID) ) ||
	  (   MFVarTestMissingVal (_MDInGrossIrrigationDemandID, itemID))||
	  (   MFVarTestMissingVal (_MDInIrrReturnFlowID,         itemID))||
	  (   MFVarTestMissingVal (_MDOutGrdWatID,               itemID))||
	  (  MFVarTestMissingVal(_MDInActIrrAreaID,		itemID))||
	  (  MFVarTestMissingVal (_MDOutDischargeAbstractionID, itemID))) 
	{
	//	printf ("Missing vars in MDBaseflow! _MDInRechargeID%i  _MDInGrossIrrigationDemandID%i _MDInIrrReturnFlowID%i _MDOutGrdWatID%i _MDOutDischargeAbstractionID %i  \n",_MDInRechargeID,  _MDInGrossIrrigationDemandID, _MDInIrrReturnFlowID, _MDOutGrdWatID, _MDOutDischargeAbstractionID);
		
		MFVarSetMissingVal (_MDOutGrdWatID,               itemID);
		MFVarSetMissingVal (_MDOutGrdWatChgID,            itemID);
		MFVarSetMissingVal (_MDOutBaseFlowID,             itemID);
		MFVarSetMissingVal (_MDOutGrdWaterAbstractionID,  itemID);
		MFVarSetMissingVal (_MDOutDischargeAbstractionID, itemID);
		return;
	}
	if (  MFVarTestMissingVal(_MDInActIrrAreaID,		itemID)) MFVarSetFloat(_MDInActIrrAreaID,		itemID,0);
	baseFlow    = _MDRecharge = MFVarGetFloat (_MDInRechargeID, itemID);
	grdWaterChg = grdWater    = MFVarGetFloat (_MDOutGrdWatID,  itemID);
	irrArea=MFVarGetFloat(_MDInActIrrAreaID,itemID);
	if (grdWater + _MDRecharge > MFMathEpsilon) {
		
		if ((grdWater = MFRungeKutta ((float) 0.0,1.0, grdWater,_MDGroundWaterFunc)) < 0.0) grdWater = 0.0;
		grdWaterChg = grdWater - grdWaterChg;
		baseFlow    = baseFlow - grdWaterChg;
	}
	else grdWaterChg = baseFlow = 0.0;
	
// Abstraction here..........................		
	if (irrArea > 0){
//	grdWaterChg=grdWater;
	irrDemand            = MFVarGetFloat (_MDInGrossIrrigationDemandID, itemID);
	irrReturnFlow        = MFVarGetFloat (_MDInIrrReturnFlowID,         itemID);

	//	if (itemID==2001)printf("InBaseFlow:gwAbst %f demand %f grdWater %f Return=%f \n", groundWaterAbstraction,irrDemand,grdWater,irrReturnFlow);
	if (irrDemand >= grdWater) {
		groundWaterAbstraction = grdWater;
		dischargeAbstraction   = irrDemand - grdWater;
		grdWater = 0.0;
	}
	else {
		groundWaterAbstraction = irrDemand;
		grdWater = grdWater - irrDemand;
		dischargeAbstraction = 0.0;
	}

	grdWater+=irrReturnFlow;
	grdWaterChg=grdWaterChg + irrReturnFlow - groundWaterAbstraction;



}
else
{
	dischargeAbstraction=0.0;
	groundWaterAbstraction=0.0;
	irrReturnFlow=0.0;
	}

//if (itemID==2001) printf ("BASEFLOW:grdWaterChg %f, gWAbstr %f DisAbst%f irrArea=%f grdWater=%f irrDemand=%f\n",grdWaterChg, groundWaterAbstraction,dischargeAbstraction,irrArea,grdWater,irrDemand);


//if (itemID==4412) printf ("in BaseFlow: irrDemand %f irrReturnFlow %f grdWater %f grdWaterChg %f, gWAbstr %f DisAbst%f \n",irrDemand, irrReturnFlow,grdWater,grdWaterChg, groundWaterAbstraction,dischargeAbstraction);


	if (dischargeAbstraction <0)printf ("DischAbstr negativ in MDBaseFlow! %f\n", dischargeAbstraction);

float in = _MDRecharge+irrReturnFlow; 
float out =baseFlow+groundWaterAbstraction;
//grdWaterChg=in-out;
float watBal=in-out-grdWaterChg;
if (fabs(watBal)>0.1){
printf ("BaseFlowWaterBalance!!!=%f  in = %f out=%f GWChange %f Return %f gwAbstr %f irrArea %f item %i  baseFlow %f \n",watBal,in,out,grdWaterChg, irrReturnFlow,groundWaterAbstraction,irrArea,itemID,baseFlow);
}


	//alles in mm!
	MFVarSetFloat (_MDOutGrdWatID,               itemID, grdWater);
	MFVarSetFloat (_MDOutGrdWatChgID,            itemID, grdWaterChg);
	MFVarSetFloat (_MDOutBaseFlowID,             itemID, baseFlow);
	MFVarSetFloat (_MDOutGrdWaterAbstractionID,  itemID, groundWaterAbstraction);
	MFVarSetFloat (_MDOutDischargeAbstractionID, itemID, dischargeAbstraction); 
}

int MDBaseFlowDef () {
	const char *optStr;
	float par;

	if (_MDOutBaseFlowID != CMfailed) return (_MDOutBaseFlowID);
	MFDefEntering ("Base flow");

	if ((_MDInRechargeID        = MDInfiltrationDef ())   == CMfailed) return (CMfailed);
	 
	if (((optStr = MFOptionGet (MDParGroundWatBETA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDGroundWatBETA = par;
//	if ((_MDInGrossIrrigationDemandID = MDIrrigationWaterDef()) == CMfailed)return CMfailed;
	if ((_MDInIrrReturnFlowID         = MFVarGetID (MDVarIrrPercolationWater,    "mm", MFInput,  MFState,  false)) == CMfailed) return CMfailed;
	if ((_MDOutGrdWatID               = MFVarGetID (MDVarGroundWater,            "mm", MFOutput, MFState, true))  == CMfailed) return CMfailed;
	if ((_MDOutGrdWatChgID            = MFVarGetID (MDVarGroundWaterChange,      "mm", MFOutput, MFFlux,  false)) == CMfailed) return CMfailed;
	if ((_MDOutBaseFlowID             = MFVarGetID (MDVarBaseFlow,               "mm", MFOutput, MFFlux,  true))  == CMfailed) return CMfailed;
	if ((_MDOutGrdWaterAbstractionID  = MFVarGetID (MDVarGroundWaterAbstraction, "mm", MFOutput, MFFlux,  false)) == CMfailed) return CMfailed;
	if ((_MDOutDischargeAbstractionID = MFVarGetID (MDVarDischargeAbstraction,   "mm", MFOutput, MFFlux,  false)) == CMfailed) return CMfailed;
	if ((_MDInGrossIrrigationDemandID = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand,          "mm", MFInput, MFState, false)) == CMfailed) return (CMfailed);
	//if ((_MDInIrrAreaID               = MFVarGetID (MDVarIrrAreaFraction,        "%",  MFInput,  MFState, false)) == CMfailed) return CMfailed;
        if ((_MDInActIrrAreaID		  = MFVarGetID (MDVarActuallyIrrArea,	     "-",  MFInput,  MFState, false))==CMfailed)return (CMfailed);
	_MDOutBaseFlowID = MFVarSetFunction (_MDOutBaseFlowID,_MDBaseFlow);
			
	MFDefLeaving ("Base flow");
	return (_MDOutBaseFlowID);
}
