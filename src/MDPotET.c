/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPotET.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

enum { MDhelp, MDinput, MDHamon, MDJensen, MDPsTaylor, MDPstd, MDPMday, MDPMdn, MDSWGday, MDSWGdn, MDTurc };

int MDPotETDef () {
	int optID = MDinput, ret = CMfailed;
	const char *optStr, *optName = MDVarPotEvapotrans;
	const char *options [] = { MDHelpStr, MDInputStr, "Hamon", "Jensen", "PsTaylor", "Pstd", "PMday", "PMdn", "SWGday", "SWGdn", "Turc", (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	MFDefEntering ("PotET");
	if (MDSPackChgDef () == CMfailed) return (CMfailed);

	switch (optID) {
		case MDinput:    ret = MFVarGetID (MDVarPotEvapotrans, "mm", MFInput, MFFlux, false); break;
		case MDHamon:    ret = MDPotETHamonDef    (); break;
		case MDJensen:   ret = MDPotETJensenDef   (); break;
		case MDPsTaylor: ret = MDPotETPsTaylorDef (); break;
		case MDPstd:     ret = MDPotETPstdDef     (); break;
		case MDPMday:    ret = MDPotETPMdayDef    (); break;
		case MDPMdn:     ret = MDPotETPMdnDef     (); break;
		case MDSWGday:   ret = MDPotETSWGdayDef   (); break;
		case MDSWGdn:    ret = MDPotETSWGdnDef    (); break;
		case MDTurc:     ret = MDPotETTurcDef     (); break;
		default:
			fprintf (stderr,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) fprintf (stderr," %s",options [optID]);
			fprintf (stderr,"\n");
			break;
	}
	MFDefLeaving ("PotET");
	return (ret);
}
