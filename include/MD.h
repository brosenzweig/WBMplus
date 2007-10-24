/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MD.h

balazs.fekete@unh.edu

*******************************************************************************/

#ifndef MD_H_INCLUDED
#define MD_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#define MDNoneStr      "none"
#define MDInputStr     "input"
#define MDLookupStr    "lookup"
#define MDCalculateStr "calculate"
#define MDOnStr        "on"
#define MDOffStr       "off"

// Options
#define MDOptDischarge                       "Discharge"
#define MDOptIrrigation                      "Irrigation"
#define MDOptModel                           "Model"
#define MDOptReservoirs                      "Reservoirs"
#define MDOptRiverbed                        "Riverbed"
#define MDOptRouting                         "Routing"
#define MDOptMuskingum                       "Muskingum"
#define MDOptIrrReferenceET                  "IrrReferenceETP"
#define MDOptIrrSmallReservoirs				 "SmallReservoirs"

#define MDVarAccBalance                      "AccumBalance"
#define MDVarAccPrecipitation                "AccumPrecipitation"
#define MDVarAccEvapotranspiration           "AccumEvapotranspiration"
#define MDVarAccSoilMoistChange              "AccumSoilMoistureChange"
#define MDVarAccGroundWaterChange            "AccumGroundWaterChange"
#define MDVarAccRunoff                       "AccumRunoff"
#define MDVarAccBalance                      "AccumBalance"
#define MDVarAvailWatCap                     "AvailWatCap"
#define MDVarAirTemperature                  "AirTemperature"
#define MDVarAirTempMinimum                  "AirTempMinimum"
#define MDVarAirTempMaximum                  "AirTempMaximum"
#define MDVarAvgNSteps                       "AverageNSteps"

#define MDVarBaseFlow                        "BaseFlow"
#define MDVarBgcTNFlux                       "BgcTNFlux"
#define MDVarBgcTNStoreWater                 "BgcTNStoreWater"         
#define MDVarBgcTNStoreWaterChange           "BgcTNStoreWaterChange" 
#define MDVarBgcTNStoreSeds                  "BgcTNStoreSeds" 
#define MDVarBgcTNStoreSedsChange            "BgcTNStoreSedsChange" 
#define MDVarBgcTNTotalUptake                "BgcTNTotalUptake" 
#define MDVarBgcTNLocalLoad                  "BgcTNLocalLoad"
#define MDVarBgcTNConc                       "BgcTNConc"
#define MDVarBgcNonPointTNSourcesCont        "NonPointTNSourcesCont" 
#define MDVarBgcPointTNSources               "PointTNSources"

#define MDVarCParamAlbedo                    "Albedo"
#define MDVarCParamCHeight                   "CanopyHeight"
#define MDVarCParamLWidth                    "LeafWidth"
#define MDVarCParamRSS                       "CParamRSS"
#define MDVarCParamR5                        "CParamR5"
#define MDVarCParamCD                        "CParamCD"
#define MDVarCParamCR                        "CParamCR"
#define MDVarCParamGLMax                     "CParamGLMax"
#define MDVarCParamLPMax                     "CParamLPMax"
#define MDVarCParamZ0g                       "CParamZ0g"
#define MDVarCloudCover                      "CloudCover"
#define MDVarCorrectedRunoff                 "CorrectedRunoff"

#define MDVarDischarge                       "Discharge"
#define MDVarDischarge0                      "Discharge0"
#define MDVarDischarge1                      "Discharge1"
#define MDVarDischObserved                   "DischargeObserved"
#define MDVarDischMean                       "DischargeMean"

#define MDVarEvapotranspiration              "Evapotranspiration"
#define MDVarGrossRadiance                   "GrossRadiance"
#define MDVarGroundWater                     "GroundWater"
#define MDVarGroundWaterChange               "GroundWaterChange"
#define MDVarLeafAreaIndex                   "LeafAreaIndex"

