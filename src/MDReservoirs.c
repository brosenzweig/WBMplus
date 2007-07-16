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
#include<cm.h>
#include<MF.h>
#include<MD.h>
 

static int _MDResCapacityID;
static int _MDResReleaseID=CMfailed;
static int _MDINDischarge0ID;
static int _MDRefDischargeID;
static int _MDResStorageID;
static int _MDResStorageChangeID;

static void _MDReservoirRelease (int itemID) {
	
float debugLat =35.75; // Hoover dam!
float debugLon=-114.75;
/* Input */
	float meanInflow;   
	float previousResStorage;
	float resCapacity;
	float resStorageChange;
	float curInflow;
/* Local */
	float resRelease;     
/* Output */
	float resStorage;
/*Parameters */
	float drySeasonPct = .6;
	float wetSeasonPct = 0.16;	

	if (MFVarTestMissingVal (_MDResCapacityID,       itemID) ||
	    MFVarTestMissingVal(_MDINDischarge0ID,itemID)||
	    MFVarTestMissingVal(_MDRefDischargeID,itemID)) { 
		MFVarSetFloat (_MDResReleaseID,itemID,0); 
		return; 
	}

	resCapacity = MFVarGetFloat (_MDResCapacityID, itemID);
	if (resCapacity <= 0.0) {
		MFVarSetFloat(_MDResReleaseID,itemID,0.0);
		return;	
	}
	else {
		meanInflow = MFVarGetFloat(_MDRefDischargeID,itemID);
		curInflow  = MFVarGetFloat(_MDINDischarge0ID,itemID);
		resStorage = MFVarGetFloat(_MDResStorageID,itemID);
		//if (curInflow >0)printf("meanInflow %f curInflow %f \n",meanInflow,curInflow);	
		previousResStorage=resStorage;
			
		if (curInflow >meanInflow) {
			resRelease=wetSeasonPct*meanInflow;
		}
		else {
		resRelease = drySeasonPct*curInflow + (meanInflow-curInflow);
			 
		}			
 		resStorage=previousResStorage+(curInflow-resRelease)*24*3600/1e9;
		if (resStorage > resCapacity) {
			//resRelease=(resStorage-resCapacity)/24/3600*1e9;			
			resRelease=curInflow *3600*24/1e9 + previousResStorage-resCapacity;
	 		resRelease = resRelease *1e9/(3600*24);
			}
			
		resStorage=previousResStorage+(curInflow-resRelease)*24*3600/1e9;
			
		if (resStorage <= 0) {
			resRelease=previousResStorage +curInflow  * 3600*24/1e9;
			resRelease = resRelease *1e9/(3600*24);
			resStorage=0;
			}
			
			resStorageChange = resStorage-previousResStorage;
//		printf("null%f \n",resStorage);			
	
//		if (resRelease >0)printf("REL%f\n",resRelease);
		MFVarSetFloat(_MDResStorageID,itemID,resStorage);//Storage km3!
		MFVarSetFloat(_MDResReleaseID,itemID,resRelease);//release in m3/s
		MFVarSetFloat(_MDResStorageChangeID,itemID,resStorageChange);
	}
//	if (resCapacity > 0.0)printf ("ResCapacity%f \n", resCapacity);		
	if ((MFModelGetLatitude(itemID) == debugLat) && (MFModelGetLongitude(itemID) == debugLon) && (curInflow > 0))
		CMmsgPrint (CMmsgDebug,"PRevStor\t IN \t Rel \t CurStor \t ResCapc \t  LAT \t LON \t %f\t%f\t%f\t%f\t%f\t%i  \n",
		            previousResStorage, curInflow*3600*24/1e9, resRelease*24*3600/1e9, resStorage, resCapacity, itemID);
}
	
int MDReservoirDef () {
	// const char *optStr, *optName = "ReservoirOperation";
	// const char *options [] = { "noOperation" , "maximal", "noReservoirs", (char *) NULL };
  
	MFDefEntering ("ReservoirOperation");
			
	if (_MDResReleaseID != CMfailed)	return (_MDResReleaseID);

	if (((_MDResReleaseID       = MFVarGetID (MDVarReservoirRelease,       "m3/s", MFOutput, MFFlux,  false)) == CMfailed) ||
	    ((_MDResCapacityID      = MFVarGetID (MDVarReservoirCapacity,      "km3",  MFInput,  MFState, false)) == CMfailed) ||
	    ((_MDINDischarge0ID     = MFVarGetID (MDVarRiverDischarge0,        "m3/s", MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDRefDischargeID     = MFVarGetID (MDVarRefDischarge,           "m3/s", MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDResStorageID       = MFVarGetID (MDVarReservoirStorage,       "km3",  MFOutput, MFState, true))  == CMfailed) ||
	    ((_MDResStorageChangeID = MFVarGetID (MDVarReservoirStorageChange, "km3",  MFOutput, MFState, true))  == CMfailed)) return (CMfailed);
	_MDResReleaseID = MFVarSetFunction(_MDResReleaseID,_MDReservoirRelease);
	MFDefLeaving ("ReservoirOperation");
	return (_MDResReleaseID); 
}
