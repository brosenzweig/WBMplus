/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDPotET.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDPotETID = MFUnset;

enum { MDinput, MDHamon, MDJensen, MDPsTaylor, MDPstd, MDPMday, MDPMdn, MDSWGday, MDSWGdn, MDTurc };

int MDPotETDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarPotEvapotrans;
	const char *options [] = { MDInputStr, "Hamon", "Jensen", "PsTaylor", "Pstd", "PMday", "PMdn", "SWGday", "SWGdn", "Turc", (char *) NULL };

	if (_MDPotETID != MFUnset) return (_MDPotETID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	MFDefEntering ("PotET");
	if (MDSPackChgDef () == CMfailed) return (CMfailed);

	switch (optID) {
		case MDinput:    _MDPotETID = MFVarGetID (MDVarPotEvapotrans, "mm", MFInput, MFFlux, false); break;
		case MDHamon:    _MDPotETID = MDPotETHamonDef    (); break;
		case MDJensen:   _MDPotETID = MDPotETJensenDef   (); break;
		case MDPsTaylor: _MDPotETID = MDPotETPsTaylorDef (); break;
		case MDPstd:     _MDPotETID = MDPotETPstdDef     (); break;
		case MDPMday:    _MDPotETID = MDPotETPMdayDef    (); break;
		case MDPMdn:     _MDPotETID = MDPotETPMdnDef     (); break;
		case MDSWGday:   _MDPotETID = MDPotETSWGdayDef   (); break;
		case MDSWGdn:    _MDPotETID = MDPotETSWGdnDef    (); break;
		case MDTurc:     _MDPotETID = MDPotETTurcDef     (); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("PotET");
	return (_MDPotETID);
}
