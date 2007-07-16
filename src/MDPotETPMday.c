/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

MDPotETPMday.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInDayLengthID,     _MDInI0HDayID;
static int _MDInCParamAlbedoID,  _MDInCParamCHeightID;
static int _MDInCParamR5ID,      _MDInCParamCDID,     _MDInCParamCRID,  _MDInCParamGLMaxID, _MDInCParamZ0gID;
static int _MDInLeafAreaIndexID, _MDInStemAreaIndexID;

static int _MDInAtMeanID, _MDInSolRadID, _MDInVPressID, _MDInWSpeedID;
static int _MDOutPetID = CMfailed;

static void _MDPotETPMday (int itemID) {
/* daily Penman-Monteith PE in mm for day */
/* Input */
	float dayLen;  /* daylength in fraction of day */
 	float i0hDay;  /* daily potential insolation on horizontal [MJ/m2] */
	float albedo;  /* albedo  */
	float height;  /* canopy height [m] */
	float r5;      /* solar radiation at which conductance is halved [W/m2] */
	float cd;      /* vpd at which conductance is halved [kPa] */
	float cr;      /* light extinction coefficient for projected LAI */
	float glMax;   /* maximum leaf surface conductance for all sides of leaf [m/s] */
	float z0g;     /* z0g       - ground surface roughness [m] */
 	float lai;     /* projected leaf area index */
	float sai;     /* projected stem area index */
	float airT;    /* air temperatur degree C */
	float solRad;  /* daily solar radiation on horizontal [MJ/m2] */
	float vPress;  /* daily average vapor pressure [kPa] */
	float wSpeed;  /* average wind speed for the day [m/s]  */
	float sHeat = 0.0; /* average subsurface heat storage for day [W/m2] */
/* Local */
	float solNet;  /* average net solar radiation for daytime [W/m2] */
	float lngNet;	/* average net longwave radiation for day  [W/m2] */
	float za;      /* reference height [m] */
 	float disp;    /* height of zero-plane [m] */
	float z0;      /* roughness parameter [m]  */
	float aa;		/* available energy [W/m2] */
	float es;      /* vapor pressure at airT [kPa] */
	float delta;   /* dEsat/dTair [kPa/K] */
 	float dd;      /* vapor pressure deficit [kPa] */
 	float rc;		/* canopy resistance [s/m] */
	float ra;		/*	aerodynamic resistance [s/ma] */
	float le;		/* latent heat [W/m2] */
/* Output */
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID,    itemID) ||
		 MFVarTestMissingVal (_MDInI0HDayID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamAlbedoID, itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID,itemID) ||
		 MFVarTestMissingVal (_MDInCParamR5ID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCDID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCRID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamGLMaxID,  itemID) ||
		 MFVarTestMissingVal (_MDInCParamZ0gID,    itemID) ||
		 MFVarTestMissingVal (_MDInLeafAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInStemAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,       itemID) ||
		 MFVarTestMissingVal (_MDInSolRadID,       itemID) ||
		 MFVarTestMissingVal (_MDInVPressID,       itemID) ||
		 MFVarTestMissingVal (_MDInWSpeedID,       itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID); return; }

	dayLen  = MFVarGetFloat (_MDInDayLengthID,    itemID);
	i0hDay  = MFVarGetFloat (_MDInI0HDayID,       itemID);
	albedo  = MFVarGetFloat (_MDInCParamAlbedoID, itemID);
	height  = MFVarGetFloat (_MDInCParamCHeightID,itemID);
	r5      = MFVarGetFloat (_MDInCParamR5ID,     itemID);
	cd      = MFVarGetFloat (_MDInCParamCDID,     itemID);
	cr      = MFVarGetFloat (_MDInCParamCRID,     itemID);
	glMax   = MFVarGetFloat (_MDInCParamGLMaxID,  itemID);
	z0g     = MFVarGetFloat (_MDInCParamZ0gID,    itemID);
	lai     = MFVarGetFloat (_MDInLeafAreaIndexID,itemID);
	sai     = MFVarGetFloat (_MDInStemAreaIndexID,itemID);
	airT    = MFVarGetFloat (_MDInAtMeanID,       itemID);
	solRad  = MFVarGetFloat (_MDInSolRadID,       itemID);
	vPress  = MFVarGetFloat (_MDInVPressID,       itemID);
	wSpeed  = fabs (MFVarGetFloat (_MDInWSpeedID, itemID));
	if (wSpeed < 0.2) wSpeed = 0.2;

	solNet = (1.0 - albedo) * solRad / MDConstIGRATE;
	lngNet = MDSRadNETLong (i0hDay,airT,solRad,vPress);

	za     = height + MDConstZMINH;
	disp   = MDPETlibZPDisplacement (height,lai,sai,z0g);
	z0     = MDPETlibRoughness (disp,height,lai,sai,z0g);
	aa     = solNet + lngNet - sHeat;
	es     = MDPETlibVPressSat (airT);
	delta  = MDPETlibVPressDelta (airT);
	dd     = es - vPress; 
	rc     = MDPETlibCanopySurfResistance (airT,solRad,dd,lai,sai,r5,cd,cr,glMax);
	ra     = log ((za - disp) / z0);
	ra     = ra * ra / (0.16 * wSpeed);
	le     = MDPETlibPenmanMontieth (aa, dd, delta, ra, rc);

	pet = MDConstEtoM * MDConstIGRATE * le; 
	if (pet<=-2.0)printf("PMday negativ = %f solnet = %f le = %f height = %f wSpeed = %f SolRad =%f \n",pet,solNet,le,height,wSpeed, solRad);
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETPMdayDef () {
	if (_MDOutPetID != CMfailed) return (_MDOutPetID);

	MFDefEntering ("PotET Penman Monteith (day)");
	if (((_MDInDayLengthID     = MDSRadDayLengthDef ()) == CMfailed) ||
		 ((_MDInI0HDayID        = MDSRadI0HDayDef    ()) == CMfailed) ||
	    ((_MDInCParamAlbedoID  = MDCParamAlbedoDef  ()) == CMfailed) ||
		 ((_MDInCParamCHeightID = MDCParamCHeightDef ()) == CMfailed) ||
		 ((_MDInCParamR5ID      = MDCParamR5Def      ()) == CMfailed) ||
		 ((_MDInCParamCDID      = MDCParamCDDef      ()) == CMfailed) ||
		 ((_MDInCParamCRID      = MDCParamCRDef      ()) == CMfailed) ||
		 ((_MDInCParamGLMaxID   = MDCParamGLMaxDef   ()) == CMfailed) ||
		 ((_MDInCParamZ0gID     = MDCParamZ0gDef     ()) == CMfailed) ||
		 ((_MDInLeafAreaIndexID = MDLeafAreaIndexDef ()) == CMfailed) ||
		 ((_MDInStemAreaIndexID = MDStemAreaIndexDef ()) == CMfailed) ||
		 ((_MDInSolRadID        = MDSolarRadDef      ()) == CMfailed) ||
		 ((_MDInAtMeanID  = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDInVPressID  = MFVarGetID (MDVarVaporPressure,  "kPa",   MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDInWSpeedID  = MFVarGetID (MDVarWindSpeed,      "m/s",   MFInput,  MFState, false)) == CMfailed) ||
		 ((_MDOutPetID    = MFVarGetID (MDVarPotEvapotrans,  "mm",    MFOutput, MFFlux,  false)) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Penman Monteith (day)");
	return(MFVarSetFunction (_MDOutPetID,_MDPotETPMday));
}
