#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInDayLengthID, _MDInI0HDayID;


static int _MDInAtMeanID,  _MDInSolRadID, _MDInVPressID, _MDInWSpeedID;
static int _MDFAOReferenceETPID = MFUnset;
static int _MDInElevationID =MFUnset;
static int _MDInAtMaxID = MFUnset;
static int _MDInAtMinID =MFUnset;

static void _MDFAOReferenceETP (int itemID) {
/* day-night Penman-Monteith PE in mm for day */
// Input
//	float dayLen;  // daylength in fraction of day
 	float i0hDay;  //  daily potential insolation on horizontal [MJ/m2]

	float airT;    // air temperatur [degree C]
	float airTMin; // daily minimum air temperature [degree C]
	float airTMax; // daily maximum air temperature [degree C]
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
	float vPress;  // daily average vapor pressure [kPa]
	float wSpeed;  // average wind speed for the day [m/s]
// Local
	float solNet;  // average net solar radiation for daytime [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float es_min;  // vapor pressure at airT [kPa]
	float es_max;  // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
	float psychometricConstant;
	float FAOEtp;
	float solNet_MJm2d;
	float nom;
	float denom;
	float meanAirTemp;
	float atmosPressure;
	float elevation;
	float nen;
	float temp;
 
//	dayLen    = MFVarGetFloat (_MDInDayLengthID,    itemID,0);
	i0hDay    = MFVarGetFloat (_MDInI0HDayID,       itemID,0);
	elevation = MFVarGetFloat (_MDInElevationID,    itemID,0); 
	airT      = MFVarGetFloat (_MDInAtMeanID,       itemID,0);
	 
	solRad    = MFVarGetFloat (_MDInSolRadID,       itemID,0);
	vPress    = MFVarGetFloat (_MDInVPressID,       itemID,0);
	wSpeed    = fabs (MFVarGetFloat (_MDInWSpeedID, itemID,0));
	if (wSpeed < 0.2) wSpeed = 0.2;
	
	airTMin = MFVarGetFloat (_MDInAtMinID,        itemID,  0.0);
	airTMax = MFVarGetFloat (_MDInAtMaxID,        itemID,  0.0);
	
	atmosPressure = (293.0 - 0.0065 * elevation) / 293.0;
	atmosPressure = pow (atmosPressure, 5.26);
	atmosPressure = atmosPressure * 101.3;
	psychometricConstant = 0.665 * atmosPressure / 1000.0;

	meanAirTemp  = (airTMin + airTMax) / 2.0;
	solNet       = (1.0 - 0.23) * solRad ;// in MJ/m2 
	es_min       = MDPETlibVPressSat (airTMin);
 	es_max       = MDPETlibVPressSat (airTMax);
 	es           = (es_min + es_max) / 2.0;
	solNet_MJm2d = solNet;  // 0.0864; FAO equation wants SolNet in MJ/m2.
	nen          = 4098 * (0.6108 * exp (17.27 * airT / (airT + 237.3)));
	delta        = nen / ((airT + 237.3)*(airT + 237.3));

 	temp         = es-vPress;
	nom          = 0.408 * delta * solNet_MJm2d + psychometricConstant * 900 / (273.3 + airT) * wSpeed * temp; 
	//FBM nimmt vapor pressure in kPA!
	denom = delta + psychometricConstant*(1+0.34 * wSpeed);

	FAOEtp = nom / denom;
	MFVarSetFloat (_MDFAOReferenceETPID,itemID,FAOEtp);
}

int MDIrrFAOReferenceETPDef () {
	if (_MDFAOReferenceETPID != CMfailed) return (_MDFAOReferenceETPID);

	MFDefEntering ("FAO Reference ETP ");
	if (((_MDInDayLengthID      = MDSRadDayLengthDef ()) == CMfailed) ||
		 ((_MDInI0HDayID        = MDSRadI0HDayDef    ()) == CMfailed) ||
		 ((_MDInSolRadID        = MDSolarRadDef      ()) == CMfailed) ||
	     ((_MDInElevationID     = MFVarGetID (MDVarMeanElevationMeters,          "m",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDInAtMeanID        = MFVarGetID (MDVarAirTemperature,               "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDInAtMinID         = MFVarGetID (MDVarAirTempMinimum,               "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDInAtMaxID         = MFVarGetID (MDVarAirTempMaximum,               "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDInVPressID        = MFVarGetID (MDVarVaporPressure,                "kPa",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDInWSpeedID        = MFVarGetID (MDVarWindSpeed,                    "m/s",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
		 ((_MDFAOReferenceETPID = MFVarGetID (MDVarReferenceEvapotranspiration,  "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed)) return (CMfailed);
    if (MFModelAddFunction (_MDFAOReferenceETP)== CMfailed) return (CMfailed);

	//printf ("RefETP ID %i\n",_MDFAOReferenceETPID);
	MFDefLeaving ("FAO Reference ETP");
	return(_MDFAOReferenceETPID);
}
