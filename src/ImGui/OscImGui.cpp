//#pragma once
//
//#include "ofxSurfingOsc.h"
//
////#include "OscImGui.h"
////#define USE_IM_GUI
//
//#ifdef USE_IM_GUI
//
//#include "ofMain.h"
//#include "ofxSurfingImGui.h"
//
////--------------------------------------------------------------
//void ofxSurfingOsc::drawImGui() {
//
//	{
//		if (ui == nullptr) return;
//
//		if (ui->BeginWindowSpecial(bGui))
//		{
//			ui->AddLabelHuge("OSC");
//			ui->AddSpacing();
//
//			//--
//
//			// Out
//
//			if (bUseOscOut)
//			{
//				//if (!ui->bMinimize) ui->AddLabelBig("OUT");
//				ui->Add(bEnableOsc_Output, OFX_IM_TOGGLE);
//				if (bEnableOsc_Output) {
//					if (!ui->bMinimize)
//					{
//						string tt = "Must restart the app \nto update these settings!";
//						//ui->AddLabelBig(ofToString(OSC_OutputPort));
//						ui->Add(OSC_OutputPort, OFX_IM_DRAG);
//						ui->AddTooltip(tt);
//						ui->Add(OSC_OutputIp, OFX_IM_TEXT_INPUT);
//						ui->AddTooltip(tt);
//					}
//
//					if (getOutEnablersSize() != 0) ui->AddSpacingSeparated();
//
//					//if (!ui->bMinimize) ui->AddLabelBig("ENABLERS");
//					SurfingGuiTypes s = OFX_IM_TOGGLE_SMALL;
//					for (int i = 0; i < getOutEnablersSize(); i++) {
//						ui->Add(getOutEnabler(i), s);
//					}
//					/*
//					ui->Add(bBeat, s);
//					ui->Add(bBang_0, s);
//					ui->Add(bBang_1, s);
//					*/
//				}
//			}
//
//			if (!ui->bMinimize)
//			{
//				ui->AddSpacingSeparated();
//
//				ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
//				if (bGui_InternalAllowed) ui->Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
//				ui->Add(bHelp, OFX_IM_TOGGLE_ROUNDED_MINI);
//			}
//
//			ui->EndWindowSpecial();
//		}
//	}
//
//}
//
////
////class OscImGui
////{
////public:
////	
////	ofxSurfingGui *ui;
////
////	void setUiPtr(ofxSurfingGui* _ui) {
////		ui = _ui;
////	}
////
////	OscImGui::OscImGui() {
////		//exit();
////	};
////
////	OscImGui::~OscImGui() {
////	};
////	
////	/*
////	void draw() {
////		if (ui == nullptr) return;
////	};
////	
////	void drawWidgets() {
////	
////	};
////	
////	void exit() {
////		//if (ui == nullptr) return;
////		//delete ui;
////	};
////	*/
////};
////
//
//#endif
