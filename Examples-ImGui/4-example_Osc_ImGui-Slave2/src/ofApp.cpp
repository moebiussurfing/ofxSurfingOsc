#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	name = "ImGui-Slave2";
	ofSetWindowTitle(name);

	ofSetBackgroundColor(ofColor::blue);

	setupOsc();
	setupGui();

	params.add(bpm);
	params.add(bEnable_Beat);
	params.add(bEnable_Bang_0);
	params.add(bEnable_Bang_1);
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	oscHelper.setup(ofxSurfingOsc::Slave);

	string Osc_Address;
	string tag = "1/";

	Osc_Address = "/bang/";
	oscHelper.addReceiver_Float(bpm, "/bpm");

	Osc_Address = "/beat";
	oscHelper.addReceiver_Bool(bEnable_Beat, Osc_Address);

	Osc_Address = "/bang/" + tag + "0";
	oscHelper.addReceiver_Bool(bEnable_Bang_0, Osc_Address);

	Osc_Address = "/bang/" + tag + "1";
	oscHelper.addReceiver_Bool(bEnable_Bang_1, Osc_Address);
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	ui.setName(name);
	ui.setup();
	ui.startup();

	// custom styles
	{
		ui.AddStyle(bpm, OFX_IM_HSLIDER_BIG);
		SurfingGuiTypes type = OFX_IM_TOGGLE_BIG_XXXL_BORDER;
		ui.AddStyle(bEnable_Beat, type);
		ui.AddStyle(bEnable_Bang_0, type);
		ui.AddStyle(bEnable_Bang_1, type);
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ui.Begin();
	{
		if (ui.BeginWindow(name))
		{
			ui.Add(oscHelper.bGui, OFX_IM_TOGGLE_ROUNDED);
			ui.Add(ui.bLog, OFX_IM_TOGGLE_ROUNDED_SMALL);
			ui.AddSpacingBigSeparated();

			ui.AddGroup(params);

			ui.EndWindow();
		}

		oscHelper.drawImGui();
	}
	ui.End();
}

//--------------------------------------------------------------
void ofApp::exit()
{
}