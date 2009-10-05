/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDWTempRiverRoute.c

wil.wollheim@unh.edu

Route temperature through river network

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Model
static int _MDWTempRiverRouteID        = MFUnset;

// Input
static int _MDInDischargeID            = MFUnset;
static int _MDInDischargeIncomingID    = MFUnset;
static int _MDInRunoffVolumeID         = MFUnset;
static int _MDInWTempRiverID           = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInRiverStorageChgID      = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
static int _MDInSolarRadID             = MFUnset;
static int _MDInWindSpeedID            = MFUnset;
static int _MDInCloudCoverID           = MFUnset;
static int _MDInAirTemperatureID       = MFUnset;
static int _MDInSnowPackID             = MFUnset;
static int _MDInResStorageChangeID     = MFUnset;
static int _MDInResStorageID           = MFUnset;
// Output
static int _MDLocalIn_QxTID            = MFUnset;
static int _MDRemoval_QxTID            = MFUnset;
static int _MDFlux_QxTID               = MFUnset;
static int _MDStorage_QxTID            = MFUnset;
static int _MDDeltaStorage_QxTID       = MFUnset;
static int _MDWTemp_QxTID              = MFUnset;
static int _MDWTempDeltaT_QxTID        = MFUnset;
static int _MDFluxMixing_QxTID         = MFUnset;
static int _MDStorageMixing_QxTID      = MFUnset;
static int _MDDeltaStorageMixing_QxTID = MFUnset;
static int _MDWTempMixing_QxTID        = MFUnset;