#define MDVarIrrAreaFraction                 "IrrigatedAreaFraction"
#define MDVarIrrEfficiency                   "IrrigationEfficiency"
#define MDVarIrrEvapotranspiration           "IrrCropEvapotranspiration"
#define MDVarIrrEvapotranspCell              "IrrCropEvapotranspirationCell"
#define MDParIrrDailyPercolationRate         "IrrDailyPercolationRate"
#define MDVarIrrGrossDemand                  "IrrGrossDemand"
#define MDVarIrrGrowingSeason1Start          "GrowingSeason1_Start"
#define MDVarIrrGrowingSeason2Start          "GrowingSeason2_Start"
#define MDVarIrrGrowingSeason3Start          "GrowingSeason3_Start"
#define MDVarIrrIntensity                    "IrrigationIntensity"
#define MDVarIrrNetWaterDemand               "IrrNetWaterDemand"
#define MDVarIrrNumberGrowingSeasons         "NumGrowingSeasons"
#define MDVarIrrPercolationWater             "IrrPercolationWater"
#define MDVarIrrRefEvapotrans                "IrrReferenceETP"
#define MDVarIrrReturnFlow                   "IrrReturnFlow"
#define MDVarIrrSoilMoisture                 "IrrSoilMoisture"
#define MDVarIrrSoilMoistChange              "IrrSoilMoistChange"
#define MDVarIrrUptakeExternal               "__IrrUptakeExternal"
#define MDVarIrrUptakeExcess                 "IrrUptakeExcess"
#define MDVarIrrUptakeGrdWater               "IrrUptakeGroundWater"
#define MDVarIrrUptakeRiver                  "IrrUptakeRiver"       
#define MDVarIrrWaterBalance                 "IrrWaterBalance"

#define	MDVarMeanElevation                   "Elevation"
#define MDVarMuskingumC0                     "MuskingumC0"
#define MDVarMuskingumC1                     "MuskingumC1"
#define MDVarMuskingumC2                     "MuskingumC2"

#define MDVarPrecipitation                   "Precipitation"
#define MDVarPrecipFraction                  "PrecipitationFraction"
#define MDVarPrecipMonthly                   "MonthlyPrecipitation"

#define MDVarRainEvapotranspiration          "RainEvapotranspiration"
#define MDVarRainInfiltration                "RainInfiltration"
#define MDVarRainInterception                "RainInterception"
#define MDVarRainPotEvapotrans               "RainPET"
#define MDVarRainSurfRunoff                  "RainSurfRunoff"
#define MDVarRainSoilMoisture                "RainSoilMoisture"
#define MDVarRainSoilMoistureCell            "RainSoilMoistureCell"
#define MDVarRainSoilMoistChange             "RainSoilMoistureChange"
#define MDVarRainTranspiration               "RainTranspiration"
#define MDVarRainWaterSurplus                "RainWaterSurplus"

#define MDVarReservoirCapacity               "ReservoirCapacity"
#define MDVarReservoirRelease                "ReservoirRelease"
#define MDVarReservoirStorage                "ReservoirStorage" 
#define MDVarReservoirStorageChange          "ReservoirStorageChange"
	
#define MDVarRelHumidity                     "RelativeHumidity"
#define MDVarRelSoilMoisture                 "RelativeSoilMoisture"	
#define MDVarRiverbedAvgDepthMean            "RiverbedAvgDepthMean"
#define MDVarRiverbedCrossArea               "RiverbedCross-sectionArea"
#define MDVarRiverbedSlope                   "RiverbedSlope"
#define MDVarRiverbedShapeExponent           "RiverbedShapeExponent"
#define MDVarRiverbedWidthMean               "RiverbedWidthMean"
#define MDVarRiverbedVelocityMean            "RiverbedVelocityMean"
#define MDVarRiverDepth                      "RiverDepth"
#define MDVarRiverMeanVelocity               "RiverMeanVelocity"
#define MDVarRiverStorage                    "RiverStorage"
#define MDVarRiverStorageChg                 "RiverStorageChange"
#define MDVarRiverWidth                      "RiverbedWidth"

#define MDVarRunoff                          "Runoff"
#define MDVarRunoffCorretion                 "RunoffCorrection"
#define MDVarRunoffVolume                    "RunoffVolume"

#define MDVarSAGEVegCover                    "SAGEVegCover"
#define MDVarSmallResCapacity                "SmallResCapacity"
#define MDVarSmallResRelease                 "SmallResRelease"
#define MDVarSmallResStorage                 "SmallResStorage" 
#define MDVarSmallResStorageChange           "SmallResStorageChange"	
#define MDVarSmallResUptake                  "SmallResUptake"
#define MDVarSnowFall                        "SnowFall"
#define MDVarSnowMelt                        "SnowMelt"
#define MDVarSnowPack                        "SnowPack"
#define MDVarSnowPackChange                  "SnowPackChange"
#define MDVarSoilAvailWaterCap               "AvailableWaterCapacity"
#define MDVarSoilFieldCapacity               "FieldCapacity"
#define MDVarSoilRootingDepth                "RootingDepth"
#define MDVarSoilMoisture                    "SoilMoisture"
#define MDVarSoilMoistChange                 "SoilMoistureChange"
#define MDVarSoilWiltingPoint                "WiltingPoint"

