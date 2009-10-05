/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDBgc_LitterDynamics.c  - Litter only


wil.wollheim@unh.edu

*******************************************************************************/
#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

static int _MPOCDischargeID       = MFUnset;
static int _MPOCRiverWidthID      = MFUnset;
static int _MPOCWTempRiverRouteID = MFUnset; // water temperature function
static int _MPOCRiverLightID      = MFUnset;
static int _MPOCRiverGPPID      = MFUnset;
static int _MPOCWTempID         = MFUnset; // water temperature
static int _MPOCSubGridNetworkID         = MFUnset; //define lengths of stream

static int _MPOCRunoffVolumeID    = MFUnset;
static int _MPOCRunoffID          = MFUnset;
static int _MPOCRiverStorageChgID = MFUnset;
static int _MPOCRiverStorageID    = MFUnset;
//static int _MPOCResCapacityID     = MFUnset;
static int _MPOCResStorageID      = MFUnset;
static int _MPOCResStorageChangeID      = MFUnset;

//static int _MDPointSources_UrbanPopID  = MFUnset;
static int _MDNonPoint_POCID      = MFUnset;
static int _MDPointSources_POCID  = MFUnset;
static int _MPOCSinuosityID       = MFUnset;
static int _MPOCPointScenarioID       = MFUnset;

static int _MDConc_POCID             = MFUnset;
static int _MDFlux_POCID             = MFUnset;
static int _MDLocalIn_POCID          = MFUnset;
static int _MDRemoval_POCID          = MFUnset;
static int _MDStorage_POCID          = MFUnset;
static int _MDDeltaStorage_POCID     = MFUnset;


static int _MDDenit_POCID            = MFUnset;
static int _MDAssim_POCID            = MFUnset;
static int _MDRemin_POCID            = MFUnset;
static int _MDBiomass_POCID            = MFUnset;
static int _MDDeltaBiomass_POCID            = MFUnset;
  
static int _MDConcMixing_POCID       = MFUnset;
static int _MDFluxMixing_POCID       = MFUnset;
static int _MDStorageMixing_POCID    = MFUnset;
static int _MDDeltaStorageMixing_POCID= MFUnset;

static int _MDDrying_POCID            = MFUnset;
static int _MDDryingMixing_POCID      = MFUnset;

