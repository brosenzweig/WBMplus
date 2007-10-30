/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDIrrReturnFlow.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDOutIrrReturnFlowID = MFUnset;

int MDIrrReturnFlowDef() {
	int ret;

	if (_MDOutIrrReturnFlowID != MFUnset) return (_MDOutIrrReturnFlowID);

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if (ret == MFUnset) return (MFUnset);
	_MDOutIrrReturnFlowID = MFVarGetID (MDVarIrrReturnFlow,     "mm",   MFInput, MFFlux,  MFBoundary);
    return (_MDOutIrrReturnFlowID);
}
