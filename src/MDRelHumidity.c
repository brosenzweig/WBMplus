/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDRelHumidity.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAtMeanID, _MDInVaporPressID;

static int _MDOutRelHumidityID = CMfailed;

static void _MDRelHumidity (int itemID) {
/* Input */
	float airT;      /* air temperature [degree C] */
	float vPress;    /* vapor pressure [kPa] */
/* Output */
	float relHumid;  /* relative humidity in fraction (0.0 - 1.0) */
/* Local */
	float sVPress;   /* saturated vapor pressure [kPa] */

	if (MFVarTestMissingVal (_MDInAtMeanID,     itemID) ||
		 MFVarTestMissingVal (_MDInVaporPressID, itemID)) { MFVarSetMissingVal (_MDOutRelHumidityID,itemID); return; }

	airT   = MFVarGetFloat (_MDInAtMeanID,     itemID);
	vPress = MFVarGetFloat (_MDInVaporPressID, itemID);

 	sVPress = MDPETlibVPressSat (airT);

 	relHumid = vPress < sVPress ? vPress / sVPress : 0.99;

	MFVarSetFloat (_MDOutRelHumidityID, itemID, relHumid);
}

enum { MDhelp, MDinput, MDcalc };

int MDRelHumidityDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarRelHumidity;
	const char *options [] = { MDHelpStr, MDInputStr, MDCalculateStr, (char *) NULL };
	
	if (_MDOutRelHumidityID != CMfailed) return (_MDOutRelHumidityID);

	MFDefEntering ("Relative Humidity");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutRelHumidityID = MFVarGetID (MDVarRelHumidity,  "%", MFInput, MFState, false); break;
		case MDcalc:
			if (((_MDInAtMeanID       = MFVarGetID (MDVarAirTemperature, "degC",  MFInput, MFState, false)) == CMfailed) ||
	    		 ((_MDInVaporPressID   = MFVarGetID (MDVarVaporPressure,  "kPa",   MFInput, MFState, false)) == CMfailed) ||
		  		 ((_MDOutRelHumidityID = MFVarGetID (MDVarRelHumidity,    "mm",    MFOutput,MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutRelHumidityID = MFVarSetFunction (_MDOutRelHumidityID,_MDRelHumidity);
			break;
		default:
			fprintf (stderr,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) fprintf (stderr," %s",options [optID]);
			fprintf (stderr,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Relative Humidity");
	return (_MDOutRelHumidityID);
}