static void _MDPOCPLUSBIOMASSRouting (int itemID) {
//input	
		// drivers
		float discharge;
		float width;
		
		float sinuosity;
        float waterT;
        float TotalVol;
        float TotalArea;
        int   day;
        int   month;
         
        // Local inputs: Distinguish between leaf litter and POC inputs via erosion or other sources 
        //     assume C:N is given to derive N loading with C loading THIS MAY CHANGE
		float LitterCLocalIn;  //kg C / m2 / d .  area is per area of canopy.  local litter allochthonous inputs.  Assumed to settle locally.
		float LitterC2NLocalIn; // molar C:N ratio of local litter allochthonous inputs
		float LitterNLocalIn;
		float CanopyWidth;  //to get inputs of only overhanging 
		float LitterBlowIn;  //factor increase of litter inputs due to wind (See Webster Book Chapter)
		
		
		//Sediment stocks.  Two storage pools.
		float C_Litter; // storage on stream bottom
		float N_Litter;
		
		// Internal or information variables
		float C2Nlitter;
		float C2Nlitternew;   
		float massbalanceC = 0;
			
		// reference rate constants : major processes will be settling of POC and Litter (the latter at much higher rates), 
		//                 resuspension of POC, leaching of DOC from POC and litter,  respiration of POC and Litter,
		//                 decomp of litter to POC, 
		float K_RespLitter = 0.005; // 1/d      //output
		float K_LeachLitter2DOC_initial = 0.01; // 1/d     //initial
		
		// rate constant modifiers : temperature affects respiration, litter2poc.  Q will affect settling, and resusp
		float Litter_Tref = 20;
		float Litter_Q10 = 2;
		    
		// N mineralization rates Manzoni et al. 2008
		float n;  //fraction of initial N remaining
		float c;  //fraction of initial C remaining
		float rb = 0.1; //decomposer N:C = 0.1 (10C:1N)
		float e;  //decomposer carbon use efficiency; amount of C in new biomass per unit C decomposed
		float rlo; //initial litter N:C ratio - will come from TEM. 
		float C_LitterMax;
		float N_LitterMax;
		
		// outputs
		float Litter2POCSeds;
		float Litter2PONSeds;
		float Litter2DOC;
		float Litter2DON;
		float Litter2CO2;
		float N_LitterRemin;
		float N_LitterUptake;
		
		month              = MFDateGetCurrentMonth();
	    day                = MFDateGetCurrentDay();
		discharge          = MFVarGetFloat (_MPOCDischargeID, itemID, 0.0);
		width              = MFVarGetFloat (_MPOCRiverWidthID, itemID, 0.0);
		sinuosity          = MFVarGetFloat (_MPOCSinuosityID,  itemID, 0.0);
		waterT			   = MFVarGetFloat (_MPOCWTempID, itemID, 0.0);
				
		LitterLocalIn
		LitterC2NLocalIn
		CanopyWidth
		LitterBlowIn
	    C_Litter			   = MFVarGetFloat (_MDBiomass_LitterCID, itemID, 0.0); //kg/m2/d
	    N_Litter			   = MFVarGetFloat (_MDBiomass_LitterNID, itemID, 0.0); //kg/m2/d
        
	    //adjust litter inputs for stream width and canopy width
	    LitterLocalIn_Adjusted
		
		
		
		// set up new cohort of leaf litter (all of previous cohort will be sent to POC_seds
		if (LitterCLocalIn > 0)
			{
			Litter2POCSeds = C_Litter; //transfer all old litter to POC seds pool on the day the new litter enters
			Litter2PONSeds = N_Litter;
			LitterNLocalIn = LitterCLocalIn * 14 / 12 * LitterC2NLocalIn;
		    //initial leaching (does not alter C:N)
			Litter2DOC = LitterCLocalIn - LitterCLocalIn * K_LeachLitter2DOC_initial;
			Litter2DON = DOCfromLitter * 14 / 12 / LitterC2NLocalIn; //asssume leachate has the same C:N
			// inputs to sediment
			C_Litter = LitterCLocalIn - Litter2DOC;  //can accumulate during time, but N:C is assumed constant for Manzoni
			N_Litter = LitterNLocalIn - DONfromLitter;     
		    C_LitterMax = C_Litter;  // assume litter inputs all occur on one day during the year. Following Variable needed for Manzoni equation
		    N_LitterMax = N_Litter;  // assume litter inputs all occur on one day during the year. Following Variable needed for Manzoni equation
		    rlo = N_LitterMax / C_LitterMax;
		    e = 0.43 * pow((rlo / rb), 0.6);
			}
		 Litter2CO2 = C_Litter * K_RespLitter * Q10approach;       
		 C_Litter = C_Litter - Litter2CO2;
		 c = C_Litter / C_LitterMax;

		 //N needed for the given level of C remaining Manzoni et al. 2008
		 // n is the fraction of initial N (N_LitterMax) remaining - can be more or less than current N
		 n = c * rb / rlo + (1 - rb / rlo) * pow(c, (1 / (1 - e)));
		 
		 if (N_Litter > (N_LitterMax * n)){
		     N_LitterRemin = N_Litter - N_LitterMax * n; // N_Litter is stored from previous time step, and updated here 
		     N_LitterUptake = 0;
		 }
		 else {
			 N_LitterRemin = 0;
			 N_LitterUptake = N_LitterMax * n - N_Litter;
		 }
		 
		 N_Litter = N_Litter + N_LitterUptake - N_LitterRemin;
		 
		 
		 
		 
        if (discharge > 0.0){
    	   TotalVol = (discharge) * 86400.0 + waterStorage;  //m3 note:local runoff already included in discharge
    	   TotalArea = MFModelGetLength(itemID) * sinuosity * width;  // How to account for mass balance with changing width	
        }
        else {
           TotalVol = 0.0;
           TotalArea = 0.0;
           width = 0.0;
        }

        // things to output
        C_LitterMax;
        N_LitterMax;
        rlo 
        POC_TotalResusp;
        POC_TotalSettling;
        POC_TotalResp;
        POC_Total2DOC;
        POC_TotalResusp;
        
        
    
			   
			   
		    }
		//if there is no dicharge
		else {
			//POCLocalIn = 0.0;
			//POCTotalIn = POCFlux + POCLocalIn + POCStorage + POC_Remin;
			//POCTotalInMixing = POCFluxMixing + POCLocalIn + POCStorageMixing;
			ConcPre_POC = 0.0;
		    POCFluxNew = 0.0;
		    POCStorageNew = 0.0;
		    POCDeltaStorage = -1 * POCStorage;
		    POC_Denit = 0.0;
		    POC_Assim = 0.0;
		    POC_TotalRemoval = 0.0;
            POCConcentration = 0.0;
		    POCFluxNewMixing = 0.0;
		    POCStorageNewMixing = 0.0;
		    POCConcentrationMixing = 0.0;
		    POCDeltaStorageMixing = -1 * POCStorageNewMixing;
		    POC_Drying = POCTotalIn;
		    POC_DryingMixing = POCTotalInMixing;
		}
		
		
		MFVarSetFloat(_MDLocalIn_POCID,itemID,POCLocalIn); 
		MFVarSetFloat(_MDFlux_POCID,itemID,POCFluxNew); 
		MFVarSetFloat(_MDStorage_POCID,itemID,POCStorageNew); 
		MFVarSetFloat(_MDDeltaStorage_POCID,itemID,POCDeltaStorage); 
		MFVarSetFloat(_MDRemoval_POCID,itemID,POC_TotalRemoval); 
		MFVarSetFloat(_MDDenit_POCID,itemID,POC_Denit); 
		MFVarSetFloat(_MDAssim_POCID,itemID,POC_Assim); 
		MFVarSetFloat(_MDRemin_POCID,itemID,POC_Remin);             
		MFVarSetFloat(_MDBiomass_POCID,itemID,C_BiomassNew); 
		MFVarSetFloat(_MDDeltaBiomass_POCID,itemID,C_DeltaBiomass); 
		MFVarSetFloat(_MDConc_POCID,itemID,POCConcentration);
		MFVarSetFloat(_MDFluxMixing_POCID,itemID,POCFluxNewMixing); 
        MFVarSetFloat(_MDStorageMixing_POCID,itemID,POCStorageNewMixing); 
		MFVarSetFloat(_MDConcMixing_POCID,itemID,POCConcentrationMixing);
		MFVarSetFloat(_MDDeltaStorageMixing_POCID,itemID,POCDeltaStorageMixing); 
		MFVarSetFloat(_MDDrying_POCID, itemID, POC_Drying);
		MFVarSetFloat(_MDDryingMixing_POCID, itemID, POC_DryingMixing);

	//if (isnan(POCConcentration)){
