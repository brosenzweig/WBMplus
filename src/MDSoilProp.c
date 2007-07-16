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
//~ /******************************************************************************

//~ GHAAS Water Balance Model Library V1.0
//~ Global Hydrologic Archive and Analysis System
//~ Copyright 1994-2004, University of New Hampshire

//~ MDSoilProp.c

//~ balazs.fekete@unh.edu

//~ *******************************************************************************/

//~ #include<stdio.h>
//~ #include<cm.h>
//~ #include<MF.h>
//~ #include<MD.h>
//~ //Major changes!!!!!!!!!!!!!!!
//~ // FC, PWP and Porsosity are in m3/m3. Multiply with rootDepth to get AvailWaterCapa when needed!


//~ static int _MDInSoilTextID, _MDInRootDepthID;
//~ static int _MDOutSPorosID, _MDOutWltPntID, _MDOutAWCapID = CMfailed;
//~ static int _MDOutFieldCapID;
//~ static int _MDSoilTextNum;
//~ static float _MDSoilPorosity  [] = { 39.0, 47.0, 55.0, 43.0, 47.0, 51.0, 47.0, 47.0, 39.0 };
//~ static float _MDFieldCapacity [] = { 54.0, 62.0, 64.0, 58.0, 62.0, 63.0, 62.0, 62.0, 54.0 };
//~ static float _MDWiltingPoint  [] = { 21.0, 34.0, 40.0, 28.0, 34.0, 37.0, 34.0, 34.0, 21.0 };

//~ static void _MDSoilProp (int itemID) {
	//~ int soilText;
	//~ float rootDepth, porosity;
	
	//~ if (MFVarTestMissingVal (_MDInSoilTextID,  itemID) ||
		 //~ MFVarTestMissingVal (_MDInRootDepthID, itemID)) {
		//~ MFVarSetMissingVal (_MDOutSPorosID,itemID);
		//~ MFVarSetMissingVal (_MDOutWltPntID,itemID);
		//~ MFVarSetMissingVal(	_MDOutFieldCapID,itemID);
		//~ MFVarSetMissingVal (_MDOutAWCapID, itemID);
		//~ return;
	//~ }

	//~ soilText  = MFVarGetInt   (_MDInSoilTextID, itemID);
	//~ rootDepth = MFVarGetFloat (_MDInRootDepthID, itemID);

	//~ if ((soilText < 0) || (soilText > _MDSoilTextNum)) {
		//~ fprintf (stderr,"Warning: Invalid soil texture [%d] at [%d]!\n",soilText,itemID);
		//~ soilText = 0;
		//~ MFVarSetInt (_MDInSoilTextID,itemID,soilText);
	//~ }
	//~ //old version: porosity = rootDepth * _MDSoilPorosity [soilText] / 100.0;	
	//~ porosity = _MDSoilPorosity [soilText] / 100.0	;
	//~ MFVarSetFloat (_MDOutSPorosID,itemID,porosity);
	//~ if (porosity <0)printf ("porosisy in soil Prop %f \n", porosity);
	//~ if (_MDWiltingPoint [soilText]  <0)printf ("WP Problem! in soil Prop %f \n", _MDWiltingPoint [soilText] );
	//~ if (_MDFieldCapacity [soilText]  <0)printf ("WP Problem! in soil Prop %f \n", _MDFieldCapacity [soilText] );
	//~ MFVarSetFloat (_MDOutWltPntID,itemID,porosity * _MDWiltingPoint [soilText] / 100.0);
	//~ MFVarSetFloat (_MDOutFieldCapID,itemID,porosity * _MDFieldCapacity [soilText] / 100.0);
		//~ float watCap= porosity * (_MDFieldCapacity [soilText] - _MDWiltingPoint [soilText]) / 100.0;
		//~ //printf("WP FC %f %f 
	//~ if (watCap <0) printf("WP > FP!! ");
//~ //		printf ("_MDOutWltPntID, _MDOutFieldCapID in MDSoilProp> %i %i \n",_MDOutWltPntID, _MDOutFieldCapID); 
	//~ MFVarSetFloat (_MDOutAWCapID, itemID,watCap);

//~ }

//~ int MDSoilPropDef () {
	//~ if (_MDOutAWCapID != CMfailed) return (_MDOutAWCapID);

	//~ MFDefEntering ("Soil Proporties");
	//~ _MDSoilTextNum = sizeof (_MDSoilPorosity) / sizeof (_MDSoilPorosity [0]);
	//~ if (((_MDInRootDepthID = MDRootDepthDef ()) == CMfailed) ||
		 //~ ((_MDInSoilTextID  = MFVarGetID (MDVarSoilTexture,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
		 //~ ((_MDOutSPorosID   = MFVarGetID (MDVarSoilPorosity, "mm",     MFOutput, MFState, false))  == CMfailed) ||
		 //~ ((_MDOutWltPntID   = MFVarGetID (MDVarWiltingPoint, "mm/mm",     MFOutput, MFState, false))  == CMfailed) ||
		//~ ((_MDOutFieldCapID	= MFVarGetID(MDVarFieldCapacity,"mm/mm", MFOutput, MFState, false))==CMfailed)||
		 //~ ((_MDOutAWCapID    = MFVarGetID (MDVarAvailWatCap,  "mm",     MFOutput, MFState, false)) == CMfailed))
		//~ return (CMfailed);
	
	//~ MFDefLeaving ("Soil Proporties");
	//~ return (MFVarSetFunction (_MDOutAWCapID,_MDSoilProp));
//~ }
