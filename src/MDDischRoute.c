/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDischCalculated.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

enum { MDhelp, MDaccumulate, MDmuskingum, MDcascade };

int MDDischRouteDef() {
	int optID = MDaccumulate, ret;
	const char *optStr, *optName = MDModDischRoute;
	const char *options []    = { MDHelpStr, "accumulate", "muskingum", "cascade", (char *) NULL };

	MFDefEntering ("Route Discharge");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	if (((optStr = MFOptionGet (MDModIrrigation)) != (char *) NULL) &&
	    ((irrOptID = CMoptLookup (irrOptions, optStr, true)) == CMfailed) optID = MDhelp; 
	switch (optID) {
		case MDaccumulate: ret = MDDischRouteAccumulateDef (); break;
		case MDmuskingum:  ret = MDDischRouteMuskingumDef  (); break;
		case MDcascade:    ret = MDDischRouteCascadeDef    (); break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Route Discharge");
	return (ret));
}
