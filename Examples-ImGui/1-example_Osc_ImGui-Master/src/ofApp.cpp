#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel("ofxSurfingOsc", OF_LOG_SILENT);

	ofSetBackgroundColor(ofColor::orange);

	setupOsc();
	setupGui();

	// settings
	psettings.add(oscHelper.bGui);
	psettings.add(oscHelper.bGui_Targets);
	ofxSurfingHelpers::loadGroup(psettings);

	// sender params
	params.add(state1);
	params.add(state2);
	params.add(state3);
	params.add(state4);
	params.add(value1);
	params.add(value2);
	params.add(value3);
	params.add(value4);

	//--

#ifdef USE_local_Targets

	setupTargets();

#endif
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	// Pick mode here:
	ofxSurfingOsc::SurfOscModes mode = ofxSurfingOsc::Master;
	//ofxSurfingOsc::SurfOscModes mode = ofxSurfingOsc::Slave;
	
	name = "ofApp | OSC ";
	if(mode == ofxSurfingOsc::Slave) name += "Slave";
	else if(mode == ofxSurfingOsc::Master) name += "Master";

	ofSetWindowTitle(name);

	oscHelper.setup(mode);

	string tag = "1/";
	string Osc_Address;
	int i = 0;
	Osc_Address = "/bang/" + tag + ofToString(i++);
	oscHelper.addSender_Bool(state1, Osc_Address);
	Osc_Address = "/bang/" + tag + ofToString(i++);
	oscHelper.addSender_Bool(state2, Osc_Address);
	Osc_Address = "/bang/" + tag + ofToString(i++);
	oscHelper.addSender_Bool(state3, Osc_Address);
	Osc_Address = "/bang/" + tag + ofToString(i++);
	oscHelper.addSender_Bool(state4, Osc_Address);
	i = 0;
	Osc_Address = "/value/" + tag + ofToString(i++);
	oscHelper.addSender_Float(value1, Osc_Address);
	Osc_Address = "/value/" + tag + ofToString(i++);
	oscHelper.addSender_Float(value2, Osc_Address);
	Osc_Address = "/value/" + tag + ofToString(i++);
	oscHelper.addSender_Float(value3, Osc_Address);
	Osc_Address = "/value/" + tag + ofToString(i++);
	oscHelper.addSender_Float(value4, Osc_Address);
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
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ui.Begin();
	{
		if (ui.BeginWindow(name))
		{
			ui.Add(oscHelper.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

#ifdef USE_local_Targets
			ui.Add(bBypass, OFX_IM_TOGGLE_ROUNDED_SMALL);
#endif
			ui.AddSeparated();
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

#ifdef USE_local_Targets

	ofAddListener(oscHelper.params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofAddListener(oscHelper.params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofAddListener(oscHelper.params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofAddListener(oscHelper.params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);

#endif
}

//----

#ifdef USE_local_Targets

//--------------------------------------------------------------
void ofApp::setupTargets()
{
	ofAddListener(oscHelper.params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofAddListener(oscHelper.params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofAddListener(oscHelper.params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofAddListener(oscHelper.params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);
}

//--

// Local callbacks
// All the params that 
// we will send and/or receive OSC/MIDI changes.

//--------------------------------------------------------------
void ofApp::Changed_Bangs(ofAbstractParameter& e)//preset load/trig
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;
	 
	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "  \t" + ofToString(e)));
}

//--------------------------------------------------------------
void ofApp::Changed_Toggles(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + " \t" + ofToString(e)));
}

//--------------------------------------------------------------
void ofApp::Changed_Numbers(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));
}

#endif