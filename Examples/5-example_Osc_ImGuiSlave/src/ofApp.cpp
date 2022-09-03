#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	name = "ImGuiSlaveBasic";
	ofSetWindowTitle(name);

	ofSetBackgroundColor(ofColor::blue);

	setupOsc();
	setupGui();

	params.add(bpm);
	params.add(bEnable_Beat);
	params.add(bEnable_Bang_0);
	params.add(bEnable_Bang_1);

	psettings.add(oscHelper.bGui);
	psettings.add(ui.bLog);
	ofxSurfingHelpers::loadGroup(psettings);
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
	oscHelper.addReceiver_Bool(bEnable_Beat, "/beat");

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

	oscHelper.setUiPtr(&ui);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ui.Begin();
	{
		if (ui.BeginWindow(name))
		{
			ui.Add(oscHelper.bGui);
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
	ofxSurfingHelpers::saveGroup(psettings);
}