static void _MDWTempRiverRoute (int itemID) {
	 float Q;
	 float Q_incoming;
     float RO_Vol;
	 float RO_WTemp;
	 float QxT_input;
	 float QxT;
	 float QxTnew = 0;
	 float QxTout = 0;
     float Q_WTemp;
     float Q_WTemp_new;
     float StorexT;
     float StorexT_new;
     float DeltaStorexT;
     float SnowPack;

     //processing variables
     float channelWidth;
     float waterStorageChange;
     float waterStorage;
     float ResWaterStorageChange = 0;
     float ResWaterStorage = 0;
     float solarRad;
     float windSpeed;
     float cloudCover;
     float Tair;
     float Tequil = 0;
     float HeatLoss_int = 4396.14; // is intercept assuming no wind and clouds
     float HeatLoss_slope = 1465.38; // is slope assuming no wind and clouds
     float deltaT;
     
     float ReservoirArea;
     float ReservoirDepth;
     float ReservoirVelocity;

     // conservative mixing variables (parallel to those above_
     float QxT_mix;
     float QxTnew_mix = 0;
     float QxTout_mix = 0;
     float Q_WTemp_mix;
     float StorexT_mix;  
     float StorexT_new_mix;  
     float DeltaStorexT_mix; 
     float QxTRemoval;
     int day;
     int month;
     
     day = MFDateGetCurrentDay();
     month = MFDateGetCurrentMonth();

   	 Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
   	 Q_incoming            = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0); // already includes local runoff
     RO_Vol                = MFVarGetFloat (_MDInRunoffVolumeID,      itemID, 0.0);
   	 RO_WTemp              = MFVarGetFloat (_MDInWTempRiverID,        itemID, 0.0);
     SnowPack              = MFVarGetFloat (_MDInSnowPackID,          itemID, 0.0);
 	
     if (_MDInResStorageID != MFUnset){
     ResWaterStorageChange = MFVarGetFloat ( _MDInResStorageChangeID, itemID, 0.0) * pow(1000,3) / 86400.0; // convert to m3/
     ResWaterStorage       = MFVarGetFloat ( _MDInResStorageID,       itemID, 0.0) * pow(1000,3); // convert to m3 
     }
     
     waterStorageChange    = MFVarGetFloat ( _MDInRiverStorageChgID,  itemID, 0.0);
   	 waterStorage          = MFVarGetFloat ( _MDInRiverStorageID,     itemID, 0.0);
   	 channelWidth          = MFVarGetFloat ( _MDInRiverWidthID,       itemID, 0.0);
 	 solarRad              = MFVarGetFloat ( _MDInSolarRadID,         itemID, 0.0); //MJ/m2/d - CHECK UNITS
 	 windSpeed             = MFVarGetFloat ( _MDInWindSpeedID,        itemID, 0.0);
     cloudCover            = MFVarGetFloat ( _MDInCloudCoverID,       itemID, 0.0);
     Tair                  = MFVarGetFloat ( _MDInAirTemperatureID,   itemID, 0.0);
        	 
     QxT                   = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);
     StorexT               = MFVarGetFloat (_MDStorage_QxTID,         itemID, 0.0);
     QxT_mix               = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);
     StorexT_mix           = MFVarGetFloat (_MDStorageMixing_QxTID,   itemID, 0.0);
    // if (itemID == 5132){
    //        	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
    //   	 }
     //TODO: combine with reservoir check above - also make reservoir hydraulics generally accessible
     //TODO: add effect of water withdrawals
     if(ResWaterStorage > 0.0){
     waterStorage = waterStorage + ResWaterStorage;
     waterStorageChange = waterStorageChange + ResWaterStorageChange;
     ReservoirArea = pow(((ResWaterStorage / pow(10,6)) / 9.208),(1 / 1.114)) * 1000 * 1000;  // m2, from Takeuchi 1997 - original equation has V in 10^6 m3 and A in km2
     ReservoirDepth = (ResWaterStorage / ReservoirArea); //m
     ReservoirVelocity = Q / (ReservoirArea); // m/s
     channelWidth = MDMaximum(channelWidth, (Q / (ReservoirDepth * ReservoirVelocity))); // m
     }
     else{
    	 ReservoirArea = 0.0;
    	 ReservoirVelocity = 0.0;
    	 ReservoirDepth = 0.0;
     }
    
     //TODO: RO_Vol has been set to never be less than 0 in MDWRunoff
     QxT_input = RO_Vol * RO_WTemp * 86400.0; //m3*degC/d 

     	//if (itemID == 188 && month == 5 && day == 1){
        //  		printf("Stop: Q %f RO_Vol %f QxT %f QxT_input %f \n", Q, RO_Vol, QxT, QxT_input);
        //}
     //note: calculation for input concentration is changed from previous iterations 
     // to use incoming Q.  Also use WaterStorage from previous time step/
     // TODO: Need to include a variable that accounts for losses due to discharge disappearing (Drying)
     // TODO:  Make all these changes for other bgc flux models
     // Q_incoming includes local runoff!!!
     if((Q_incoming) > 0.0) {		//do not include water storage in this check - will screw up mixing estimates
         QxTnew = QxT + QxT_input + StorexT; //m3*degC/d
   	     QxTnew_mix = QxT_mix + QxT_input + StorexT_mix;
        
   	     Q_WTemp = QxTnew / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC
         Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC

        ///Temperature Processing using Dingman 1972 
         if (cloudCover < 95){  // clear skies, assume cloud cover < 95% convertcalories / cm2 /d to kJ/m2/d
            HeatLoss_int = (105 + 23 *  windSpeed) * 4.1868 / 1000 * 100 * 100; // kJ/m2/d
            HeatLoss_slope = (35 + 4.2 * windSpeed) * 4.1868 / 1000 * 100 * 100;// kJ/m2/d/degC

         } else{                // cloudy skies, assume cloud cover > 95%
        	 HeatLoss_int = (-73 + 9.1 *  windSpeed) * 4.1868 / 1000 * 100 * 100;
        	 HeatLoss_slope = (37 + 4.6 * windSpeed) * 4.1868 / 1000 * 100 * 100;
         }
         Tequil = Tair + (((solarRad * 1000) - HeatLoss_int) / HeatLoss_slope); //solar rad converted from MJ to kJ/m2/d

          // use exponential form 
         //TODO channelWidth can equal 0 when waterStorage > 0.0, so need to check here
         // Apply model only to large enough discharges, otherwise assume temperature equils equilibrium
        // if (channelWidth > 0 && Q > 0.001){
         if (channelWidth > 0){
	 Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * MFModelGetLength(itemID)) / (999.73 * 4.1922 * (Q * 86400.0 / channelWidth)))) + Tequil));
         }
         else {
        	 Q_WTemp_new = MDMaximum(0, Tequil);
         }
         
         //if cell has reservoir, assume reservoir exchange dominates
         //if(ResWaterStorage > 0){
         //    Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * ReservoirArea) / (999.73 * 4.1922 * (Q * 86400.0)))) + Tequil));
         //}

         
         deltaT = Q_WTemp_new - Q_WTemp;
         //if (Q_WTemp_new > 50){
       //  if (Q_WTemp_mix > 30){
       //  printf("Toggle");
       //  printf("Stop WaterTemp > 50 itemID %d XCoord %f YCoord %f month %d day %d Q %f Q_incoming %f waterStorage %f "
       //     		"RO_Vol %f RO_WTemp %f QxT %f QxT_mix %f StorexT %f Storext_mix %f QxT_input %f QxTnew %f Q_WTemp %f Q_WTemp_mix %f Q_WTemp_new %f Tequil %f \n", 
       //     		 itemID, MFModelGetXCoord(itemID),MFModelGetYCoord(itemID), month, day, Q, Q_incoming, waterStorage, 
       //     		 RO_Vol, RO_WTemp, QxT, QxT_mix, StorexT, StorexT_mix, QxT_input, QxTnew, Q_WTemp, Q_WTemp_mix, Q_WTemp_new, Tequil);
       //  }
   	     StorexT_new  = waterStorage * Q_WTemp_new; //m3*degC
         DeltaStorexT = StorexT_new - StorexT; //
         QxTout       = Q * 86400.0 * Q_WTemp_new ; //m3*degC/d
         QxTRemoval   = QxTnew - (StorexT_new + QxTout); //m3*degC/d
         StorexT_new_mix  = waterStorage * Q_WTemp_mix; //m3*degC
         DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
         QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; //m3*degC/s
         MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);
         MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
         MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
         MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
         MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
         MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);
         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);
         MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
         MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
         MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
         MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);
   	     }
         else{
        	 if (waterStorage > 0){
                 QxTnew = QxT_input + StorexT; //m3*degC
                 QxTnew_mix = QxT_input + StorexT_mix;
        	 }
        	 else{
        		 QxTnew = 0; 
        		 QxTnew_mix = 0;
             }
        	 StorexT_new  = 0.0; //m3*degC
        	 DeltaStorexT = StorexT_new - StorexT; //
        	 QxTout       = 0.0; //m3*degC/dStorexT_new_mix  = 0; //m3*degC
             QxTRemoval   = 0.0; //m3*degC/d
             StorexT_new_mix  = 0.0; //m3*degC
        	 DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
        	 QxTout_mix       = 0.0; //m3*degC/s
             
             MFVarSetFloat(_MDLocalIn_QxTID, itemID, 0.0);
             MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
        	 MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
        	 MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
        	 MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
           	 MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
        	 MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
        	 MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
        
        	 MFVarSetMissingVal(_MDWTemp_QxTID, itemID);
             MFVarSetMissingVal(_MDWTempDeltaT_QxTID, itemID);
             MFVarSetMissingVal(_MDWTempMixing_QxTID, itemID);
         }
  	float mb;
  	float mbmix;
  	mb = QxT_input + QxT - QxTRemoval - QxTout - DeltaStorexT;
  	mbmix = (QxT_input + QxT_mix - QxTout_mix - DeltaStorexT_mix);
    //if (mbmix > 100000){
  	//printf("mass balance = mb %f mbmix %f \n", mb, mbmix);
    //}
 	  
}

