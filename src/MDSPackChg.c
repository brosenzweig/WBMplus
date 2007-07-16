/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDSPackChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAtMeanID,		_MDInPrecipID;
static int _MDOutSnowPackID,	_MDOutSPackChgID = CMfailed;
static int _MDOutSnowMeltID;
static void _MDSPackChg (int itemID) {
/* Input */
	float airT;
	float precip;
/* Local */
	float sPack;
	float sPackChg = 0.0;
	if (MFVarTestMissingVal (_MDInAtMeanID,itemID) ||
		 MFVarTestMissingVal (_MDInPrecipID, itemID)) { MFVarSetFloat (_MDOutSPackChgID,itemID,sPackChg); return; }

	airT   = MFVarGetFloat (_MDInAtMeanID,itemID);
	precip = MFVarGetFloat (_MDInPrecipID,itemID);
	sPack  = MFVarGetFloat (_MDOutSnowPackID,itemID);

	if (airT < -1.0) {  /* Accumulating snow pack */
		MFVarSetFloat (_MDOutSPackChgID,itemID,precip);
		MFVarSetFloat (_MDOutSnowPackID,itemID,sPack + precip);
	}
	else if (airT > 1.0) { /* Melting snow pack */
		sPackChg = 2.63 + 2.55 * airT + 0.0912 * airT * precip;
		sPackChg = - (sPack < sPackChg ? sPack : sPackChg);
		MFVarSetFloat (_MDOutSPackChgID,itemID,sPackChg);
		MFVarSetFloat (_MDOutSnowPackID,itemID,sPack + sPackChg);
		MFVarSetFloat(_MDOutSnowMeltID,itemID,sPackChg);
	}
	else { /* No change when air temperature is in [-1.0,1.0] range */
		MFVarSetFloat (_MDOutSPackChgID,itemID,0.0);
		MFVarSetFloat (_MDOutSnowPackID,itemID,sPack);
		
	}
}


int MDSPackChgDef () {

	if (_MDOutSPackChgID != CMfailed) return (_MDOutSPackChgID);
	MFDefEntering ("Snow Pack Change");

	if (((_MDInPrecipID    = MDPrecipitationDef ()) == CMfailed) ||
		 ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature, "degC", MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDOutSnowPackID = MFVarGetID (MDVarSnowPack,       "mm",   MFOutput, MFState, true))  == CMfailed) ||
		 ((_MDOutSnowMeltID = MFVarGetID(MDVarSnowMelt, 	"mm",  MFOutput, MFState, true))  == CMfailed)||
		 ((_MDOutSPackChgID = MFVarGetID (MDVarSnowPackChange, "mm",   MFOutput, MFFlux,  false)) == CMfailed))
		return (CMfailed);
	MFDefLeaving ("Snow Pack Change");
	return (MFVarSetFunction (_MDOutSPackChgID,_MDSPackChg));
}