#define MDVarSolarRadiation                  "SolarRadiation"
#define MDVarSRadDayLength                   "DayLength"
#define MDVarSRadI0HDay                      "I0HDay"
#define MDVarStemAreaIndex                   "StemAreaIndex"
#define MDVarSunShine                        "SunShine"
#define MDVarSurfRunoff                      "SurfaceRunoff"
#define MDVarVaporPressure                   "VaporPressure"
#define MDVarTEMVegCover                     "TEMVegCover"
#define MDVarWBMCover                        "WBMCover"
#define MDVarWaterBalance                    "WaterBalance"
#define MDVarWetDays                         "WetDays"
#define MDVarWetDaysAlpha                    "WetDaysAlpha"
#define MDVarWetDaysBeta                     "WetDaysBeta"
#define MDVarWindSpeed                       "WindSpeed"

#define MDParGrossRadTAU                     "GrossRadTAU"
#define MDParGroundWatBETA                   "GroundWaterBETA"
#define MDParInfiltrationFrac                "InfiltrationFraction"

#define MDConst_m3PerSecTOm3PerDay    86400.0
#define MDConst_m3PerSecTOmmKm2PerDay 86400000.0
#define MDConst_mmKm2PerDayTOm3PerSec (1.0 / 86400000.0)
#define MDConstInterceptCI  0.3	   // Interception LAI+SAI constant
#define MDConstInterceptCH 10.0    // Interception canopy height constant 
#define MDConstInterceptD   0.2    // Interception rain fraction of the day

#define MDConstLPC          4.0    // minimum projected LAI for "closed" canopy
#define MDConstC1           0.25   // intercept in actual/potential solar radiation to sunshine duration
#define MDConstC2           0.5    // slope in actual/potential solar radiation to sunshine duration
#define MDConstC3           0.2    // longwave cloud correction coefficient
#define MDConstPTALPHA      1.26   // Priestley-Taylor coefficient
#define MDConstCZS          0.13   // Ratio of roughness to height for smooth closed canopies
#define MDConstCZR          0.05   // ratio of roughness to height for rough closed canopies
#define MDConstHR          10.0    // height above which CZR applies, m
#define MDConstHS           1.0    // height below which CZS applies, m
#define MDConstZMINH        2.0    // reference height above the canopy height m
#define MDConstRSS        500.0    // soil surface resistance, s/m
#define MDConstWNDRAT       0.3    // ratio of nighttime to daytime wind speed
#define MDConstN            2.5    // wind/diffusivity extinction coefficient
#define MDConstCS           0.035  // ratio of projected stem area to height for closed canopy
#define MDConstGLMIN        0.0003 // cuticular leaf surface conductance for all sides of leaf, m/s
#define MDConstRM        1000.0    // maximum solar radiation, at which FR = 1,  W/m2
#define MDConstRHOTP        2.0    // ratio of total leaf surface area to projected leaf area

#define MDConstCPRHO     1240.0    // volumetric heat capacity of air, J/(K m3)
#define MDConstPSGAMMA      0.067  // psychrometric constant, kPa/K
#define MDConstIGRATE       0.0864 // integrates W/m2 over 1 d to MJ/m2
#define MDConstEtoM         0.4085 // converts MJ/m2 to mm of water
#define MDConstSIGMA      5.67E-08 // Stefan-Boltzmann constant, W/(m2/K4)
#define MDConstK            0.4    // von Karman constant

#define MDMinimum(a,b) (((a) < (b)) ? (a) : (b))
#define MDMaximum(a,b) (((a) > (b)) ? (a) : (b))

int MDAccumPrecipDef();
int MDAccumEvapDef();
int MDAccumSMoistChgDef();
int MDAccumGrdWatChgDef();
int MDAccumRunoffDef();
int MDAccumBalanceDef();
int MDAvgNStepsDef ();
int MDBaseFlowDef ();
int MDBgcRoutingDef ();
 
