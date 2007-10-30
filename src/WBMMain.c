/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

WBMMain.c

balazs.fekete@unh.edu

*******************************************************************************/
#include "wbm.h"

enum { MDpet, MDsurplus, MDinfiltration, MDrunoff, MDdischarge,  MDbalance, MDgeometry, MDwatertemp, MDbgc};

int main (int argc,char *argv []) {
	int argNum;
	int  optID = MDbalance;
	const char *optStr, *optName = MDOptModel;
	const char *options [] = { "pet", "surplus", "infiltration", "runoff", "discharge",  "balance", "geometry", "watertemp", "bgc", (char *) NULL };

	argNum = MFOptionParse (argc,argv);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDpet:          return (MFModelRun (argc,argv,argNum,MDRainPotETDef));
		case MDsurplus:      return (MFModelRun (argc,argv,argNum,MDRainWaterSurplusDef));
		case MDinfiltration: return (MFModelRun (argc,argv,argNum,MDRainInfiltrationDef));
		case MDrunoff:       return (MFModelRun (argc,argv,argNum,MDRunoffDef));
		case MDdischarge:    return (MFModelRun (argc,argv,argNum,MDDischargeDef));
		case MDbalance:      return (MFModelRun (argc,argv,argNum,MDWaterBalanceDef));
		case MDgeometry:     return (MFModelRun (argc,argv,argNum,MDRiverWidthDef));
		case MDwatertemp:    return (MFModelRun (argc,argv,argNum,MDWTempRiverDef));
		case MDbgc:          return (MFModelRun (argc,argv,argNum,MDBgcRoutingDef));
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	return (CMfailed);
}
