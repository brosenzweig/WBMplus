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

 //MD BGCInput
# define MDVarNonPointTNSourcesCont          "NonPointTNSourcesCont" 
# define MDVarPointTNSources                 "PointTNSources"

 //MD BGCOutput
# define MDVarBgcTNFlux                      "BgcTNFlux"
# define MDVarBgcTNStoreWater                "BgcTNStoreWater"         
# define MDVarBgcTNStoreWaterChange          "BgcTNStoreWaterChange" 
# define MDVarBgcTNStoreSeds                 "BgcTNStoreSeds" 
# define MDVarBgcTNStoreSedsChange           "BgcTNStoreSedsChange" 
# define MDVarBgcTNTotalUptake               "BgcTNTotalUptake" 
# define MDVarBgcTNLocalLoad                 "BgcTNLocalLoad"
# define MDVarBgcTNConc                      "BgcTNConc"

#define MDConst_m3PerSecTOm3PerDay 86400.0

//MD IrrigationInput
#define MDIrrConstantKc                             "IrrConstantKc"
#define MDVarIrrEfficiency                          "IrrigationEfficiency"
#define MDVarIrrIntensity                           "IrrigationIntensity"
#define MDVarIrrGrowingSeason1Start                 "GrowingSeason1_Start"
#define MDVarIrrGrowingSeason2Start                 "GrowingSeason2_Start"
#define MDVarIrrGrowingSeason3Start                 "GrowingSeason3_Start"
#define MDVarNumberGrowingSeasons                   "NumGrowingSeasons"

#define MDVarReservoirCapacity				"ReservoirCapacity"
#define MDVarReservoirStorage		"ReservoirStorage" 
#define MDVarReservoirStorageChange "ReservoirStorageChange"
#define MDVarReservoirRelease	"ReservoirRelease"
//MD IrrigationOutput
#define MDVarIrrNetIrrigationWaterDemand			"IrrNetWaterDemand"
#define MDVarIrrCropETP								"IrrCropEvapotranspiration"
#define MDVarIrrGrossIrrigationWaterDemand			"IrrGrossWaterDemand"
#define MDVarIrrAreaFraction                        "IrrigatedAreaFraction"
#define MDVarCombinedEvapotranspiration				"CombinedEvapotranspiration"
//MD Runoff Output	
#define MDVarWaterBalance                           "WaterBalance"

#define MDVarReferenceEvapotranspiration            "ReferenceETP"
#define MDVarIrrPercolationWater					"IrrPercolationWater"

#define MDParIrrDailyPercolationRate 				"IrrDailyPercolationRate"
 
#define MDVarIrrSoilMoistureChange                  "IrrSoilMoistChange"

#define MDVarIrrGrossDemand               "IrrGrossDemand"      // IrrGrossDemand = IrrUptakeGroundWater + IrrUptakeRiver + IrrUptakeExcess
#define MDVarIrrReturnFlow                "IrrReturnFlow"
#define MDVarIrrUptakeExternal            "__IrrUptakeExternal" // __IrrUptakeExternal = IrrUptakeRiver + IrrUptakeExcess
#define MDVarIrrUptakeExcess              "IrrUptakeExcess"
#define MDVarIrrUptakeGrdWater            "IrrUptakeGroundWater"
#define MDVarIrrUptakeRiver               "IrrUptakeRiver"       

#define MDVarCorrectedRunoff              "CorrectedRunoff"
#define MDVarCrop_1                       "CropFraction_1"
#define MDVarCrop_2                       "CropFraction_2"
#define MDVarCrop_3                       "CropFraction_3"
#define MDVarCrop_4                       "CropFraction_4"
#define MDVarCrop_5                       "CropFraction_5"
#define MDVarCrop_6                       "CropFraction_6"
#define MDVarCrop_7                       "CropFraction_7"
#define MDVarCrop_8                       "CropFraction_8"
#define MDVarCrop_9                       "CropFraction_9"
#define MDVarCrop_10								"CropFraction_10"
#define MDVarCrop_11								"CropFraction_11"
#define MDVarCrop_12								"CropFraction_12"
#define MDVarCrop_13								"CropFraction_13"
#define MDVarCrop_14								"CropFraction_14"
#define MDVarCrop_15								"CropFraction_15"
#define MDVarCrop_16								"CropFraction_16"
#define MDVarCrop_17								"CropFraction_17"
#define MDVarCrop_18								"CropFraction_18"
#define MDVarCrop_19								"CropFraction_19"
//=================================================================

