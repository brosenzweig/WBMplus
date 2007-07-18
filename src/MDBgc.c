/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDBgc.c  - Routing and processing of nutrients

wil.wollheim@unh.edu

*******************************************************************************/
#include<cm.h>
#include<MF.h>
#include<MD.h>
#include<math.h>
 
static int _MDAirTemperatureID        = MFUnset;
static int _MDDischargeID             = MFUnset;
static int _MDRunoffVolumeID          = MFUnset;
static int _MDVarRiverStorageChangeID = MFUnset;
static int _MDVarRiverStorageID       = MFUnset;
static int _MDResCapacityID           = MFUnset;
static int _MDResStorageID            = MFUnset;

static int _MDNonPointTNSourcesContID = MFUnset;
static int _MDPointTNSourcesID        = MFUnset;

static int _MDTNFluxID                = MFUnset;

static int _MDTNLocalLoadID           = MFUnset;
static int _MDTNStoreWaterID          = MFUnset;
static int _MDTNStoreWaterChangeID    = MFUnset;
static int _MDTNStoreSedsID           = MFUnset;
static int _MDTNStoreSedsChangeID     = MFUnset;
static int _MDTNTotalUptakeID         = MFUnset;
static int _MDTNConcID                = MFUnset;

static void _MDBgcRouting (int itemID) {
//input	
	float airT;
	float discharge;
	float runoff;
	float channelWidth;
	float channelDepth;
	float waterStorageChange;
	float waterStorage;

	float tnNonpointLoadConc; 
	float tnPointLoadFlux; 

//route
	float tnFlux;

//output
	float tnLocalLoad;
	float tnStoreWater;
	float tnStoreWaterChange;
	float tnStoreSeds;
	float tnStoreSedsChange;
	float tnTotalUptake;

// Local
	float tnTotalMassPre;
	float tnTotalMassPost;
	float tnConcPre;
	float tnConcPost;
	float waterT;
	//float UmaxAdj;
	float tnVfAdj;
	float waterTotalVol;
	float tnArealUptake;

	//
	float tnQ10 = 2;
	//float tnUmax = 2.9;
	//float tnKs = 0.416;
	float tnVfref = 35;
	float tnTref = 20;


	if (MFVarTestMissingVal (_MDAirTemperatureID,          itemID) ||
	    MFVarTestMissingVal (_MDDischargeID,               itemID) ||
	    MFVarTestMissingVal (_MDRunoffVolumeID,            itemID) ||
	    MFVarTestMissingVal (_MDVarRiverStorageChangeID,   itemID) ||
	    MFVarTestMissingVal (_MDVarRiverStorageID,         itemID) ||
	    MFVarTestMissingVal (_MDNonPointTNSourcesContID,   itemID) ||
	    MFVarTestMissingVal (_MDPointTNSourcesID,          itemID)) {
		MFVarSetFloat (_MDAirTemperatureID,          itemID, 0.0);
		MFVarSetFloat (_MDDischargeID,               itemID, 0.0);
		MFVarSetFloat (_MDRunoffVolumeID,            itemID, 0.0);
		MFVarSetFloat (_MDVarRiverStorageChangeID,   itemID, 0.0);
		MFVarSetFloat (_MDVarRiverStorageID,         itemID, 0.0);
		MFVarSetFloat (_MDNonPointTNSourcesContID,   itemID, 0.0);
		MFVarSetFloat (_MDPointTNSourcesID,          itemID, 0.0);
	}		  
	else {
		airT               = MFVarGetFloat (_MDAirTemperatureID,         itemID, 0.0);
		discharge          = MFVarGetFloat (_MDDischargeID,              itemID, 0.0);
		runoff             = MFVarGetFloat (_MDRunoffVolumeID,           itemID, 0.0);
		waterStorageChange = MFVarGetFloat ( _MDVarRiverStorageChangeID, itemID, 0.0);
		waterStorage       = MFVarGetFloat ( _MDVarRiverStorageID,       itemID, 0.0);

		tnNonpointLoadConc = MFVarGetFloat (_MDNonPointTNSourcesContID,  itemID, 0.0);
		tnPointLoadFlux    = MFVarGetFloat (_MDPointTNSourcesID,         itemID, 0.0);

		tnFlux             = MFVarGetFloat ( _MDTNFluxID,                itemID, 0.0);
		tnStoreWater       = MFVarGetFloat ( _MDTNStoreWaterID,          itemID, 0.0);
		tnStoreSeds        = MFVarGetFloat ( _MDTNStoreSedsID,           itemID, 0.0);
   }

//////////////////////////////  GUTS
//		  if (itemID==6447) printf("tnStoreWater, %f tnFlux %f \n\n", tnStoreWater, tnFlux);

	waterT = 0.8 + ((26.2 - 0.8) / (1 + exp(0.18 * (13.3 - airT))));

 	channelWidth = 8.3 * pow(discharge,0.5);
	channelDepth = 0.4 * pow(discharge,0.4);
	waterStorage = channelWidth * MFModelGetLength(itemID) * channelDepth;
// The conversion needs attention! Balazs
	tnLocalLoad= runoff * 86400 * tnNonpointLoadConc + tnPointLoadFlux; //kg/d

	//waterStorage needs to be corrected to have a positive value (using mean depth across length of stream
	tnTotalMassPre = tnLocalLoad + tnStoreWater + tnFlux;  //kg
	waterTotalVol = (waterStorage + waterStorageChange * MDConst_m3PerSecTOm3PerDay) + (discharge * MDConst_m3PerSecTOm3PerDay); 
	                // waterStorageChange is negative if storage declined - this should be reflected in
	                // discharge, and will need to remove this from the total volume to avoid double counting
	                // Visa versa if waterStorage is positive
		
	if (!isnan(waterTotalVol) && waterTotalVol != 0){
		tnConcPre = tnTotalMassPre / waterTotalVol;  //kg/m3
	}
	else tnConcPre = 0.0;
		  
	tnVfAdj = tnVfref * pow(tnQ10, ((waterT - tnTref) / 10));
	tnArealUptake =  (tnVfAdj *  tnConcPre) / 365;  //kg/m2/d
	tnTotalUptake = tnArealUptake * MFModelGetLength(itemID) * channelWidth;

	if (tnTotalMassPre < tnTotalUptake) tnTotalUptake = tnTotalMassPre;

	tnTotalMassPost = tnTotalMassPre - tnTotalUptake;

	if (!isnan(waterTotalVol) && waterTotalVol != 0){
	tnConcPost = tnTotalMassPost / waterTotalVol;
	}
	else tnConcPost = 0.0;

	tnFlux = (discharge * MDConst_m3PerSecTOm3PerDay) * tnConcPost;
		  
	tnStoreSeds = tnStoreSeds + tnTotalUptake;
	tnStoreSedsChange = tnTotalUptake;
	tnStoreWaterChange = waterStorageChange * MDConst_m3PerSecTOm3PerDay * tnConcPost; //check that this value matches day to day change in tn storage
	tnStoreWater = waterStorage * tnConcPost;

//  if (itemID==6447) printf("tnTotalMassPre %f tnNonpointLoadConc %f tnPointLoadFlux %f tnLocalLoad %f WaterTotalVol %f tnStoreWater %f tnStoreWaterChange %f tnConcPre %f tnConcPost %f tnTotalMassPost %f \n waterStorage %f  waterStorageChange %f tnTotalUptake %f runoff %f discharge %f conversion %f tnFlux %f \n\n",
//							tnTotalMassPre,tnNonpointLoadConc, tnPointLoadFlux, tnLocalLoad, waterTotalVol, tnStoreWater, tnStoreWaterChange, tnConcPre, tnConcPost, tnTotalMassPost, waterStorage, waterStorageChange, tnTotalUptake, runoff, discharge, MDConst_m3PerSecTOm3PerDay, tnFlux );

///////////////////////////////////
		
	//(Variable ID , itemID, value)
	MFVarSetFloat (_MDTNFluxID,itemID,tnFlux); 
	 	
	MFVarSetFloat (_MDTNLocalLoadID,itemID,tnLocalLoad); 
	MFVarSetFloat (_MDTNStoreWaterID,itemID,tnStoreWater); 
	MFVarSetFloat (_MDTNStoreWaterChangeID,itemID,tnStoreWaterChange); 
	MFVarSetFloat (_MDTNStoreSedsID,itemID,tnStoreSeds); 
	MFVarSetFloat (_MDTNStoreSedsChangeID,itemID,tnStoreSedsChange); 
	MFVarSetFloat (_MDTNTotalUptakeID,itemID,tnTotalUptake); 
	MFVarSetFloat (_MDTNConcID,itemID,tnConcPost); 

}