int MDWTempRiverRouteDef () {
	int optID = MFUnset;
    const char *optStr;
		const char *options [] = { "none", "calculate", (char *) NULL };

	if (_MDWTempRiverRouteID != MFUnset) return (_MDWTempRiverRouteID);

	MFDefEntering ("Route river temperature");
	
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
				CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
				return CMfailed;
			}
    if (optID==1){
  //  	printf ("Resoption=%i\n",optID);
    if (((_MDInResStorageID           = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))  == CMfailed) ||
        ((_MDInResStorageChangeID     = MFVarGetID (MDVarReservoirStorageChange, "km3/s",   MFInput, MFState, MFBoundary)) == CMfailed))
    	return CMfailed;
    }
	
		
		//input
	if (((_MDInDischargeID            = MDDischargeDef    ()) == CMfailed) ||
	    ((_MDInSolarRadID             = MDSolarRadDef     ()) == CMfailed) ||
       
        ((_MDInWTempRiverID           = MDWTempRiverDef   ()) == CMfailed) ||
        ((_MDInRiverWidthID           = MDRiverWidthDef   ()) == CMfailed) ||
        ((_MDInRunoffVolumeID         = MDRunoffVolumeDef ()) == CMfailed) ||
        ((_MDInDischargeIncomingID    = MFVarGetID (MDVarDischarge0,            "m3/s",       MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDInWindSpeedID            = MFVarGetID (MDVarWindSpeed,             "m/s",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInAirTemperatureID       = MFVarGetID (MDVarAirTemperature,        "degC",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCloudCoverID           = MFVarGetID (MDVarCloudCover,            "%",          MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,       "m3/s",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,          "m3",         MFInput,  MFState, MFInitial))  == CMfailed) ||
        ((_MDInSnowPackID             = MFVarGetID (MDVarSnowPack,               "mm",        MFInput,  MFState, MFBoundary)) == CMfailed) ||

        // output
        ((_MDLocalIn_QxTID            = MFVarGetID (MDVarBgcLocalIn_QxT,         "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDRemoval_QxTID            = MFVarGetID (MDVarRemoval_QxT,            "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFlux_QxTID               = MFVarGetID (MDVarFlux_QxT,               "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDStorage_QxTID            = MFVarGetID (MDVarStorage_QxT,            "m3*degC",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaStorage_QxTID       = MFVarGetID (MDVarDeltaStorage_QxT,       "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDWTemp_QxTID              = MFVarGetID (MDVarWTemp_QxT,              "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDWTempDeltaT_QxTID        = MFVarGetID (MDVarWTempDeltaT_QxT,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        
        //output mixing
        ((_MDFluxMixing_QxTID         = MFVarGetID (MDVarFluxMixing_QxT,         "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDStorageMixing_QxTID      = MFVarGetID (MDVarStorageMixing_QxT,      "m3*degC",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaStorageMixing_QxTID = MFVarGetID (MDVarDeltaStorageMixing_QxT, "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDWTempMixing_QxTID        = MFVarGetID (MDVarWTempMixing_QxT,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
                
           
       (MFModelAddFunction (_MDWTempRiverRoute) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Route river temperature");
	   return (_MDWTemp_QxTID);
}
