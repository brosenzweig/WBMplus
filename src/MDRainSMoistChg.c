/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRainSMoistChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static float _MDAWCap, _MDPet, _MDWaterIn;

static float _MDSoilMoistALPHA = 5.0;

static float _MDDryingFunc (float sMoist) {
	float gm;
	gm = (1.0 - exp (- _MDSoilMoistALPHA * sMoist / _MDAWCap)) / (1.0 - exp (- _MDSoilMoistALPHA)); 
	return (gm);
}

// Input
static int _MDInAirTMeanID          = MFUnset;
static int _MDInPrecipID            = MFUnset;
static int _MDInPotETID             = MFUnset;
static int _MDInInterceptID         = MFUnset;
static int _MDInSPackChgID          = MFUnset;
static int _MDInSoilAvailWaterCapID = MFUnset;
static int _MDInIrrAreaFracID       = MFUnset;
 

// Output
static int _MDOutEvaptrsID          = MFUnset;
static int _MDOutSoilMoistCellID    = MFUnset;
static int _MDOutSoilMoistID        = MFUnset;
static int _MDOutSMoistChgID        = MFUnset;
 
 

static void _MDRainSMoistChg (int itemID) {	
// Input
	float airT;            // Air temperature [degreeC]
	float precip;          // Precipitation [mm/dt]
	float pet;             // Potential evapotranspiration [mm/dt]
	float intercept=0;       // Interception (when the interception module is turned on) [mm/dt]
	float sPackChg;        // Snow pack change [mm/dt]
	float irrAreaFrac=0;     // Irrigated area fraction
	float impAreaFrac=0;     // Impervious area fraction RJS 01-17-08
	float H2OAreaFrac=0;     // water area fraction RJS 01-17-08
//	float runofftoPerv;    // runoff from impervious to pervious [mm/dt]  RJS 01-17-08
	float def;             // water deficit [mm/dt]
	float prevSMoist;      // soil moisture from previous time step [mm/dt]
	float Xs=0;
	float Xr=0;
// Output
	float sMoist=0;          // Soil moisture [mm/dt]
	float sMoistChg=0;       // Soil moisture change [mm/dt]
	float transp=0;          // Transpiration [mm]
	float evapotrans;
	float excess=0;
	airT         = MFVarGetFloat (_MDInAirTMeanID,          itemID, 0.0);
	precip       = MFVarGetFloat (_MDInPrecipID,            itemID, 0.0);
 	sPackChg     = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	pet          = MFVarGetFloat (_MDInPotETID,             itemID, 0.0);
	_MDAWCap     = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	prevSMoist       = MFVarGetFloat (_MDOutSoilMoistCellID,    itemID, 0.0);
	intercept    = _MDInInterceptID   != MFUnset ? MFVarGetFloat (_MDInInterceptID,   itemID, 0.0) : 0.0;
	irrAreaFrac  = _MDInIrrAreaFracID != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0) : 0.0;
	_MDWaterIn   = precip - intercept - sPackChg ;  //RJS 01-29-08
	_MDWaterIn   = precip -sPackChg ;  //RJS 01-29-08
	
	if (prevSMoist < 0.0) prevSMoist = 0.0;
	 	 
	if (airT > 0.0) {
	
		if (pet < 0.0) pet = 0.0; 
		if (_MDAWCap > 0.0) {	
			_MDPet     = pet;	
			if (_MDAWCap> prevSMoist){} 
			def = _MDAWCap - prevSMoist + _MDPet;
			if ((_MDWaterIn >=_MDPet) && (def > _MDWaterIn)) {
		//		if (_MDWaterIn >= _MDPet) {
				sMoistChg = _MDWaterIn - _MDPet;
				//if (itemID==58215)printf("hier dSM=%f WaterIn %f\n",sMoistChg, _MDWaterIn);
			}
			if (_MDWaterIn < _MDPet) {
				sMoistChg = (-1.0 * _MDDryingFunc(prevSMoist))*(_MDPet - _MDWaterIn);
			//	if (itemID==58215)printf("hier dSM=%f\n",sMoistChg);
			}
			if (def <= _MDWaterIn) {
				sMoistChg = def-_MDPet;
			}
		}
		else  sMoistChg = 0.0;
		
		if (_MDWaterIn<def)Xr=0;		
		if (_MDWaterIn>=def)Xr=precip - intercept-def;
	//	
		if (fabs(sPackChg) < def)Xs=0;
		if (fabs(sPackChg) >= def)Xs=fabs(sPackChg) - def;
	
		
	} //temp
	else { transp =  0.0; sMoistChg=0;}
	//sMoistChg=0;
	
	sMoist = prevSMoist + sMoistChg;

	if (sMoist >=_MDAWCap)   {
	//	if (itemID==50896)printf ("SM %f dsm %f prevSM %f\n",sMoist,sMoistChg,prevSMoist);
				 
				sMoist = _MDAWCap;
				sMoistChg = def-_MDPet ;
				
			}

	//	transp = precip - intercept - sPackChg - sMoistChg;
			if (_MDWaterIn < _MDPet)transp=_MDWaterIn - sMoistChg;
			if (_MDWaterIn >= _MDPet)transp = _MDPet;
			if (_MDAWCap <=0)transp=0; 
	
			

			excess=Xs+Xr;	
			evapotrans =  transp;
	excess = precip - sPackChg - evapotrans - sMoistChg;	
	

	if (sMoist<0)printf("Alram %f \n",sMoist);
	float balance = _MDWaterIn - evapotrans-sMoistChg-excess;
	if((fabs(balance) > 0.001)&& (_MDAWCap > 0)) printf("balance = %f sMoist = %f, precip=%f sMoistChg = %f, prevSMoist = %f, transp = %f, _MDWaterIn = %f, _MDPet = %f, excess = %f, def = %f itemID = %i AWC %f\n", balance,sMoist,precip, sMoistChg, prevSMoist, transp, _MDWaterIn, _MDPet, excess, def,itemID,_MDAWCap );