//	if (discharge > 0){
   //	if (POCConcentration > 1){
	//	printf("itemID %d day %d discharge %f TotalVol %f N_Biomass %f runoffVol %f POC_TotalRemoval %f POCFlux %f POCFluxNew %f ConcPre_POC %f POCTotalIn %f POCLocalIn %f POCTotalInMixing %f POCConcentration %f POCConcentrationMixing %f POCStorage %f POC_Remin %f \n", 
    //     		itemID, day, discharge, TotalVol, N_Biomass, runoffVol, POC_TotalRemoval, POCFlux, POCFluxNew, ConcPre_POC, POCTotalIn, POCLocalIn, POCTotalInMixing, POCConcentration, POCConcentrationMixing, POCStorage, POC_Remin);
    // }
    if (POCTotalIn > 0.0){
	massbalance = (POCFlux + POCLocalIn + POC_Remin - (POC_Denit + POC_Assim + POCDeltaStorage + POCFluxNew + POC_Drying)) / POCTotalIn;
    } else {massbalance = 0;}
    if (POCTotalInMixing > 0.0){
	massbalanceMixing = (POCFluxMixing + POCLocalIn - (POCDeltaStorageMixing + POCFluxNewMixing + POC_DryingMixing)) / POCTotalInMixing;
    temp = (POCFluxMixing + POCLocalIn - (POCDeltaStorageMixing + POCFluxNewMixing + POC_DryingMixing));
    temp2 = temp / POCTotalInMixing;
    } else {massbalanceMixing = 0;}
    
    if(!isnan(discharge) && (discharge > 0.0)) {
       //if (itemID == 1245 && ((massbalance > 0.001) || (massbalanceMixing > 0.001))) {
       if ((massbalance > 0.001) || (massbalanceMixing > 0.001)) {

           printf (" MassBalance %f itemID %d Day %d POCTotalIn %f POC_Kremin %f \n", massbalance, itemID, day, POCTotalIn, POC_Kremin);
           printf (" MassBalanceMixing %f itemID %d Day %d POCTotalInMixing %f \n", massbalanceMixing, itemID, day, POCTotalInMixing);
    }
	}    
}

