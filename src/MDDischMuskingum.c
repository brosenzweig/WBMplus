/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDichMuskingum.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInMuskingumC0ID,   _MDInMuskingumC1ID,   _MDInMuskingumC2ID;
static int _MDInDischargeID = CMfailed;
static int _MDInRiverSlopeID;
static int _MDOutRiverVelocityID;

static void _MDDischMuskingumCoeff (int itemID) {
	float slope, discharge;
	float eta = 0.25, nu = 0.4, tau = 8.0, phi = 0.58, beta = 1.44;
	float C, D;
	float C0,C1,C2;
	float yMean, wMean, vMean, dt, dl;

	if (MFVarTestMissingVal (_MDInRiverSlopeID,      itemID) ||
	    MFVarTestMissingVal (_MDInDischargeID,       itemID)) {
		MFVarSetMissingVal (_MDInMuskingumC0ID,     itemID);
		MFVarSetMissingVal (_MDInMuskingumC1ID,     itemID);
		MFVarSetMissingVal (_MDInMuskingumC2ID,     itemID);
	}
	else {
		discharge  = fabs(MFVarGetFloat(_MDInDischargeID,  itemID));
		slope      = MFVarGetFloat(_MDInRiverSlopeID,      itemID) / 1000.0;

		dl    = MFModelGetLength (itemID);
		dt    = 86400.0; 
	
		if ((discharge <= 0.0) || isnan (discharge)) discharge = 100.0; // This deserves comment!

		yMean = eta * pow (discharge, nu);
		wMean = tau * pow (discharge, phi);
		vMean = discharge / (yMean * wMean);

		if (dl > 0.0) {
			C = beta * vMean * dt / dl;
			D = yMean / (dl * slope * beta);
			if (isnan(C)) { printf ("C is nan %f %f %f %f\n",dl, slope, discharge, vMean); C = 1.0; }
			if (isnan(D)) { printf ("D is nan %f %f %f %f\n",dl, slope, discharge, vMean); D = 1.0; }
		 
			
			C0= (-1 + C + D) / (1 + C + D);
			C1=(1 + C - D) / (1 + C + D);
			C2=( 1 - C + D) / (1 + C + D);
if (isnan(C0)) printf ("the framework must die!!!!!!!!! C %f D %f, yMean %f dl %f slope %f beta %f \n", C, D, yMean, dl, slope, beta);
			//Quick and dirty fix for negative C coefficients:
			if ((C0 < 0) || (C1 < 0) || (C2 < 0)) { C0 = 1; C1 = 0; C2 = 0; }
							
			MFVarSetFloat (_MDInMuskingumC0ID, itemID, C0 );
			MFVarSetFloat (_MDInMuskingumC1ID, itemID, C1);
			MFVarSetFloat (_MDInMuskingumC2ID, itemID, C2);
		}
		else {
			MFVarSetFloat (_MDInMuskingumC0ID, itemID, 1.0);
			MFVarSetFloat (_MDInMuskingumC1ID, itemID, 0.0);
			MFVarSetFloat (_MDInMuskingumC2ID, itemID, 0.0);
		}
	}

	MFVarSetFloat (_MDOutRiverVelocityID,itemID,vMean);
	if (isnan(vMean)) printf ("Discharge %f C %f  beta%f vMea%f dt%f dl%f yMean%f wMean%f\n",discharge,C, beta, vMean, dt, dl,yMean, wMean);
}

/* Input */
static int _MDInRunoffID;
static int _MDInDischargeObsID = CMfailed;
static int _MDInDischargeAbstractionID;
static int _MDInReservoirReleaseID;
static int _MDInReservoirCapacityID;

/* Output */
static int _MDOutRiverStorageID, _MDOutDischarge0ID,   _MDOutDischarge1ID;
static int _MDOutDischargeID     = CMfailed;
static int _MDOutDischargeCalcID = CMfailed;
static int _MDOutExcessAbstractionID;
static int _MDOutRiverStorageChangeID;

