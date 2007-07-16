/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDBgc.c  - Routing and processing of nutrients

wil.wollheim@unh.edu

*******************************************************************************/
#include<cm.h>
#include<MF.h>
#include<MD.h>
#include<math.h>
 
static int _MDAirTemperatureID;
static int _MDDischargeID;
static int _MDRunoffVolumeID;
static int _MDVarRiverStorageChangeID;
static int _MDVarRiverStorageID;
static int _MDResCapacityID;
static int _MDResStorageID;

static int _MDNonPointTNSourcesContID;
static int _MDPointTNSourcesID;

static int _MDTNFluxID = CMfailed; // this differs because it is MDTNFlux is output of the function

static int _MDTNLocalLoadID;
static int _MDTNStoreWaterID;
static int _MDTNStoreWaterChangeID ;
static int _MDTNStoreSedsID;
static int _MDTNStoreSedsChangeID;
static int _MDTNTotalUptakeID;
static int _MDTNConcID;

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

   /* Local */
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

/*Parameters */
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
   airT = MFVarGetFloat (_MDAirTemperatureID, itemID);
	discharge = MFVarGetFloat (_MDDischargeID, itemID);
   runoff = MFVarGetFloat (_MDRunoffVolumeID, itemID);
   waterStorageChange = MFVarGetFloat ( _MDVarRiverStorageChangeID, itemID);
   waterStorage = MFVarGetFloat ( _MDVarRiverStorageID, itemID);

	tnNonpointLoadConc = MFVarGetFloat (_MDNonPointTNSourcesContID, itemID);
   tnPointLoadFlux = MFVarGetFloat (_MDPointTNSourcesID, itemID);

   tnFlux = MFVarGetFloat ( _MDTNFluxID, itemID);
   tnStoreWater = MFVarGetFloat ( _MDTNStoreWaterID, itemID);
   tnStoreSeds = MFVarGetFloat ( _MDTNStoreSedsID, itemID);
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
// 
 
// define the variable needed by the function
int MDBgcRoutingDef () {
  
	MFDefEntering ("Nutrient Calculation");
			
	if (_MDTNFluxID != CMfailed)	return (_MDTNFluxID);
   //Input
	if (((_MDAirTemperatureID        = MFVarGetID (MDVarAirTemperature,         "degC",    MFInput, MFState,  false)) == CMfailed) ||
	    ((_MDRunoffVolumeID          = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDDischargeID             = MDDischargeDef ()) == CMfailed) ||
	    ((_MDVarRiverStorageChangeID = MFVarGetID (MDVarRiverStorageChange,     "m3/s",      MFInput, MFState, false))  == CMfailed) ||
	    ((_MDVarRiverStorageID       = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput, MFState, true))   == CMfailed) ||
	    ((_MDResCapacityID           = MFVarGetID (MDVarReservoirCapacity,      "km3",     MFInput, MFState, false))  == CMfailed) ||
	    ((_MDResStorageID            = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, true))   == CMfailed) ||
	    ((_MDNonPointTNSourcesContID = MFVarGetID (MDVarNonPointTNSourcesCont,  "kg/m3",   MFInput, MFState, false))  == CMfailed) ||
	    ((_MDPointTNSourcesID        = MFVarGetID (MDVarPointTNSources,         "kg/day",  MFInput, MFState, false))  == CMfailed) ||

	//OUTPUT
	    ((_MDTNLocalLoadID           = MFVarGetID (MDVarBgcTNLocalLoad,         "kg/day",  MFOutput, MFState, false))  == CMfailed) ||
	    ((_MDTNStoreWaterID          = MFVarGetID (MDVarBgcTNStoreWater,        "kg",      MFOutput, MFState, true))   == CMfailed) ||
	    ((_MDTNStoreWaterChangeID    = MFVarGetID (MDVarBgcTNStoreWaterChange,  "kg/day",  MFOutput, MFState, true))   == CMfailed) ||
	    ((_MDTNStoreSedsID           = MFVarGetID (MDVarBgcTNStoreSeds,         "kg",      MFOutput, MFState, true))   == CMfailed) ||
	    ((_MDTNStoreSedsChangeID     = MFVarGetID (MDVarBgcTNStoreSedsChange,   "kg/day",  MFOutput, MFState, true))   == CMfailed) ||
	    ((_MDTNTotalUptakeID         = MFVarGetID (MDVarBgcTNTotalUptake,       "kg/day",  MFOutput, MFState, false))  == CMfailed) ||
	    ((_MDTNConcID                = MFVarGetID (MDVarBgcTNConc,              "kg/m3",   MFOutput, MFState, false))  == CMfailed) ||
	    ((_MDTNFluxID                = MFVarGetID (MDVarBgcTNFlux  ,            "kg/day",  MFRoute,  MFState, false))  == CMfailed))
	  		return (CMfailed); 
	_MDTNFluxID = MFVarSetFunction(_MDTNFluxID,_MDBgcRouting);
	MFDefLeaving ("Nutrient Calculation");
	return (_MDTNFluxID); 
}
