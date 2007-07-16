/* 

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDWetDays.c

balazs.fekete@unh.edu

*/
 

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInPrecipID, _MDInAlphaID, _MDInBetaID;
static int _MDOutWetDaysID = CMfailed;

static void _MDWetDays (int itemID)
	{
// Input /
	float precip;
	float alpha;
	float beta;
// Output 
	int wetDays;
// Local 
	int nDays;

	if (MFVarTestMissingVal (_MDInPrecipID, itemID) ||
		 MFVarTestMissingVal (_MDInAlphaID,  itemID) ||
		 MFVarTestMissingVal (_MDInBetaID,   itemID)) { MFVarSetMissingVal (_MDOutWetDaysID,itemID); return; }

	precip = MFVarGetFloat (_MDInPrecipID, itemID);
	alpha  = MFVarGetFloat (_MDInAlphaID,  itemID);
	beta   = MFVarGetFloat (_MDInBetaID,   itemID);

	nDays   = MFDateGetMonthLength ();
	wetDays = (int) ((float) nDays * alpha * (1.0 - exp ((double) (beta * precip))));
	if (wetDays > nDays) wetDays = nDays;
	if (wetDays < 1)     wetDays = 1;

	MFVarSetInt (_MDOutWetDaysID,itemID,wetDays);
	}

enum { MDhelp, MDinput, MDlbg };

int MDWetDaysDef ()
	{
	int optID = MDinput;
	const char *optStr, *optName = MDVarWetDays;
	const char *options [] = { MDHelpStr, MDInputStr, "LBG", (char *) NULL };

	if (_MDOutWetDaysID != CMfailed) return (_MDOutWetDaysID);

	MFDefEntering ("Wet Days");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID)
		{
		case MDinput:  _MDOutWetDaysID = MFVarGetID (MDVarWetDays,           " ", MFInput,   MFState, false); break;
		case MDlbg:
			if (((_MDInPrecipID   = MFVarGetID (MDVarPrecipitation, "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
				 ((_MDInAlphaID    = MFVarGetID (MDVarWetDaysAlpha,  " ",  MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDInBetaID     = MFVarGetID (MDVarWetDaysBeta,   " ",  MFInput,  MFState, false)) == CMfailed) ||
	   		 ((_MDOutWetDaysID = MFVarGetID (MDVarWetDays,       "  ", MFOutput, MFFlux,  false)) == CMfailed))
				return (CMfailed);
			_MDOutWetDaysID = MFVarSetFunction (_MDOutWetDaysID,_MDWetDays); 
			break;
		default:
			fprintf (stderr,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) fprintf (stderr," %s",options [optID]);
			fprintf (stderr,"\n");
			return (CMfailed);
		}
	MFDefLeaving ("Wet Days");
	return (_MDOutWetDaysID);
	}
