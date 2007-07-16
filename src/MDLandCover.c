/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDLandCover.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>


static int _MDOutCoverID = CMfailed;
static int _MDInCroplandPctID;

static int _MDInSAGEVegID;

static void _MDLCSAGEVegToCover (int itemID) {
	if (MFVarTestMissingVal (_MDInCroplandPctID, itemID) |
		 MFVarTestMissingVal (_MDInSAGEVegID,     itemID)) { MFVarSetMissingVal (_MDOutCoverID, itemID); return; }

	switch (MFVarGetInt (_MDInSAGEVegID,itemID)) {
		case  1: MFVarSetInt (_MDOutCoverID,itemID, 8); break;
		case  2:
		case  3:
		case  4:
		case  5:
		case  6: MFVarSetInt (_MDOutCoverID,itemID, 2); break;
		case  7:
		case  8: MFVarSetInt (_MDOutCoverID,itemID, 1); break;
		case  9: MFVarSetInt (_MDOutCoverID,itemID, 2); break;
		case 10: MFVarSetInt (_MDOutCoverID,itemID, 3); break;
		case 11: MFVarSetInt (_MDOutCoverID,itemID, 4); break;
		case 12:
		case 13: MFVarSetInt (_MDOutCoverID,itemID, 3); break;
		case 14:
		case 15: MFVarSetInt (_MDOutCoverID,itemID, 7); break;
	}
}

static int _MDInTEMVegID;

static void _MDLCTEMVegToCover (int itemID) {
	int wetland = 3;
	
	if (MFVarTestMissingVal (_MDInCroplandPctID, itemID) |
	    MFVarTestMissingVal (_MDInTEMVegID,      itemID)) { MFVarSetMissingVal (_MDOutCoverID, itemID); return; }

	switch (MFVarGetInt (_MDInTEMVegID,itemID)) {
		case   4: /* Conifer forest */
		case   9: MFVarSetInt (_MDOutCoverID,itemID, 0); break;
		case   8: /* Broadleaf forest */
		case  10:
		case  18:
		case  16: 
		case  33: MFVarSetInt (_MDOutCoverID,itemID, 1); break;
		case  14: /* Savannah */
		case  15:
		case  19:
		case  27:
		case  31:
		case  35: MFVarSetInt (_MDOutCoverID,itemID, 2); break;
		case   6: /* Grassland */
		case  12:
		case  13: MFVarSetInt (_MDOutCoverID,itemID, 3); break;
		case   2: /* Tundra */
		case   3: MFVarSetInt (_MDOutCoverID,itemID, 4); break;
					 /* Cultivation */
					 /* Desert */
		case  21: MFVarSetInt (_MDOutCoverID,itemID, 6); break;
		case -36: /* Water */
		case -37: MFVarSetInt (_MDOutCoverID,itemID, 7); break;
		case  11: /* Wetland forest */
		case  17:
		case  20:
		case  25:
		case  29: if (wetland == 1) MFVarSetMissingVal (_MDOutCoverID,itemID);
					 else MFVarSetInt (_MDOutCoverID,itemID,wetland == 2 ? 7 : 1); break;
					 /* Wetland Savannah */
		case   5: if (wetland == 1) MFVarSetMissingVal (_MDOutCoverID,itemID);
					 else MFVarSetInt (_MDOutCoverID,itemID,wetland == 2 ? 7 : 2); break;
		case   7: /* Wetland Tundra */
		case  22:
		case  23:
		case  24:
		case  26:
		case  28:
		case  30: if (wetland == 1) MFVarSetMissingVal (_MDOutCoverID,itemID);
					 else MFVarSetInt (_MDOutCoverID,itemID,wetland == 2 ? 7 : 4); break;
		case   1:
		default:	MFVarSetMissingVal (_MDOutCoverID,itemID);
	}
}

enum { MDhelp, MDinput, MDLCLookup, MDLCSAGEVeg, MDLCTEMVeg };

int MDLandCoverDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarWBMCover;
	const char *options [] = { MDHelpStr, MDInputStr, "lookup", "SAGEVeg", "TEMVeg",(char *) NULL };

	if (_MDOutCoverID != CMfailed) return (_MDOutCoverID);

	MFDefEntering ("Landcover");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutCoverID = MFVarGetID (MDVarWBMCover,   MFNoUnit, MFInput, MFState, false); break;
		case MDLCLookup:
			break;
		case MDLCSAGEVeg:
			  if(  ((_MDInSAGEVegID     = MFVarGetID (MDVarSAGEVegCover, MFNoUnit, MFInput, MFState, false)) == CMfailed) ||
			    ((_MDOutCoverID      = MFVarGetID (MDVarWBMCover,     MFNoUnit, MFByte,  MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCoverID = MFVarSetFunction (_MDOutCoverID,_MDLCSAGEVegToCover);
			break;
		case MDLCTEMVeg:
			  if ( ((_MDInTEMVegID      = MFVarGetID (MDVarTEMVegCover,  MFNoUnit, MFInput, MFState, false)) == CMfailed) ||
			    ((_MDOutCoverID      = MFVarGetID (MDVarWBMCover,     MFNoUnit, MFByte,  MFState, false)) == CMfailed))
				return (CMfailed);
			_MDOutCoverID = MFVarSetFunction (_MDOutCoverID,_MDLCTEMVegToCover);
			break;
		default:
			CMmsgPrint (CMmsgInfo,"Help [%s options]:",optName);
			for (optID = 1;options [optID] != (char *) NULL;++optID) CMmsgPrint (CMmsgInfo," %s",options [optID]);
			CMmsgPrint (CMmsgInfo,"\n");
			return (CMfailed);
	}
	MFDefLeaving ("Landcover");
	return (_MDOutCoverID);
}
