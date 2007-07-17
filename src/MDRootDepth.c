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

//~ MDRootDepth.c

//~ balazs.fekete@unh.edu

//~ *******************************************************************************/

//~ #include<stdio.h>
//~ #include<cm.h>
//~ #include<MF.h>
//~ #include<MD.h>

//~ static int _MDInSoilTextID   = MFUnset;
//~ static int _MDOutRootDepthID = MFUnset;

//~ static int _MDSoilTextNum = 9;

//~ static void _MDRootDepth1DLookup (int itemID) {
//~ /* Input */
	//~ int soilText;
//~ /* Output */
	//~ float rootDepth;

	//~ if (MFVarTestMissingVal (_MDInSoilTextID, itemID)) { MFVarSetMissingVal (_MDInSoilTextID, itemID); return; }

	//~ soilText = MFVarGetInt   (_MDInSoilTextID, itemID);
	//~ if ((soilText < 0) || (soilText > _MDSoilTextNum))
		
		//~ { fprintf (stderr,"Warning: Invalid soil texture [%d] at [%d]!\n",soilText,itemID); soilText = 0; }
	//~ rootDepth = 0.5;
	//~ MFVarSetFloat (_MDOutRootDepthID,itemID,rootDepth);
		
		
		
		
		
//~ }

//~ static int _MDInCoverID;

//~ static void _MDRootDepth2DLookup (int itemID) {
	//~ int soilText, cover;
	//~ float rootDepth;
	
	//~ if (MFVarTestMissingVal (_MDInSoilTextID, itemID)) { MFVarSetMissingVal (_MDInSoilTextID, itemID); return; }
	//~ if (MFVarTestMissingVal (_MDInCoverID,    itemID)) return;

	//~ soilText = MFVarGetInt   (_MDInSoilTextID, itemID);
	//~ cover    = MFVarGetInt   (_MDInCoverID,    itemID) - 1;
	//~ if ((soilText < 0) || (soilText > _MDSoilTextNum)) {
		//~ CMmsgPrint (CMmsgWarning,"Warning: Invalid soil texture [%d] at [%d] in: %s:%d\n",soilText,itemID,__FILE__,__LINE__);
		//~ soilText = 0;
	//~ }
//~ //float rootDepth[CoverType][Soiltexture]

//~ float RootDepth [7][8] = 
	//~ {
		//~ {2.50 ,2.00 ,1.17 ,2.00, 2.00, 1.60 ,2.00 ,0.10},
		//~ {2.50, 2.00, 1.17, 2.00, 2.00, 1.60, 2.00, 0.10},
	    //~ {1.50 ,1.50, 0.67 ,1.67, 1.50 ,1.00 ,1.50, 0.10},
    	//~ {1.00 ,1.25, 0.67 ,1.00 ,1.25 ,1.00 ,1.25, 0.10},
	    //~ {0.40 ,0.40, 0.40 ,0.40 ,0.40 ,0.40 ,0.40, 0.10},
	    //~ {0.50 ,0.50, 0.50 ,0.50 ,0.50 ,0.50 ,0.50, 0.10},
     	//~ {1.50 ,1.50 ,0.67, 1.67 ,1.50 ,1.00, 1.50, 0.10}
      	//~ };

	//~ rootDepth = RootDepth[cover][soilText];
	//~ MFVarSetFloat (_MDOutRootDepthID,itemID,rootDepth);
//~ }

//~ enum { MDinput, MD1DLookup, MD2DLookup };

//~ int MDRootDepthDef () {
	//~ int optID = MDinput;
	//~ const char *optStr, *optName = MDVarRootingDepth;
	//~ const char *options [] = { MDInputStr, "1Dlookup", "2Dlookup", (char *) NULL };

	//~ if (_MDOutRootDepthID != MFUnset) return (_MDOutRootDepthID);

	//~ MFDefEntering ("Rooting Depth");
	//~ if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	//~ switch (optID) {
		//~ case MDinput: _MDOutRootDepthID = MFVarGetID (MDVarRootingDepth, "mm", MFInput, MFState, false); break;
		//~ case MD1DLookup:
			//~ if (((_MDInSoilTextID   = MFVarGetID (MDVarSoilTexture,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
			    //~ ((_MDOutRootDepthID = MFVarGetID (MDVarRootingDepth, "mm",     MFOutput, MFState, false)) == CMfailed))
				//~ return (CMfailed);
			//~ _MDOutRootDepthID = MFVarSetFunction (_MDOutRootDepthID,_MDRootDepth1DLookup);
			//~ break;
		//~ case MD2DLookup:
			//~ if (((_MDInCoverID      = MFVarGetID (MDVarSoilTexture,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
			    //~ ((_MDInSoilTextID   = MFVarGetID (MDVarSoilTexture,  MFNoUnit, MFInput,  MFState, false)) == CMfailed) ||
			    //~ ((_MDOutRootDepthID = MFVarGetID (MDVarRootingDepth, "mm",     MFOutput, MFState, false)) == CMfailed))
				//~ return (CMfailed);
			//~ _MDOutRootDepthID = MFVarSetFunction (_MDOutRootDepthID,_MDRootDepth2DLookup);
			//~ break;
		//~ default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	//~ }
	//~ MFDefLeaving ("Rooting Depth");
	//~ return (_MDOutRootDepthID);
//~ }
