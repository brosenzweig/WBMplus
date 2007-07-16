/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDCParams.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInCoverID = CMfailed;

static int _MDInSnowPackID, _MDOutCParamAlbedoID = CMfailed; 

static void _MDCParamAlbedo (int itemID) {
/* Input */
	int cover;
	float snowPack;
/* Local */
	static float albedo []     = { 0.14, 0.18, 0.18, 0.20, 0.20, 0.22, 0.26, 0.10 };
	static float albedoSnow [] = { 0.14, 0.23, 0.35, 0.50, 0.50, 0.50, 0.50, 0.50 };

	if (MFVarTestMissingVal (_MDInCoverID,    itemID) ||
		 MFVarTestMissingVal (_MDInSnowPackID, itemID)) { MFVarSetMissingVal (_MDOutCParamAlbedoID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (albedo) / sizeof (albedo [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	snowPack = MFVarGetFloat (_MDInSnowPackID,itemID);
	MFVarSetFloat (_MDOutCParamAlbedoID,itemID,snowPack > 0.0 ? albedoSnow[cover] : albedo[cover]);	
}

enum { MDhelp, MDinput, MDlookup };

int MDCParamAlbedoDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamAlbedo;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamAlbedoID != CMfailed) return (_MDOutCParamAlbedoID);

	MFDefEntering ("Albedo");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamAlbedoID = MFVarGetID (MDVarCParamAlbedo,  MFNoUnit, MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID    = MDLandCoverDef ()) == CMfailed) ||
			 	 ((_MDInSnowPackID = MFVarGetID (MDVarSnowPack, "mm", MFInput,  MFState, true)) == CMfailed) ||
				 ((_MDOutCParamAlbedoID = MFVarGetID (MDVarCParamAlbedo, MFNoUnit, MFOutput, MFState, false)) == CMfailed))
				 return (CMfailed);
			_MDOutCParamAlbedoID = MFVarSetFunction (_MDOutCParamAlbedoID,_MDCParamAlbedo); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Albedo");
	return (_MDOutCParamAlbedoID); 
}

static int _MDOutCParamCHeightID = CMfailed; 

static void _MDCParamCHeight (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup [] = { 25.0, 25.0, 8.0, 0.5, 0.3, 0.3, 0.1, 0.01}; 

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamCHeightID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning ,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCHeightID,itemID, lookup [cover]);	
}

