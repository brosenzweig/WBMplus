/******************************************************************************

GHAAS Water Balance Modell V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

WBMMain.c

balazs.fekete@unh.edu

*******************************************************************************/
#include "wbm.h"

enum { MDhelp, MDpet, MDsurplus, MDinfiltration, MDrunoff, MDdischarge, MDbgc, MDbalance };

int main (int argc,char *argv []) {
	int argNum;
	int  optID = MDbalance;
	const char *optStr, *optName = "Model";
	const char *options [] = { MDHelpStr, "pet", "surplus", "infiltration", "runoff", "discharge", "bgc", "balance", (char *) NULL };

	argNum = MFOptionParse (argc,argv);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDpet:          return (MFModelRun (argc,argv,argNum,MDPotETDef));
		case MDsurplus:      return (MFModelRun (argc,argv,argNum,MDWaterSurplusDef));
		case MDinfiltration: return (MFModelRun (argc,argv,argNum,MDInfiltrationDef));
		case MDrunoff:       return (MFModelRun (argc,argv,argNum,MDRunoffDef));
		case MDdischarge:    return (MFModelRun (argc,argv,argNum,MDDischargeDef));
		case MDbgc:          return (MFModelRun (argc,argv,argNum,MDBgcRoutingDef));
		case MDbalance:      return (MFModelRun (argc,argv,argNum,MDWaterBalanceDef));
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			break;
	}
	return (CMfailed);
}
