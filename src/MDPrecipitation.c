/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPrecipitation.c

balazs.fekete@unh.edu
******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

bool MDEvent (int nSteps,int nEvents,int step)

	{
  	bool inv = false;
	int event;
	float freq;

	if (nSteps == nEvents) return (true);

	if (nEvents > nSteps / 2) { nEvents = nSteps - nEvents; inv = true; }
		
  	freq = (float) nSteps / (float) nEvents;
	for (event = 0;event < step;++event)
		if ((int) (rint (event * freq + freq / 2.0)) == step) return (inv ? false : true);

	return (inv ? true : false);
	}

static int _MDInPrecipID, _MDInWetDaysID, _MDInPrecipFracID;
static int _MDOutPrecipID = CMfailed;

static void _MDPrecipWetDays (int itemID)
	{
// Input 
	float precipIn;
	int  wetDays;
// Output 
	float precipOut; 
// Local 
	int day, nDays; 

	if (MFVarTestMissingVal (_MDInPrecipID,  itemID) ||
		 MFVarTestMissingVal (_MDInWetDaysID, itemID)) { MFVarSetMissingVal (_MDOutPrecipID,itemID); return; }

	day      = MFDateGetCurrentDay ();
	nDays    = MFDateGetMonthLength ();
	precipIn = MFVarGetFloat (_MDInPrecipID,  itemID);

	wetDays = MFVarGetInt   (_MDInWetDaysID, itemID);

	precipOut = MDEvent (nDays,wetDays,day) ? precipIn * (float) nDays / (float) wetDays : 0.0;

	MFVarSetFloat (_MDOutPrecipID,itemID,precipOut);
	}

static void _MDPrecipFraction (int itemID)
	{
// Input 
	float precipIn;
	float precipFrac;
// Output 
	float precipOut; 
// Local 
	int nDays    = MFDateGetMonthLength ();

	if (MFVarTestMissingVal (_MDInPrecipID,     itemID) ||
		 MFVarTestMissingVal (_MDInPrecipFracID, itemID)) { MFVarSetMissingVal (_MDOutPrecipID,itemID); return; }

	precipIn   = MFVarGetFloat (_MDInPrecipID,     itemID);
	precipFrac = MFVarGetFloat (_MDInPrecipFracID, itemID);

	precipOut = precipIn *  precipFrac* nDays;
	if (precipOut <0){printf ("Precip negative! precipIn=%f precipFrac =%fprecipFrac\n", precipIn, precipFrac);}
 //if (itemID==2)printf("PrecipOut %f nDays %i precipFrac %f precipIn %f \n",precipOut, nDays,precipFrac,precipIn);
	MFVarSetFloat (_MDOutPrecipID,itemID,precipOut);
	}

enum { MDhelp, MDinput, MDwetdays, MDfraction };

int MDPrecipitationDef ()

	{
	int optID = MDinput;
	const char *optStr, *optName = MDVarPrecipitation;
	const char *options [] = { MDHelpStr, MDInputStr, "wetdays", "fraction",(char *) NULL };

	if (_MDOutPrecipID != CMfailed) return (_MDOutPrecipID);

	MFDefEntering ("Precipitation");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID)
		{
		case MDinput: _MDOutPrecipID = MFVarGetID (MDVarPrecipitation, "mm", MFInput,  MFFlux,  false); break;
		case MDwetdays:
			if (((_MDInWetDaysID    = MDWetDaysDef ()) == CMfailed) ||
				 ((_MDInPrecipID     = MFVarGetID (MDVarPrecipMonthly,  "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
				 ((_MDOutPrecipID    = MFVarGetID (MDVarPrecipitation,  "mm", MFOutput, MFFlux,  false)) == CMfailed))
				return (CMfailed);
			_MDOutPrecipID = MFVarSetFunction (_MDOutPrecipID,_MDPrecipWetDays);
			break;
		case MDfraction:
			if (((_MDInPrecipID     = MFVarGetID (MDVarPrecipMonthly,  "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
			    ((_MDInPrecipFracID = MFVarGetID (MDVarPrecipFraction, "mm", MFInput,  MFState, false)) == CMfailed) ||
				 ((_MDOutPrecipID    = MFVarGetID (MDVarPrecipitation,  "mm", MFOutput, MFFlux,  false)) == CMfailed))
				return (CMfailed);
			_MDOutPrecipID = MFVarSetFunction (_MDOutPrecipID,_MDPrecipFraction);
			break;
		default:
			fprintf (stderr,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) fprintf (stderr," %s",options [optID]);
			fprintf (stderr,"\n");
			return (CMfailed);
		}
	MFDefLeaving ("Precipitation");
	return (_MDOutPrecipID);
	}