int MDCParamCHeightDef ()
	{
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamCHeight;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamCHeightID != CMfailed) return (_MDOutCParamCHeightID);

	MFDefEntering ("Canopy Height");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCHeightID = MFVarGetID (MDVarCParamCHeight, "m", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
			    ((_MDOutCParamCHeightID = MFVarGetID (MDVarCParamCHeight, "m", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamCHeightID = MFVarSetFunction (_MDOutCParamCHeightID,_MDCParamCHeight); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Canopy Height");
	return (_MDOutCParamCHeightID); 
}

static int _MDOutCParamLWidthID = CMfailed; 

static void _MDCParamLWidth (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup [] = { 0.004,0.1,  0.03, 0.01, 0.01, 0.1,  0.02, 0.001};

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamLWidthID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamLWidthID,itemID, lookup [cover]);	
}

int MDCParamLWidthDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamLWidth;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamLWidthID != CMfailed) return (_MDOutCParamLWidthID);

	MFDefEntering ("Leaf Width");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamLWidthID = MFVarGetID (MDVarCParamLWidth,  "mm", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID         = MDLandCoverDef ()) == CMfailed) ||
			    ((_MDOutCParamLWidthID = MFVarGetID (MDVarCParamLWidth, "mm", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamLWidthID = MFVarSetFunction (_MDOutCParamLWidthID,_MDCParamLWidth); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) fprintf (stderr," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Leaf Width");
	return (_MDOutCParamLWidthID); 
}

static int _MDOutCParamRSSID = CMfailed; 

static void _MDCParamRSS (int itemID) {
/* Input */
/* Local */

	MFVarSetFloat (_MDOutCParamRSSID,itemID, 500.0);	
}

int MDCParamRSSDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamRSS;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamRSSID != CMfailed) return (_MDOutCParamRSSID);

	MFDefEntering ("RSS");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamRSSID = MFVarGetID (MDVarCParamRSS,  "s/m", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
				 ((_MDOutCParamRSSID = MFVarGetID (MDVarCParamRSS, "s/m", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamRSSID =  MFVarSetFunction (_MDOutCParamRSSID,_MDCParamRSS); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("RSS");
	return (_MDOutCParamRSSID); 
}

static int _MDOutCParamR5ID = CMfailed; 

static void _MDCParamR5 (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup []     = { 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 10.0 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamR5ID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamR5ID,itemID, lookup [cover]);	
}

int MDCParamR5Def () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamR5;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamR5ID != CMfailed) return (_MDOutCParamR5ID);

	MFDefEntering ("R5");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamR5ID = MFVarGetID (MDVarCParamR5,  "W/m2", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
				 ((_MDOutCParamR5ID = MFVarGetID (MDVarCParamR5, "W/m2", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamR5ID = MFVarSetFunction (_MDOutCParamR5ID,_MDCParamR5); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("R5");
	return (_MDOutCParamR5ID); 
}

static int _MDOutCParamCDID = CMfailed; 

static void _MDCParamCD (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup []     = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0.10 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamCDID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
//	if (itemID==2)printf ("Hier Land cover no  %i \n", cover );
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCDID,itemID, lookup [cover]);	
}

int MDCParamCDDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamCD;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamCDID != CMfailed) return (_MDOutCParamCDID);

	MFDefEntering ("CD");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCDID = MFVarGetID (MDVarCParamCD,  "kPa", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamCDID = MFVarGetID (MDVarCParamCD, "kPa", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamCDID = MFVarSetFunction (_MDOutCParamCDID,_MDCParamCD); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("CD");
	return (_MDOutCParamCDID); 
}

static int _MDOutCParamCRID = CMfailed; 

static void _MDCParamCR (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup [] = { 0.5, 0.6, 0.6, 0.7, 0.7, 0.7, 0.7, 0.01 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamCRID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCRID,itemID, lookup [cover]);	
}

int MDCParamCRDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamCR;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamCRID != CMfailed) return (_MDOutCParamCRID);

	MFDefEntering ("CR");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCRID = MFVarGetID (MDVarCParamCR,  MFNoUnit, MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamCRID = MFVarGetID (MDVarCParamCR, MFNoUnit, MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamCRID = MFVarSetFunction (_MDOutCParamCRID,_MDCParamCR); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("CR");
	return (_MDOutCParamCRID); 
}

static int _MDOutCParamGLMaxID = CMfailed; 

static void _MDCParamGLMax (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup []     = { 0.0053, 0.0053, 0.0053, 0.008, 0.0066, 0.011, 0.005, 0.001 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamGLMaxID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamGLMaxID,itemID, lookup [cover]);	
}

int MDCParamGLMaxDef ()
	{
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamGLMax;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamGLMaxID != CMfailed) return (_MDOutCParamGLMaxID);

	MFDefEntering ("GLMax");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamGLMaxID = MFVarGetID (MDVarCParamGLMax,  "m/s", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	   		 ((_MDOutCParamGLMaxID = MFVarGetID (MDVarCParamGLMax, "m/s", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamGLMaxID = MFVarSetFunction (_MDOutCParamGLMaxID,_MDCParamGLMax); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("GLMax");
	return (_MDOutCParamGLMaxID); 
}

static int _MDOutCParamLPMaxID = CMfailed; 

static void _MDCParamLPMax (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup []     = { 6, 6, 3, 3, 4, 3, 1, 0.00001 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamLPMaxID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamLPMaxID,itemID, lookup [cover]);	
}

int MDCParamLPMaxDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamLPMax;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamLPMaxID != CMfailed) return (_MDOutCParamLPMaxID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("LPMax");
	switch (optID) {
		case MDinput:  _MDOutCParamLPMaxID = MFVarGetID (MDVarCParamLPMax,  MFNoUnit, MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamLPMaxID = MFVarGetID (MDVarCParamLPMax, MFNoUnit, MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamLPMaxID = MFVarSetFunction (_MDOutCParamLPMaxID,_MDCParamLPMax); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("LPMax");
	return (_MDOutCParamLPMaxID); 
}

static int _MDOutCParamZ0gID = CMfailed; 

static void _MDCParamZ0g (int itemID) {
/* Input */
	int cover;
/* Local */
	static float lookup []     = { 0.02, 0.02, 0.02, 0.01, 0.01, 0.005, 0.001, 0.001 };

	if (MFVarTestMissingVal (_MDInCoverID, itemID)) { MFVarSetMissingVal (_MDOutCParamZ0gID,itemID); return; }

	cover = MFVarGetInt (_MDInCoverID,itemID);
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamZ0gID,itemID, lookup [cover]);	
	}

int MDCParamZ0gDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarCParamZ0g;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", (char *) NULL };

	if (_MDOutCParamZ0gID != CMfailed) return (_MDOutCParamZ0gID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("Z0g");
	switch (optID) {
		case MDinput: _MDOutCParamZ0gID = MFVarGetID (MDVarCParamZ0g,  "m", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamZ0gID = MFVarGetID (MDVarCParamZ0g, "m", MFOutput, MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCParamZ0gID = MFVarSetFunction (_MDOutCParamZ0gID,_MDCParamZ0g); 
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Z0g");
	return (_MDOutCParamZ0gID); 
}