#define MDVarStartGrowingSeasonCalc	"StartGrowingSeasonCalc"
#define MDVarEndGrowingSeasonCalc	"EndGrowingSeasonCalc"
#define MDVarIsGrowingSeasonCalc	"IsGrowingSeasonCalc"

#define MDVarCropSMDef_1                           "CropSMDef_1"
#define MDVarCropSMDef_2                           "CropSMDef_2"
#define MDVarCropSMDef_3                           "CropSMDef_3"
#define MDVarCropSMDef_4                           "CropSMDef_4"
#define MDVarCropSMDef_5                           "CropSMDef_5"
#define MDVarCropSMDef_6                           "CropSMDef_6"
#define MDVarCropSMDef_7                           "CropSMDef_7"
#define MDVarCropSMDef_8                           "CropSMDef_8"
#define MDVarCropSMDef_9                           "CropSMDef_9"
#define MDVarCropSMDef_10                          "CropSMDef_10"
#define MDVarCropSMDef_11                          "CropSMDef_11"
#define MDVarCropSMDef_12                          "CropSMDef_12"
#define MDVarCropSMDef_13                          "CropSMDef_13"
#define MDVarCropSMDef_14                          "CropSMDef_14"
#define MDVarCropSMDef_15                          "CropSMDef_15"
#define MDVarCropSMDef_16                          "CropSMDef_16"
#define MDVarCropSMDef_17                          "CropSMDef_17"
#define MDVarCropSMDef_18                          "CropSMDef_18"
#define MDVarCropSMDef_19                          "CropSMDef_19"

#define MDVarAccBalance                            "AccumBalance"
#define MDVarAccPrecipitation                      "AccumPrecipitation"
#define MDVarAccEvapotranspiration                 "AccumEvapotranspiration"
#define MDVarAccSoilMoistChange                    "AccumSoilMoistureChange"
#define MDVarAccGroundWaterChange                  "AccumGroundWaterChange"
#define MDVarAccRunoff                             "AccumRunoff"
#define MDVarAccBalance                            "AccumBalance"
#define MDVarAvailWatCap                           "AvailWatCap"
#define MDVarAirTemperature                        "AirTemperature"
#define MDVarAirTempMinimum                        "AirTempMinimum"
#define MDVarAirTempMaximum                        "AirTempMaximum"
#define MDVarAvgNSteps                             "AverageNSteps"

#define MDVarCParamAlbedo                          "Albedo"
#define MDVarCParamCHeight                         "CanopyHeight"
#define MDVarCParamLWidth                          "LeafWidth"
#define MDVarCParamRSS                             "CParamRSS"
#define MDVarCParamR5                              "CParamR5"
#define MDVarCParamCD                              "CParamCD"
#define MDVarCParamCR                              "CParamCR"
#define MDVarCParamGLMax                           "CParamGLMax"
#define MDVarCParamLPMax                           "CParamLPMax"
#define MDVarCParamZ0g                             "CParamZ0g"

#define MDVarCloudCover                            "CloudCover"
#define MDVarBaseFlow                              "BaseFlow"

#define MDVarDischarge                             "Discharge"
#define MDVarDischarge0                            "Discharge0"
#define MDVarDischarge1                            "Discharge1"
#define MDVarDischLevel1                           "__DischargeLevel1"
#define MDVarDischLevel2                           "__DischargeLevel2"
#define MDVarDischLevel3                           "__DischargeLevel3"
#define MDVarDischObserved                         "DischargeObserved"
#define MDVarDischMean	                           "DischargeMean"

