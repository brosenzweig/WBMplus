/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDAccBalance.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

//Input;
static int _MDInAccPrecipID    = MFUnset;
static int _MDInAccEvapID      = MFUnset;
static int _MDInAccSMoistChgID = MFUnset;
static int _MDInAccGrdWatChgID = MFUnset;
static int _MDInAccRunoffID    = MFUnset;

//Output
static int _MDOutAccBalanceID  = MFUnset;

static void _MDAccumBalance (int itemID)
{
	double precip;
	double evap;
	double sMoistChg;
	double grdWatChg;
	double runoff;

	if (MFVarTestMissingVal (_MDInAccPrecipID,    itemID) ||
		 MFVarTestMissingVal (_MDInAccEvapID,      itemID) ||
		 MFVarTestMissingVal (_MDInAccSMoistChgID, itemID) ||
		 MFVarTestMissingVal (_MDInAccGrdWatChgID, itemID) ||
		 MFVarTestMissingVal (_MDInAccRunoffID,    itemID)) MFVarSetMissingVal (_MDOutAccBalanceID,itemID);
	else {
		precip     = MFVarGetFloat(_MDInAccPrecipID,    itemID);
		evap       = MFVarGetFloat(_MDInAccEvapID,      itemID);
		sMoistChg  = MFVarGetFloat(_MDInAccSMoistChgID, itemID);
		grdWatChg  = MFVarGetFloat(_MDInAccGrdWatChgID, itemID);
		runoff     = MFVarGetFloat(_MDInAccRunoffID,    itemID);
		MFVarSetFloat(_MDOutAccBalanceID, itemID, precip + evap + sMoistChg + grdWatChg + runoff);
	}
}

enum { MDinput, MDcalc };

int MDAccumBalanceDef() {

	if (_MDOutAccBalanceID != MFUnset) return (_MDOutAccBalanceID);

	MFDefEntering ("Accumulated Balance");

	if (((_MDInAccPrecipID    = MDAccumPrecipDef())    == CMfailed) ||
	    ((_MDInAccRunoffID    = MDAccumRunoffDef())    == CMfailed) ||
	    ((_MDInAccGrdWatChgID = MDAccumGrdWatChgDef()) == CMfailed) ||
	    ((_MDInAccSMoistChgID = MDAccumSMoistChgDef()) == CMfailed) ||
	    ((_MDInAccEvapID      = MDAccumEvapDef())      == CMfailed) ||
	    ((_MDOutAccBalanceID  = MFVarGetID (MDVarAccBalance,  "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed)) return CMfailed;
	_MDOutAccBalanceID = MFVarSetFunction(_MDOutAccBalanceID,_MDAccumBalance);

	MFDefLeaving ("Accumulated Balance");
	return (_MDOutAccBalanceID);	
}
