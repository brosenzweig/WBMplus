/*******************************************************************************
 * Copyright (c) 2005, 2007 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
#include<stdio.h>
#include<math.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>
//Water In;

static int _MDInPrecipID;
static int _MDInEvaptrsID;
static int _MDInSnowPackChgID;
static int _MDInSoilMoistChgID;
static int _MDInIrrSoilMoistChgID;
static int _MDInGrdWatChgID;
static int _MDInRunoffID;
static int _MDInActIrrAreaID;
static int _MDInCropEtpID;
static int _MDInDischargeAbstractionID;

static int _MDInInfiltrationID;
static int _MDInGrdWaterAbstractionID;
static int _MDInGrossIrrDemandID;
static int _MDInIrrigationDrainageID;
static int _MDInExcessAbstractionID;
static int _MDDischarge;
static int _MDBgcRoutingID;
//Output
static int _MDOutWaterBalanceID;
static int _MDOutPrecipID;
static int  _MDDischarge;
static int _MDInAirTemperatureID;
static int _MDOutTotalEvapotranpirationID;
static int _MDIsGrowingSeasonID;
static int _MDInRefETPID;
float etpThreshold=0.5;
static void _MDWaterBalance(int itemID) {

	if ( MFVarTestMissingVal (_MDInActIrrAreaID, itemID)) 
	{
		printf("IrrArea set to zero!\n");
		MFVarSetFloat(_MDInActIrrAreaID,itemID, 0.0);
	}
	if	(MFVarTestMissingVal (_MDInCropEtpID,         	itemID)){
		 MFVarSetFloat(_MDInCropEtpID,itemID, 0.0);
		printf("CropETP missing??, ID=%i \n",_MDInCropEtpID);
	}
	if (  MFVarTestMissingVal (_MDInDischargeAbstractionID,      itemID))MFVarSetFloat(_MDInDischargeAbstractionID,itemID, 0.0);
	if ((MFVarTestMissingVal (_MDInPrecipID,              itemID) ) ||
	 (  MFVarTestMissingVal (_MDInEvaptrsID, 		itemID))||
	 (  MFVarTestMissingVal (_MDInSnowPackChgID,         itemID))||
  	(  MFVarTestMissingVal (_MDInSoilMoistChgID,         itemID))||
  	(  MFVarTestMissingVal (_MDInRunoffID,        		 itemID))||
        (  MFVarTestMissingVal (_MDInIrrSoilMoistChgID,         itemID))||
	( MFVarTestMissingVal (_MDInGrdWatChgID,         	itemID))||
	(MFVarTestMissingVal(_MDInInfiltrationID,itemID)))
	{
//	printf ("Missing Variables in MDWaterBalance!\n");
	MFVarSetMissingVal(_MDOutWaterBalanceID,itemID);
	return;
	
	}
	
	float ppt              = MFVarGetFloat(_MDInPrecipID,itemID);
	float etp              = MFVarGetFloat(_MDInEvaptrsID,itemID);	
	float snowPChange      = MFVarGetFloat(_MDInSnowPackChgID,itemID);	
	float soilMChange      = MFVarGetFloat(_MDInSoilMoistChgID,itemID);
	float runoffPoolChange = MFVarGetFloat(_MDInGrdWatChgID,itemID);
	float cropETP		 = MFVarGetFloat(_MDInCropEtpID,itemID);
	float runoff           = MFVarGetFloat(_MDInRunoffID,itemID);
	float irrAreaFraction  = MFVarGetFloat(_MDInActIrrAreaID,itemID);
	float waterbalance;
	float infiltration = MFVarGetFloat(_MDInActIrrAreaID,itemID);
	// float irrPercolation =MFVarGetFloat(_MDInIrrigationDrainageID,itemID);
	float gwAbstraction=MFVarGetFloat(_MDInGrdWaterAbstractionID,itemID);
	float disAbstraction=MFVarGetFloat(_MDInDischargeAbstractionID,itemID);
	float exAbstraction= MFVarGetFloat(_MDInExcessAbstractionID,itemID);
	float irrDemand =MFVarGetFloat(_MDInGrossIrrDemandID,itemID);
	float refETP=MFVarGetFloat(_MDInRefETPID,itemID);

	float irrBalance;
	int isGrowingSeason=0;
	float totETP=0;
	float airTemp = MFVarGetFloat(_MDInAirTemperatureID,itemID);
	if (irrAreaFraction == 0) {
	waterbalance= ppt-etp - snowPChange-soilMChange-runoffPoolChange-runoff;
	totETP=etp;	
	//if (itemID==75532) printf ("DOY %d Precip %f ETP %f Runoff %f  SnowPChange %f Temperature %f  \n", doy, ppt, etp, runoff, snowPChange,airTemp);
	if (airTemp >5){
	if (etp > refETP * etpThreshold){
	isGrowingSeason=1;
	}
	}
	}

//if (itemID==2) printf("airTemp=%f refETP=%f actETP=%f\n",airTemp,refETP,etp);

	else {
		irrBalance = irrDemand - gwAbstraction - disAbstraction - exAbstraction;
		//if (disAbstraction < 0) printf ("DischargeAbstra < 0! %f \n", disAbstraction);
		if (fabs(irrBalance) > 0.1) printf ("IrrBalance %f Demand %f gwAb %f disAbstr %f  exAbstr %f itemID=%i\n",  irrBalance,  irrDemand, gwAbstraction, disAbstraction, exAbstraction,itemID);
	 	
		
		cropETP=cropETP/irrAreaFraction;
		float irrSMChange=MFVarGetFloat(_MDInIrrSoilMoistChgID,itemID);
		if (MFVarTestMissingVal(_MDInActIrrAreaID,itemID)) irrAreaFraction=0;
		totETP = cropETP * irrAreaFraction + etp * (1-irrAreaFraction);
		if (itemID==2001) printf("totETPID =%i\n", _MDOutTotalEvapotranpirationID);
		if (totETP>10)printf("totETP=%f cropETP=%f irrAreaFrac=%f etp%f\n",totETP,cropETP,irrAreaFraction,etp);
//	if (itemID ==4412) printf ("etp %f: cropETP  %f TotETP %f irrAreaFract %f irrDem %f  \n", etp, cropETP, totETP, irrAreaFraction, irrDemand );
	
	float totSMChange;
	totSMChange=irrSMChange*irrAreaFraction+(1-irrAreaFraction)*soilMChange;
//	if (itemID ==4412) printf ("SMDelta %f: IrrSMChange  %f NonIrrDeltaS %f irrAreaFract %f irrDem %f  \n", totSMChange, irrSMChange, soilMChange, irrAreaFraction, irrDemand );
	snowPChange=snowPChange*(1-irrAreaFraction);

		float in = ppt+exAbstraction+disAbstraction;
		float out = totETP + runoffPoolChange+ totSMChange + snowPChange+runoff;
		
//	if (itemID==2001) printf ("OUT= %f eto=%f cropETP=%f totETP=%f IrrSMCH=%f soilChange=%f totSMCHANGE=%f, dR=%f dSP=%f,R=%f \n",out,etp,cropETP, totETP, irrSMChange,soilMChange,totSMChange, runoffPoolChange,snowPChange,runoff);
//	if (itemID==2001) printf ("IN= %f precip=%f exAbs=%f disAbstr=%f \n",in ,ppt,exAbstraction,disAbstraction);

		
		waterbalance=in -out;
		//if (itemID ==39869) printf ("WaterBalacne %f: ppt %f etp %f perc %f infi %f demand %f  \n", waterbalance,ppt,etp, irrPercolation, infiltration);
//	if (itemID ==4412) printf("MDWaterBalance=%f: totETP=%f irrSMChange %f smChange %f irrArea %f cropETP %f Excess %f,ppt %f runoff %f dr%f spack%f \n",waterbalance,totETP, irrSMChange, soilMChange,irrAreaFraction,cropETP,exAbstraction,ppt,runoff,runoffPoolChange,snowPChange);
	
		if (fabs(waterbalance) >0.01 ) 
		{
				
	printf ("WaterBalance= %f in= %f out= %f exe= %f disAbstr=%f itmeID=%i,snowChange=%f\n", waterbalance, in, out, exAbstraction,disAbstraction,itemID,snowPChange);

}
		}
	MFVarSetFloat(_MDOutWaterBalanceID,itemID,waterbalance);
	MFVarSetFloat (_MDInInfiltrationID,itemID, infiltration);
	MFVarSetFloat(_MDOutTotalEvapotranpirationID,itemID,totETP);
	MFVarSetFloat(_MDIsGrowingSeasonID ,itemID,isGrowingSeason);
 	MFVarSetFloat(_MDOutPrecipID,itemID,ppt);
	//if (waterbalance>1){printf (" WaterBalance!%f, irrArea %f etp %f item %i\n", waterbalance,irrAreaFraction, etp,itemID);}
}

int MDWaterBalanceDef() {
	MFDefEntering ("WaterBalance");
	if ((                               MDAccumBalanceDef ()   == CMfailed) ||
	    ((_MDInPrecipID               = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDDischarge                = MDDischargeDef ())     == CMfailed) ||
	    ((_MDBgcRoutingID              = MDBgcRoutingDef ())     == CMfailed) ||
	    ((_MDInEvaptrsID              = MFVarGetID (MDVarEvapotranspiration,   "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInSnowPackChgID          = MFVarGetID (MDVarSnowPackChange,       "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInRefETPID  = MFVarGetID (MDVarPotEvapotrans, "mm", MFInput,  MFState, false)) == CMfailed) ||

	    ((_MDInSoilMoistChgID         = MFVarGetID (MDVarSoilMoistChange,      "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInRunoffID               = MFVarGetID (MDVarRunoff,               "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInCropEtpID              = MFVarGetID (MDVarIrrCropETP,           "mm", MFInput,  MFFlux,  false)) == CMfailed) ||
	    ((_MDInIrrSoilMoistChgID      = MFVarGetID(MDVarIrrSoilMoistureChange, "mm", MFInput, MFFlux,  false)) == CMfailed) ||
	    ((_MDInGrdWatChgID            = MFVarGetID (MDVarGroundWaterChange,    "mm", MFInput, MFFlux,  false)) == CMfailed) ||
	    ((_MDOutPrecipID		  = MFVarGetID (MDVarPrecipitationOUT,	   "mm" ,MFOutput, MFFlux,  false)) == CMfailed) ||	
	    ((_MDInDischargeAbstractionID = MFVarGetID (MDVarDischargeAbstraction, "mm", MFInput, MFFlux,  false)) == CMfailed) ||
	    ((_MDInInfiltrationID        = MFVarGetID (MDVarInfiltration,         "mm", MFInput, MFFlux, false)) == CMfailed)||
	    ((_MDInIrrigationDrainageID = MFVarGetID (MDVarIrrPercolationWater,   "mm", MFInput, MFState, false)) == CMfailed) || 
	    ((_MDInGrdWaterAbstractionID  = MFVarGetID (MDVarGroundWaterAbstraction, "mm", MFInput, MFFlux,  false)) == CMfailed)||
	    ((_MDInActIrrAreaID            = MFVarGetID (MDVarActuallyIrrArea,      "-",  MFInput,  MFState, false)) == CMfailed) ||
	    ((_MDInExcessAbstractionID    = MFVarGetID (MDVarExcessAbstraction,    "mm",     MFInput, MFFlux,  false)) == CMfailed) ||
	    ((_MDInDischargeAbstractionID = MFVarGetID (MDVarDischargeAbstraction,   "mm", MFInput, MFFlux,  false)) == CMfailed) ||
	    ((_MDOutTotalEvapotranpirationID= MFVarGetID(MDVarCombinedEvapotranspiration, "mm",MFOutput,MFState,false))==CMfailed)||
	    ((_MDInAirTemperatureID  = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, false)) == CMfailed) ||
	    ((_MDIsGrowingSeasonID  = MFVarGetID (MDVarIsGrowingSeasonCalc, "DoY",  MFOutput,  MFState, false)) == CMfailed)||
	    ((_MDInGrossIrrDemandID     = MFVarGetID (MDVarIrrGrossIrrigationWaterDemand, "mm", MFInput, MFState, false)) == CMfailed)||
	    ((_MDOutWaterBalanceID        = MFVarGetID (MDVarWaterBalance,         "mm", MFOutput, MFFlux,  false)) == CMfailed)) return (CMfailed);
	_MDOutWaterBalanceID = MFVarSetFunction(_MDOutWaterBalanceID,_MDWaterBalance);
	
	MFDefLeaving ("WaterBalance");
	return (_MDOutWaterBalanceID);	
}
