#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//w.doReset();

	//ofSetLogLevel("ofxSurfingOsc", OF_LOG_SILENT);

	ofSetBackgroundColor(ofColor::orange);

	setupOsc();
	setupGui();

	// Settings
	psettings.add(oscHelper.bGui);
	ofxSurfingHelpers::loadGroup(psettings);

	// Sender params
	params.add(bang0);
	params.add(bang1);
	params.add(bang2);
	params.add(bang3);
	params.add(bang4);
	params.add(toggle0);
	params.add(toggle1);
	params.add(toggle2);
	params.add(toggle3);
	params.add(toggle4);
	params.add(value0);
	params.add(value1);
	params.add(value2);
	params.add(value3);
	params.add(value4);
	params.add(number0);
	params.add(number1);
	params.add(number2);
	params.add(number3);
	params.add(number4);

	//--

#ifdef USE_local_Targets
	setupTargetsCallbacks();
#endif
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	name = "ofApp | OSC ";

	// Pick mode here:
	ofxSurfingOsc::SurfOscModes mode = ofxSurfingOsc::Master;

	if (mode == ofxSurfingOsc::Slave) name += "Slave";
	else if (mode == ofxSurfingOsc::Master) name += "Master";
	ofSetWindowTitle(name);

	oscHelper.disableGuiInternal();
	oscHelper.setCustomTemplate(true);

	oscHelper.setup(mode);

	// Link param
	//if(0)
#ifndef USE_local_Targets
	{
		oscHelper.linkBang(bang0);
		oscHelper.linkBang(bang1);
		oscHelper.linkBang(bang2);
		oscHelper.linkBang(bang3);
		oscHelper.linkBang(bang4);
		oscHelper.linkToggle(toggle0);
		oscHelper.linkToggle(toggle1);
		oscHelper.linkToggle(toggle2);
		oscHelper.linkToggle(toggle3);
		oscHelper.linkToggle(toggle4);
		oscHelper.linkValue(value0);
		oscHelper.linkValue(value1);
		oscHelper.linkValue(value2);
		oscHelper.linkValue(value3);
		oscHelper.linkValue(value4);
		oscHelper.linkNumber(number0);
		oscHelper.linkNumber(number1);
		oscHelper.linkNumber(number2);
		oscHelper.linkNumber(number3);
		oscHelper.linkNumber(number4);
	}
#endif
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	ui.setName("ofApp");
	ui.setup();
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
			ui.AddSpacingBigSeparated();
			ui.Add(bBypass, OFX_IM_TOGGLE_ROUNDED_SMALL);
#endif
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

#ifdef USE_local_Targets
	ofAddListener(oscHelper.params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofAddListener(oscHelper.params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofAddListener(oscHelper.params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofAddListener(oscHelper.params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);
#endif
}

//----

#ifdef USE_local_Targets

// Local callbacks
// All the params that 
// we will send and/or 
// receive OSC changes.

//--------------------------------------------------------------
void ofApp::setupTargetsCallbacks()
{
	ofAddListener(oscHelper.params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofAddListener(oscHelper.params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofAddListener(oscHelper.params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofAddListener(oscHelper.params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);
}

//--

//--------------------------------------------------------------
void ofApp::Changed_Bangs(ofAbstractParameter& e)//preset load/trig
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "  \t" + ofToString(e)));

	if (e.type() == typeid(ofParameter<bool>).name())
	{
		ofParameter<bool> p = e.cast<bool>();

		if (name == "BANG_0") bang0 = p;
		else if (name == "BANG_1") bang1 = p;
		else if (name == "BANG_2") bang2 = p;
		else if (name == "BANG_3") bang3 = p;
		else if (name == "BANG_4") bang4 = p;
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Toggles(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));

	if (e.type() == typeid(ofParameter<bool>).name())
	{
		ofParameter<bool> p = e.cast<bool>();

		if (name == "TOGGLE_0") toggle0 = p;
		else if (name == "TOGGLE_1") toggle1 = p;
		else if (name == "TOGGLE_2") toggle2 = p;
		else if (name == "TOGGLE_3") toggle3 = p;
		else if (name == "TOGGLE_4") toggle4 = p;
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + " \t" + ofToString(e)));

	if (e.type() == typeid(ofParameter<float>).name())
	{
		ofParameter<float> p = e.cast<float>();

		if (name == "VALUE_0") value0 = p;
		else if (name == "VALUE_1") value1 = p;
		else if (name == "VALUE_2") value2 = p;
		else if (name == "VALUE_3") value3 = p;
		else if (name == "VALUE_4") value4 = p;
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Numbers(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));

	if (e.type() == typeid(ofParameter<int>).name())
	{
		ofParameter<int> p = e.cast<int>();

		if (name == "NUMBER_0") number0 = p;
		else if (name == "NUMBER_1") number1 = p;
		else if (name == "NUMBER_2") number2 = p;
		else if (name == "NUMBER_3") number3 = p;
		else if (name == "NUMBER_4") number4 = p;
	}
}

#endif