static void _MDDischMuskingum (int itemID) {
	float c0, c1, c2;
	float runoff, inDischCurrent, inDischPrevious, outDisch;
	float resRelease;
	float excessAbstraction;
	float resCapacity;
	float dischargeAbstraction;
	if (MFVarTestMissingVal (_MDInReservoirCapacityID,      itemID))  MFVarSetFloat(_MDInReservoirCapacityID,      itemID,0);
	if (MFVarTestMissingVal (_MDInMuskingumC0ID,          itemID) ||
		 MFVarTestMissingVal (_MDInMuskingumC1ID,          itemID) ||
		 MFVarTestMissingVal (_MDInMuskingumC2ID,          itemID) ||
		 MFVarTestMissingVal (_MDInDischargeAbstractionID, itemID) ||
		 MFVarTestMissingVal (_MDOutRiverStorageID,        itemID) ||
		 MFVarTestMissingVal (_MDOutDischarge0ID,          itemID) ||
		 MFVarTestMissingVal (_MDOutDischarge1ID,          itemID) ||
		 MFVarTestMissingVal (_MDOutDischargeID,           itemID)) {
		MFVarSetFloat (_MDOutRiverStorageID,  itemID, 0.0);
		MFVarSetFloat (_MDOutDischarge0ID,    itemID, 0.0);
		MFVarSetFloat (_MDOutDischarge1ID,    itemID, 0.0);
		MFVarSetFloat (_MDOutDischargeID,     itemID, 0.0);
		MFVarSetFloat (_MDOutRiverVelocityID, itemID, 0.0);
		MFVarSetFloat (_MDInDischargeAbstractionID, itemID, 0.0);
		 
	}
	else {
		if (MFVarTestMissingVal (_MDInRunoffID, itemID)) runoff = 0.0;
 		else runoff          = MFVarGetFloat (_MDInRunoffID,               itemID) * MFModelGetArea (itemID) * MDConst_mmKm2PerDayTOm3PerSec;
		if (runoff < 0.0)
			printf ("Runoff mm %f runoff km3 %f MFModelGetArea(itemID) %f\n", MFVarGetFloat (_MDInRunoffID,  itemID), runoff, MFModelGetArea (itemID));
		c0                   = MFVarGetFloat (_MDInMuskingumC0ID,          itemID);
		c1                   = MFVarGetFloat (_MDInMuskingumC1ID,          itemID);
		c2                   = MFVarGetFloat (_MDInMuskingumC2ID,          itemID);
		resCapacity          = MFVarGetFloat (_MDInReservoirCapacityID,    itemID);
		inDischPrevious      = MFVarGetFloat (_MDOutDischarge0ID,          itemID);
		dischargeAbstraction = MFVarGetFloat (_MDInDischargeAbstractionID, itemID) * MFModelGetArea (itemID) * MDConst_mmKm2PerDayTOm3PerSec;
		inDischCurrent       = MFVarGetFloat (_MDOutDischargeID,           itemID) + runoff;// - dischargeAbstraction;
//	if (itemID==2001) printf ("RO %f dischargeAbstraction %f \n",runoff,dischargeAbstraction*1/MDConst_mmKm2PerDayTOm3PerSec/MFModelGetArea(itemID));
		if (dischargeAbstraction >  inDischCurrent) {
		if (inDischCurrent < 0) printf ("DischAbstr in Musk < 0!! abstr %f inDischCur %f Runoff %f\n", dischargeAbstraction, inDischCurrent, runoff);
			excessAbstraction    = dischargeAbstraction - inDischCurrent;
			dischargeAbstraction = inDischCurrent;
			
			inDischCurrent = 0; //in m3/s !!!!!!!!!!!!!
		}
		else
		{
		excessAbstraction=0;
		
		}
		if (inDischCurrent < 0)
			printf ("InDisch <0! IOuDich %f inDischCurrent%f dischargeAbstraction%f runoff  %f \n",
			        MFVarGetFloat (_MDOutDischargeID, itemID), inDischCurrent, runoff, dischargeAbstraction);
		
			excessAbstraction    = excessAbstraction    / MFModelGetArea(itemID) * MDConst_m3PerSecTOmmKm2PerDay; // back to mm!
			dischargeAbstraction = dischargeAbstraction / MFModelGetArea(itemID) * MDConst_m3PerSecTOmmKm2PerDay;
			outDisch             = MFVarGetFloat (_MDOutDischarge1ID,          itemID);
			
		if (resCapacity > 0.0) {
	 			resRelease = MFVarGetFloat (_MDInReservoirReleaseID, itemID);
				outDisch=resRelease;
			}
		else { //no reservoir
		outDisch = c0 * inDischCurrent + c1 * inDischPrevious + c2 * outDisch;
		}	
 
		if (isnan(outDisch)) {
			CMmsgPrint (CMmsgDebug,"Outdich  %f C0 %f c1%f c2%f resCapacity%f runoff %f LON %f LAT %f \n",
					outDisch, c0, c1, c2, resCapacity, runoff , MFModelGetLongitude(itemID),MFModelGetLatitude(itemID));
		//	printf ("Outdich  %f C0 %f c1%f c2%f resCapacity%f runoff %f ,inDisc %f, inDisPrev %f LON %f LAT %f \n",
		//			outDisch, c0, c1, c2, resCapacity,runoff,inDischCurrent, inDischPrevious, MFModelGetLongitude(itemID),MFModelGetLatitude(itemID));
			outDisch = 0.0;
		}
		if (outDisch < 0) {
			CMmsgPrint (CMmsgDebug,"Outdich  %f resCapacity %f c0 %f c1 %f c2 %f indischPrevious %f inDischCurrent %f \n",
			            outDisch, resCapacity, c0, c1, c2, inDischPrevious, inDischCurrent);
		}
//if (itemID==2001) printf ("ExAbstractionMUSK=%f\n", excessAbstraction);
		MFVarSetFloat (_MDInDischargeAbstractionID,itemID, dischargeAbstraction); // overwrite the value that was set in MDBaseFlow!
		MFVarSetFloat (_MDOutDischarge0ID,         itemID, inDischCurrent);
		MFVarSetFloat (_MDOutDischarge1ID,         itemID, outDisch);
		MFVarSetFloat (_MDOutExcessAbstractionID,  itemID, excessAbstraction);
		MFVarSetFloat (_MDOutRiverStorageChangeID, itemID, inDischPrevious - outDisch);

		if (_MDInDischargeObsID != CMfailed) {
			MFVarSetFloat (_MDOutDischargeCalcID, itemID, outDisch);
			if (!MFVarTestMissingVal (_MDInDischargeObsID, itemID)) /* Overwrite discharge */ 
				outDisch = MFVarGetFloat (_MDInDischargeObsID, itemID);
		}
		MFVarSetFloat (_MDOutDischargeID,          itemID, outDisch);

	// if ((MFModelGetLongitude (itemID) == debugLon) && (MFModelGetLatitude (itemID) == debugLat)) 
	// printf("INCurrent\tRunoff\t InPrevious ResRelease\t OUT \t %f\t%f\t%f\t%f\t%f\n",inDischCurrent, runoff, inDischPrevious,resRelease,outDisch);
	}
}

