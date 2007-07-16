/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDichAccumulate.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInRunoffID;
// static int _MDOutDischargeAbstractionID;
static int _MDOutDischargeID = CMfailed;

static void _MDDischAccumulate (int itemID) {
	float discharge;
	
	discharge = MFVarTestMissingVal (_MDInRunoffID, itemID) ? 0.0 : MFVarGetFloat(_MDInRunoffID, itemID);
	//discharge += MFVarGetFloat(_MDOutDischargeAbstractionID,itemID);
	discharge = discharge/1000 * MFModelGetArea (itemID) / (3600*24);
	MFVarSetFloat(_MDOutDischargeID, itemID, MFVarGetFloat (_MDOutDischargeID, itemID) + discharge);
}

int MDDischAccumulateDef () {

	if (_MDOutDischargeID != CMfailed) return (_MDOutDischargeID);

	MFDefEntering ("Discharge Accumulate");
	if ((_MDInRunoffID     = MDRunoffDef ()) == CMfailed) return (CMfailed);
		
//	if	((_MDOutDischargeAbstractionID = MFVarGetID (MDVarDischargeAbstraction, "mm",   MFOutput, MFFlux,  false)) == CMfailed) return CMfailed;
	if ((_MDOutDischargeID            = MFVarGetID (MDVarRiverDischarge,       "m3/s", MFRoute,  MFState, true))  == CMfailed) return CMfailed;
	_MDOutDischargeID = MFVarSetFunction(_MDOutDischargeID,_MDDischAccumulate);

	MFDefLeaving ("Discharge Accumulate");
	return (_MDOutDischargeID);
}