#define MDVarEvapotranspiration                    "Evapotranspiration"
#define MDVarGrossRadiance                         "GrossRadiance"
#define MDVarGroundWater                           "GroundWater"
#define MDVarGroundWaterChange                     "GroundWaterChange"
#define MDVarLeafAreaIndex                         "LeafAreaIndex"
#define MDVarInfiltration                          "Infiltration"
#define MDVarInterception                          "Interception"
#define MDVarIsotope                               "Isotope"
#define MDVarIsotopeET                             "IsotopeET"
#define MDVarIsotopeSnow                           "IsotopeSnow"
#define MDVarIsotopeSoil                           "IsotopeSoil"
#define MDVarIsotopeRunoff                         "IsotopeRunoff"
#define MDVarMuskingumC0                           "MuskingumC0"
#define MDVarMuskingumC1                           "MuskingumC1"
#define MDVarMuskingumC2                           "MuskingumC2"
#define MDVarPotEvapotrans                         "PET"

#define MDVarPrecipitation                         "Precipitation"
#define MDVarPrecipFraction                        "PrecipitationFraction"
#define MDVarPrecipMonthly                         "MonthlyPrecipitation"
#define MDVarRelHumidity                           "RelativeHumidity"
#define MDVarRiverbedAvgDepthMean	               "RiverbedAvgDepthMean"
#define MDVarRiverbedCrossArea	                   "RiverbedCross-sectionArea"
#define MDVarRiverbedSlope	                       "RiverbedSlope"
#define MDVarRiverbedShapeExponent	               "RiverbedShapeExponent"
#define MDVarRiverbedWidthMean	                   "RiverbedWidthMean"
#define MDVarRiverbedVelocityMean                  "RiverbedVelocityMean"
#define MDVarRiverDepth          	               "RiverDepth"
#define MDVarRiverStorage                          "RiverStorage"
#define MDVarRiverStorageChg                       "RiverStorageChange"
#define MDVarRiverWidth          	               "RiverbedWidth"

#define MDVarRootingDepth	                       "RootingDepth"
#define MDVarRunoff                                "Runoff"
#define MDVarRunoffCorretion                       "RunoffCorrection"
#define MDVarRunoffVolume                          "RunoffVolume"
#define MDVarSAGEVegCover                          "SAGEVegCover"
#define MDVarSnowPack                              "SnowPack"
#define MDVarSnowPackChange                        "SnowPackChange"
#define MDVarSoilMoisture                          "SoilMoisture"
#define MDVarSoilMoistChange                       "SoilMoistureChange"
#define MDVarRiverMeanVelocity                     "RiverMeanVelocity"

//#define MDVarSoilPorosity                          "SoilPorosity"
//#define MDVarSoilTexture                           "SoilTexture"

#define MDVarSolarRadiation                        "SolarRadiation"
#define MDVarSRadDayLength                         "DayLength"
#define MDVarSRadI0HDay                            "I0HDay"
#define MDVarStemAreaIndex                         "StemAreaIndex"
#define MDVarSunShine                              "SunShine"
#define MDVarSurfaceRO                             "SurfaceRunoff"
#define MDVarTranspiration                         "Transpiration"
#define MDVarVaporPressure                         "VaporPressure"
#define MDVarTEMVegCover                           "TEMVegCover"
#define MDVarWBMCover                              "WBMCover"
#define MDVarWetDays                               "WetDays"
#define MDVarWetDaysAlpha                          "WetDaysAlpha"
#define MDVarWetDaysBeta                           "WetDaysBeta"
#define MDVarWiltingPoint                          "WiltingPoint"
#define MDVarFieldCapacity                         "FieldCapacity"
#define MDVarWaterSurplus                          "WaterSurplus"
#define MDVarWindSpeed                             "WindSpeed"

#define MDVarSnowMelt				"SnowMelt"
#define MDVarPrecipitationOUT				"PrecipitationOut"

#define MDParGrossRadTAU                           "GrossRadTAU"
#define MDParGroundWatBETA                         "GroundWaterBETA"
#define MDParInfiltrationFrac                      "InfiltrationFraction"
#define MDParIsotopeC0                             "IsotopeC0"
#define MDParIsotopeC1                             "IsotopeC1"
#define MDParIsotopeC2                             "IsotopeC2"
#define MDParIsotopeVSMOW                          "IsotopeVSMOW"
#define MDParIsotopeRhoRatio                       "IsotopeRhoRatio"