int MDCParamAlbedoDef ();
int MDCParamCHeightDef ();
int MDCParamLWidthDef ();
int MDCParamRSSDef ();
int MDCParamR5Def ();
int MDCParamCDDef ();
int MDCParamCRDef ();
int MDCParamGLMaxDef ();
int MDCParamLPMaxDef ();
int MDCParamZ0gDef ();
int MDDischargeDef ();
int MDDischLevel1Def ();
int MDDischLevel2Def ();
int MDDischLevel3Def ();
int MDDischLevel3AccumulateDef ();
int MDDischLevel3CascadeDef ();
int MDDischLevel3MuskingumDef ();
int MDDischLevel3MuskingumCoeffDef ();
int MDDischMeanDef ();
int MDEvapotranspirationDef ();
int MDGrossRadDef ();
int MDIrrigatedAreaFracDef();
int MDIrrGrossDemandDef();
int MDIrrRefEvapotransDef ();
int MDIrrRefEvapotransFAODef ();
int MDIrrRefEvapotransHamonDef ();
int MDReservoirDef ();
int MDLandCoverDef ();
int MDLeafAreaIndexDef ();
int MDRainInfiltrationDef ();
int MDRainInterceptDef ();
int MDRainPotETDef ();
int MDRainPotETHamonDef ();
int MDRainPotETJensenDef ();
int MDRainPotETPstdDef ();
int MDRainPotETPsTaylorDef ();
int MDRainPotETPMdayDef ();
int MDRainPotETPMdnDef ();
int MDRainPotETSWGdayDef ();
int MDRainPotETSWGdnDef ();
int MDRainPotETTurcDef ();
int MDRainSMoistChgDef ();
int MDRainSurfRunoffDef ();
int MDRainWaterSurplusDef ();

int MDRelHumidityDef ();
int MDRiverbedShapeExponentDef ();
int MDRiverWidthDef ();
//int MDRootDepthDef ();
int MDRunoffDef ();
int MDRunoffVolumeDef ();
int MDSPackChgDef ();
int MDSmallReservoirReleaseDef ();
int MDSmallReservoirCapacityDef ();
int MDSolarRadDef ();
int MDSRadDayLengthDef ();
int MDSRadI0HDayDef ();
int MDSoilAvailWaterCapDef ();
int MDSoilMoistChgDef ();
int MDSurfRunoffDef ();
int MDStemAreaIndexDef ();
int MDWaterBalanceDef ();
int MDPrecipitationDef ();
int MDWetDaysDef ();

//bool MDEvent (int,int,int);

/* PET & Related Functions */

float MDSRadNETLong (float,float,float,float);

float MDPETlibVPressSat   (float);
float MDPETlibVPressDelta (float);

float MDWindAdjustment (float,float,float,float, float, float);

float MDPETlibLeafAreaIndex (float,float);
float MDPETlibSteamAreaIndex (float,float);
float MDPETlibRoughnessClosed (float,float);
float MDPETlibRoughness (float,float,float,float,float);
float MDPETlibZPDisplacement (float,float,float,float);
float MDPETlibCanopySurfResistance (float,float,float,float,float,float,float,float,float);
float MDPETlibBoundaryResistance (float,float,float,float,float,float,float);
float MDPETlibLeafResistance (float,float, float,float,float, float,float, float);
float MDPETlibGroundResistance (float,float,float,float,float,float,float);

float MDPETlibPenmanMontieth (float,float,float,float,float);
float MDPETlibShuttleworthWallace (float,float,float,float,float,float,float,float,float);

float MDPETJensen (float,float);
float MDPETMcNaughtonBlack (float,float);
float MDPETPenmanStd (float,float,float,float,float,float,float,float);
float MDPETPenmanMonteithDN (float,float,
									float,float,float,float,float,float,float,
									float,float,float,float,float,float,float,
									float,float);
float MDPETPenmanMonteith (float,float,
									float,float,float,float,float,
									float,float,float,float,float, float,float,
									float,float);
float MDPETShuttleworthWallaceDN (float,float,
									float,float,float,float,float,float,float,
									float,float,float,float,float,float,float,float,float,
									float,float);
float MDPETPriestleyTaylor (float,float,float,float,float,float,float);
float MDPETThornthwaite (float,float,float);
float MDPETTurc (float,float);

#if defined(__cplusplus)
}
#endif

#endif /* MD_H_INCLUDED */