//	if(itemID == 100) printf("balance = %f\n", balance);
	
	
	MFVarSetFloat (_MDOutSoilMoistCellID, itemID, sMoist);
	MFVarSetFloat (_MDOutEvaptrsID,       itemID,       evapotrans * (1.0 - irrAreaFrac )); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSoilMoistID,     itemID,       sMoist     * (1.0 - irrAreaFrac )); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSMoistChgID,     itemID,       sMoistChg  * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
//	if (itemID==104)printf("eva  %f delta SM %f\n",evapotrans, sMoistChg);
	
	//if(itemID == 58215) printf("sMoist = %f, sMoistChg = %f, AWC = %f, pet = %f, airT = %f pet =%f \n", sMoist, sMoistChg, _MDAWCap, _MDPet, airT,_MDPet );
	
}

int MDRainSMoistChgDef () {
	int ret = 0;
	float par;
	const char *optStr;
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

	MFDefEntering ("Rainfed Soil Moisture");
	if (((optStr = MFOptionGet (MDParSoilMoistALPHA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSoilMoistALPHA = par;

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset)  &&
	  ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) )
		return (CMfailed);
	if (((_MDInPrecipID            = MDPrecipitationDef     ()) == CMfailed) ||
	    ((_MDInSPackChgID          = MDSPackChgDef          ()) == CMfailed) ||
	    ((_MDInPotETID             = MDRainPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID         = MDRainInterceptDef     ()) == CMfailed) ||
	    ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInAirTMeanID          = MFVarGetID (MDVarAirTemperature,             "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutEvaptrsID          = MFVarGetID (MDVarRainEvapotranspiration,     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSoilMoistCellID    = MFVarGetID (MDVarRainSoilMoistureCell,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSoilMoistID        = MFVarGetID (MDVarRainSoilMoisture,           "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutSMoistChgID        = MFVarGetID (MDVarRainSoilMoistChange,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Soil Moisture");
	return (_MDOutSMoistChgID);
}