// define the variable needed by the function
int MDBgcRoutingDef () {
  
	MFDefEntering ("Nutrient Calculation");
			
	if (_MDTNFluxID != MFUnset)	return (_MDTNFluxID);
   // Input
	if (((_MDAirTemperatureID        = MFVarGetID (MDVarAirTemperature,         "degC",    MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDRunoffVolumeID          = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDDischargeID             = MDDischargeDef ()) == CMfailed) ||
	    ((_MDVarRiverStorageChangeID = MFVarGetID (MDVarRiverStorageChange,     "m3/s",    MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDVarRiverStorageID       = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDResCapacityID           = MFVarGetID (MDVarReservoirCapacity,      "km3",     MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDResStorageID            = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDNonPointTNSourcesContID = MFVarGetID (MDVarNonPointTNSourcesCont,  "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDPointTNSourcesID        = MFVarGetID (MDVarPointTNSources,         "kg/day",  MFInput, MFState, MFBoundary))  == CMfailed) ||
	// Output
	    ((_MDTNLocalLoadID           = MFVarGetID (MDVarBgcTNLocalLoad,         "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDTNStoreWaterID          = MFVarGetID (MDVarBgcTNStoreWater,        "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDTNStoreWaterChangeID    = MFVarGetID (MDVarBgcTNStoreWaterChange,  "kg/day",  MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDTNStoreSedsID           = MFVarGetID (MDVarBgcTNStoreSeds,         "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDTNStoreSedsChangeID     = MFVarGetID (MDVarBgcTNStoreSedsChange,   "kg/day",  MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDTNTotalUptakeID         = MFVarGetID (MDVarBgcTNTotalUptake,       "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDTNConcID                = MFVarGetID (MDVarBgcTNConc,              "kg/m3",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDTNFluxID                = MFVarGetID (MDVarBgcTNFlux  ,            "kg/day",  MFRoute,  MFState, MFBoundary))  == CMfailed))
	  		return (CMfailed); 
	_MDTNFluxID = MFVarSetFunction(_MDTNFluxID,_MDBgcRouting);
	MFDefLeaving ("Nutrient Calculation");
	return (_MDTNFluxID); 
}
