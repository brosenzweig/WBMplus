/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischarge.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

enum { MDhelp, MDaccumulate, MDmuskingum, MDcascade };


int MDDischargeDef() {
	int optID = MDaccumulate, ret;
	const char *optStr, *optName = MDVarRiverDischarge;
	const char *options [] = { MDHelpStr, "accumulate", "muskingum", "cascade", (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("Discharge");
	 
	switch (optID) {
		case MDaccumulate: ret = MDDischAccumulateDef (); break;
		case MDmuskingum:  ret = MDDischMuskingumDef  (); break;
		case MDcascade:    ret = MDDischCascadeDef    (); break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
		}
	MFDefLeaving ("Discharge");
	return (ret);
}
