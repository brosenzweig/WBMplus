/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSPackChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAtMeanID    = MFUnset;
static int _MDInPrecipID    = MFUnset;
// Output
static int _MDOutSnowPackID = MFUnset;
static int _MDOutSPackChgID = MFUnset;
static int _MDOutSnowMeltID = MFUnset;
static int _MDOutSnowFallID = MFUnset;
static float _MDSnowMeltThreshold = 1.0;
static float _MDFallThreshold = -1.0;

static void _MDSPackChg (int itemID) {
// Input
	float airT;
	float precip;
// Local
	float sPack;
	float sPackChg = 0.0;

	sPack  = MFVarGetFloat (_MDOutSnowPackID, itemID, 0.0);
	if (MFVarTestMissingVal (_MDInAtMeanID,itemID) || MFVarTestMissingVal (_MDInPrecipID, itemID)) {
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack);	
		MFVarSetFloat (_MDOutSPackChgID, itemID, 0.0);
		return; 
	}

//printf ("SnowMeltThreshold= %f SnoFall %f\n",_MDSnowMeltThreshold, _MDFallThreshold);
	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID, 0.0);
	precip = MFVarGetFloat (_MDInPrecipID,    itemID, 0.0);

	if (airT < _MDFallThreshold) {  /* Accumulating snow pack */
		MFVarSetFloat (_MDOutSnowFallID, itemID, precip);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack + precip);
		MFVarSetFloat (_MDOutSPackChgID, itemID, precip);
	}
	else if (airT > _MDSnowMeltThreshold) { /* Melting snow pack */
		sPackChg = 2.63 + 2.55 * airT + 0.0912 * airT * precip;
		sPackChg = - (sPack < sPackChg ? sPack : sPackChg);
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, fabs(sPackChg));
		MFVarSetFloat (_MDOutSPackChgID, itemID, sPackChg);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack + sPackChg);
	}
	else { /* No change when air temperature is in [-1.0,1.0] range */
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack);	
		MFVarSetFloat (_MDOutSPackChgID, itemID, 0.0);
	}
}

int MDSPackChgDef () {

	if (_MDOutSPackChgID != MFUnset) return (_MDOutSPackChgID);
	MFDefEntering ("Snow Pack Change");
	const char *optStr;
	float par;
	if (((optStr = MFOptionGet (MDParSnowMeltThreshold))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDSnowMeltThreshold = par;
	
	if (((optStr = MFOptionGet (MDParFallThreshold))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDFallThreshold= par;

	
	
	if (((_MDInPrecipID    = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature, "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowFallID = MFVarGetID (MDVarSnowFall,       "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowMeltID = MFVarGetID (MDVarSnowMelt,       "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowPackID = MFVarGetID (MDVarSnowPack,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSPackChgID = MFVarGetID (MDVarSnowPackChange, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||

	    	
	    (MFModelAddFunction (_MDSPackChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Snow Pack Change");
	return (_MDOutSPackChgID);
}