#define MDConst_m3PerSecTOmmKm2PerDay 86400000.0
#define MDConst_mmKm2PerDayTOm3PerSec (1.0 / 86400000.0)
#define MDConstInterceptCI  0.3	/* Interception LAI+SAI constant */
#define MDConstInterceptCH 10.0  /* Interception canopy height constant */ 
#define MDConstInterceptD   0.2  /* Interception rain fraction of the day */

#define MDConstLPC 4.0        /* minimum projected LAI for "closed" canopy */
#define MDConstC1 0.25        /* intercept in actual/potential solar radiation to sunshine duration */
#define MDConstC2 0.5         /* slope in actual/potential solar radiation to sunshine duration */
#define MDConstC3 0.2         /* longwave cloud correction coefficient */
#define MDConstPTALPHA 1.26   /* Priestley-Taylor coefficient */
#define MDConstCZS 0.13       /* ratio of roughness to height for smooth closed canopies */
#define MDConstCZR 0.05       /* ratio of roughness to height for rough closed canopies */
#define MDConstHR 10.0        /* height above which CZR applies, m */
#define MDConstHS 1.0         /* height below which CZS applies, m */
#define MDConstZMINH 2.0      /* reference height above the canopy height m */
#define MDConstRSS 500.0      /* soil surface resistance, s/m */
#define MDConstWNDRAT 0.3     /* ratio of nighttime to daytime wind speed */
#define MDConstN 2.5          /* wind/diffusivity extinction coefficient */
#define MDConstCS 0.035       /* ratio of projected stem area to height for closed canopy */
#define MDConstGLMIN 0.0003   /* cuticular leaf surface conductance for all sides of leaf, m/s */
#define MDConstRM 1000.0      /* maximum solar radiation, at which FR = 1,  W/m2 */
#define MDConstRHOTP 2.0      /* ratio of total leaf surface area to projected leaf area */

#define MDConstCPRHO 1240     /* volumetric heat capacity of air, J/(K m3) */
#define MDConstPSGAMMA 0.067  /* psychrometric constant, kPa/K */
#define MDConstIGRATE 0.0864  /* integrates W/m2 over 1 d to MJ/m2 */
#define MDConstEtoM 0.4085    /* converts MJ/m2 to mm of water */
#define MDConstSIGMA 5.67E-08 /* Stefan-Boltzmann constant, W/(m2/K4) */
#define MDConstK 0.4          /* von Karman constant */

#define MDMinimum(a,b) (((a) < (b)) ? (a) : (b))
#define MDMaximum(a,b) (((a) > (b)) ? (a) : (b))

#define DWDebug 1;


//IrrigationStuff+++++++++++++++++++++++++++=
 
int MDParNumberOfCrops;
//+++++++++++++++++++++++++++++++++++=
int MDIrrigationDef();
int MDIrrGrossDemandDef();
int MDIrrReferenceETPDef ();
int MDIrrFAOReferenceETPDef ();
int MDIrrHamonReferenceETPDef ();
int MDReservoirDef ();

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
int MDGrossRadDef ();
int MDLandCoverDef ();
int MDLeafAreaIndexDef ();
int MDInfiltrationDef ();
int MDInterceptDef ();
int MDPotETDef ();
int MDPotETHamonDef ();
int MDPotETJensenDef ();
int MDPotETPstdDef ();
int MDPotETPsTaylorDef ();
int MDPotETPMdayDef ();
int MDPotETPMdnDef ();
int MDPotETSWGdayDef ();
int MDPotETSWGdnDef ();
int MDPotETTurcDef ();
int MDPotFAOReferenceETDef ();
int MDRelHumidityDef ();
int MDRiverbedShapeExponentDef ();
int MDRiverWidthDef ();
//int MDRootDepthDef ();
int MDRunoffDef ();
int MDRunoffVolumeDef ();
int MDSMoistChgDef ();
int MDSPackChgDef ();
int MDSolarRadDef ();
int MDSRadDayLengthDef ();
int MDSRadI0HDayDef ();
int MDSoilPropDef ();
int MDStemAreaIndexDef ();
int MDWaterBalanceDef ();
int MDWaterSurplusDef ();
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