int MDPOCPLUSBIOMASSRoutingDef () {
	int optID = MFUnset;
	const char *optStr;
	const char *options [] = { "none", "calculate", (char *) NULL };
     //const char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_POCID != MFUnset)	return (_MDFlux_POCID);

	MFDefEntering ("POCPLUSBIOMASS Routing Calculation");
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
						CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
						return CMfailed;
					}
		    if (optID==1){
		  //  	printf ("Resoption=%i\n",optID);
		    if (((_MPOCResStorageID              = MFVarGetID (MDVarReservoirStorage,      "km3",     MFInput, MFState, MFInitial))  == CMfailed) ||
		        ((_MPOCResStorageChangeID        = MFVarGetID (MDVarReservoirStorageChange,       "km3/s",     MFInput, MFState, MFBoundary))   == CMfailed))
		    	return CMfailed;
		    }	
		//TODO add parallel for irrigation    
   // Input
	if (((_MPOCDischargeID           = MDDischargeDef    ()) == CMfailed) ||
        ((_MPOCRiverWidthID          = MDRiverWidthDef   ()) == CMfailed) ||
	    ((_MPOCRunoffVolumeID        = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MPOCRunoffID              = MDRunoffDef ()) == CMfailed) ||
	    //((_MPOCSubGridNetworkID      = MDSubGridNetworkDef ()) == CMfailed) ||
        ((_MPOCWTempRiverRouteID     = MDWTempRiverRouteDef ()) == CMfailed) ||
		//((_MPOCRiverLightID          = MDRiverLightDef ()) == CMfailed) ||
        //((_MPOCRiverGPPID            = MDRiverGPPDef ()) == CMfailed) ||
        ((_MPOCWTempID               = MFVarGetID (MDVarWTemp_QxT,                "degC", MFInput, MFState, MFBoundary))  == CMfailed) ||
        ((_MPOCRiverStorageChgID     = MFVarGetID (MDVarRiverStorageChg,        "m3/s",    MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MPOCRiverStorageID        = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput, MFState, MFInitial))   == CMfailed) ||
	    ((_MPOCSinuosityID           = MFVarGetID (MDVarSinuosity,              MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MPOCPointScenarioID       = MFVarGetID (MDVarPointScenario,          MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDNonPoint_POCID          = MFVarGetID (MDVarNonPoint_POC,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDPointSources_POCID      = MFVarGetID (MDVarPointSources_POC,         "kg/day",  MFInput, MFState, MFBoundary))  == CMfailed) ||
//TODO for feedbacks you should use initial	 - could refer to stocks or conditions like mean discharge  
	    // Output
	    ((_MDLocalIn_POCID                = MFVarGetID (MDVarPOCLocalIn,                "kg/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
	    ((_MDRemoval_POCID                = MFVarGetID (MDVarPOCRemoval,                "kg/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
	    ((_MDDenit_POCID                = MFVarGetID (MDVarPOCDenit,                "kg/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDAssim_POCID                = MFVarGetID (MDVarPOCAssim,                "kg/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDRemin_POCID                = MFVarGetID (MDVarPOCRemin,                "kg/d",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDBiomass_POCID                = MFVarGetID (MDVarPOCBiomass,                "kg",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaBiomass_POCID                = MFVarGetID (MDVarPOCDeltaBiomass,                "kg/day",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDConc_POCID                   = MFVarGetID (MDVarPOCConcentration,          "kg/m3",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStorage_POCID                = MFVarGetID (MDVarPOCStorage,                "kg",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDDeltaStorage_POCID           = MFVarGetID (MDVarPOCDeltaStorage,           "kg/day",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDFlux_POCID                  = MFVarGetID (MDVarPOCFlux  ,                  "kg/day",  MFRoute,  MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDConcMixing_POCID            = MFVarGetID (MDVarPOCConcentration_Mixing,    "kg/m3",   MFOutput, MFState, MFBoundary))  == CMfailed) ||
        ((_MDStorageMixing_POCID         = MFVarGetID (MDVarPOCStorage_Mixing,          "kg",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaStorageMixing_POCID    = MFVarGetID (MDVarPOCDeltaStorage_Mixing,     "kg/day",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDDrying_POCID                = MFVarGetID (MDVarPOCDrying,                  "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
        ((_MDDryingMixing_POCID          = MFVarGetID (MDVarPOCDrying_Mixing,           "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||         		
        ((_MDFluxMixing_POCID            = MFVarGetID (MDVarPOCFlux_Mixing ,            "kg/day",  MFRoute,  MFFlux, MFBoundary))  == CMfailed) ||
        (MFModelAddFunction (_MDPOCPLUSBIOMASSRouting) == CMfailed)) return (CMfailed); 

	MFDefLeaving ("POCPLUSBIOMASS Routing Calculation");
	return (_MDFlux_POCID); 
}
