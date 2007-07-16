/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDLSAreaIndex.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInCoverID, _MDInAtMeanID, _MDInCParamLPMaxID;
static int _MDOutLeafAreaIndexID = CMfailed;

static void _MDLeafAreaIndex (int itemID) {
/* projected leaf area index (lai) pulled out from cover dependent PET functions  */
/* Input */
	int   cover;  
	float lpMax;  /* maximum projected leaf area index */
	float airT;   /* air temperature [degree C] */
/* Local */
	float lai;

	if (MFVarTestMissingVal (_MDInCoverID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamLPMaxID, itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,      itemID)) { MFVarSetMissingVal (_MDOutLeafAreaIndexID,itemID); return; }

	cover = MFVarGetInt   (_MDInCoverID,       itemID);
	airT  = MFVarGetFloat (_MDInAtMeanID,      itemID);
	lpMax = MFVarGetFloat (_MDInCParamLPMaxID, itemID);

	if (cover == 0) lai = lpMax;
	else if (airT > 8.0) lai = lpMax;
	else lai = 0.0;

   MFVarSetFloat (_MDOutLeafAreaIndexID,itemID,0.001 > lai ? 0.001 : lai);
}

enum { MDhelp, MDinput, MDstandard };

int MDLeafAreaIndexDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarLeafAreaIndex;
	const char *options [] = { MDHelpStr, MDInputStr, "standard", (char *) NULL };

	if (_MDOutLeafAreaIndexID != CMfailed) return (_MDOutLeafAreaIndexID);

	MFDefEntering ("Leaf Area");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex, MFNoUnit, MFInput, MFState, false); break;
		case MDstandard:
			if (((_MDInCParamLPMaxID    = MDCParamLPMaxDef ()) == CMfailed) ||
				 ((_MDInCoverID          = MFVarGetID (MDVarWBMCover,       MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDInAtMeanID         = MFVarGetID (MDVarAirTemperature, "degC",   MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDOutLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex,  MFNoUnit, MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutLeafAreaIndexID = MFVarSetFunction (_MDOutLeafAreaIndexID,_MDLeafAreaIndex); 
			break;
	}
	MFDefLeaving ("Leaf Area");
	return (_MDOutLeafAreaIndexID);
}

static int _MDInCParamCHeightID;
static int _MDOutStemAreaIndexID = CMfailed;

static void _MDStemAreaIndex (int itemID) {
/* Projected Stem area index (sai) pulled out from McNaugthon and Black PET function  */
/* Input */
 	float lpMax;   /* maximum projected leaf area index */
	float cHeight; /* canopy height [m] */
/* Local */
	float sai;

	if (MFVarTestMissingVal (_MDInCParamLPMaxID,   itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID, itemID)) { MFVarSetMissingVal (_MDOutStemAreaIndexID,itemID); return; }

	lpMax   = MFVarGetFloat (_MDInCParamLPMaxID,   itemID);
	cHeight = MFVarGetFloat (_MDInCParamCHeightID, itemID);

	sai = lpMax > MDConstLPC ? MDConstCS * cHeight : (lpMax / MDConstLPC) * MDConstCS * cHeight;
	MFVarSetFloat (_MDOutStemAreaIndexID,itemID,sai);
}

int MDStemAreaIndexDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarStemAreaIndex;
	const char *options [] = { MDHelpStr, MDInputStr, "standard", (char *) NULL };

	if (_MDOutStemAreaIndexID != CMfailed) return (_MDOutStemAreaIndexID);

	MFDefEntering ("Stem Area Index");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex, MFNoUnit, MFInput, MFState, false); break;
		case MDstandard:
			if (((_MDInCParamLPMaxID    = MFVarGetID (MDVarCParamLPMax,    MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDInCParamCHeightID  = MFVarGetID (MDVarCParamCHeight,  "m",      MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDOutStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex,  MFNoUnit, MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutStemAreaIndexID = MFVarSetFunction (_MDOutStemAreaIndexID,_MDStemAreaIndex); 
			break;
	}
	MFDefLeaving ("Stem Area Index");
	return (_MDOutStemAreaIndexID);
}