enum { MDhelp, MDinput, MDstatic, MDdynamic };
enum { MDcorHelp, MDcorOn, MDcorOff };

int MDDischMuskingumDef () {
	int  optID = MDinput, corOptID;
	const char *optStr, *optName = "Muskingum";
	const char *options [] = { MDHelpStr, MDInputStr, "static", "dynamic", (char *) NULL };
	const char *corOptions [] = { MDHelpStr, MDOnStr, MDOffStr, (char *) NULL };

	if (_MDOutDischargeID != CMfailed) return (_MDOutDischargeID);

	MFDefEntering ("Discharge Muskingum");
	if ((_MDInRunoffID           = MDRunoffDef    ()) == CMfailed) return (CMfailed);
	if ((_MDInReservoirReleaseID = MDReservoirDef ()) == CMfailed) return (CMfailed);
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			if (((_MDInMuskingumC0ID = MFVarGetID (MDVarMuskingumC0, MFNoUnit,   MFInput,  MFState,  false)) == CMfailed) ||
			    ((_MDInMuskingumC1ID = MFVarGetID (MDVarMuskingumC1, MFNoUnit,   MFInput,  MFState,  false)) == CMfailed) ||
			    ((_MDInMuskingumC2ID = MFVarGetID (MDVarMuskingumC2, MFNoUnit,   MFInput,  MFState,  false)) == CMfailed))
				return (CMfailed);
			break;
		case MDstatic:
			if ((_MDInDischargeID = MDRefDischargeDef ()) == CMfailed) return (CMfailed);
		case MDdynamic:
			if (((_MDInRiverSlopeID      = MFVarGetID (MDVarRiverSlope,        "m/km",     MFInput,  MFState,  false)) == CMfailed) ||
			    ((_MDInMuskingumC0ID     = MFVarGetID (MDVarMuskingumC0,       MFNoUnit,   MFOutput, MFState,  false)) == CMfailed) ||
			    ((_MDInMuskingumC1ID     = MFVarGetID (MDVarMuskingumC1,       MFNoUnit,   MFOutput, MFState,  false)) == CMfailed) ||
			    ((_MDInMuskingumC2ID     = MFVarGetID (MDVarMuskingumC2,       MFNoUnit,   MFOutput, MFState,  false)) == CMfailed) ||
			    ((_MDOutRiverVelocityID  = MFVarGetID (MDVarRiverMeanVelocity, "m/s",      MFOutput, MFState,  false)) == CMfailed))
				return (CMfailed);
				_MDInMuskingumC0ID = MFVarSetFunction(_MDInMuskingumC0ID,_MDDischMuskingumCoeff);
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	if ((optStr = MFOptionGet ("DischargeCorrection")) != (char *) NULL) {
		switch ((corOptID = CMoptLookup (corOptions, optStr, true))) {
			case MDcorOn:
				if (((_MDInDischargeObsID   = MFVarGetID (MDVarRiverDischargeObserved,   "m3/s",  MFInput,  MFState, false)) == CMfailed) ||
				    ((_MDOutDischargeCalcID = MFVarGetID (MDVarRiverDischargeCalculated, "m3/s",  MFOutput, MFState, false)) == CMfailed)) return CMfailed;
				break;
			case MDcorOff: break; 
			default:
				CMmsgPrint (CMmsgInfo,"Help [%s options]:","DischargeCorrection");
				for (optID = 1;corOptions [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
				CMmsgPrint (CMmsgInfo,"\n");
				return (CMfailed);
		}
	}
	if (((_MDInDischargeAbstractionID  = MFVarGetID (MDVarDischargeAbstraction, "mm",     MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInReservoirCapacityID     = MFVarGetID (MDVarReservoirCapacity,    "km3",    MFInput,  MFState, false)) == CMfailed) ||
	    ((_MDOutRiverStorageID         = MFVarGetID (MDVarRiverStorage,         "m3",     MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDOutRiverStorageChangeID   = MFVarGetID (MDVarRiverStorageChange,   "m3/s",   MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDOutDischarge0ID           = MFVarGetID (MDVarRiverDischarge0,      "m3/s",   MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDOutDischarge1ID           = MFVarGetID (MDVarRiverDischarge1,      "m3/s",   MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDOutExcessAbstractionID    = MFVarGetID (MDVarExcessAbstraction,    "mm",     MFOutput, MFFlux,  false)) == CMfailed) ||
	    ((_MDOutDischargeID            = MFVarGetID (MDVarRiverDischarge,       "m3/s",   MFRoute,  MFState, true))  == CMfailed)) return CMfailed;

	if (_MDInDischargeID == CMfailed) _MDInDischargeID = _MDOutDischargeID;
	_MDOutDischargeID = MFVarSetFunction(_MDOutDischargeID,_MDDischMuskingum);

	MFDefLeaving ("Discharge Muskingum");
	return (_MDOutDischargeID);
}
