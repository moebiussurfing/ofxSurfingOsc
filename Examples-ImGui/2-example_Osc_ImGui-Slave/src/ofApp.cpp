#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel("ofxSurfingOsc", OF_LOG_SILENT);

	ofSetBackgroundColor(ofColor::green);

	setupOsc();
	setupGui();

	// Settings
	psettings.add(oscHelper.bGui);
#ifdef USE_local_Targets
	psettings.add(bDebug);
	psettings.add(bBypass);
#endif
	ofxSurfingHelpers::loadGroup(psettings);

	// Sender params
	params.add(bang1);
	params.add(bang2);
	params.add(bang3);
	params.add(bang4);
	params.add(toggle1);
	params.add(toggle2);
	params.add(toggle3);
	params.add(toggle4);
	params.add(value1);
	params.add(value2);
	params.add(value3);
	params.add(value4);
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
	ofxSurfingOsc::SurfOscModes mode = ofxSurfingOsc::Slave;
	
	if(mode == ofxSurfingOsc::Slave) name += "Slave";
	else if(mode == ofxSurfingOsc::Master) name += "Master";

	ofSetWindowTitle(name);
	oscHelper.setup(mode);

	// Link params
	{
		oscHelper.linkBang(bang1);
		oscHelper.linkBang(bang2);
		oscHelper.linkBang(bang3);
		oscHelper.linkBang(bang4);
		oscHelper.linkToggle(toggle1);
		oscHelper.linkToggle(toggle2);
		oscHelper.linkToggle(toggle3);
		oscHelper.linkToggle(toggle4);
		oscHelper.linkValue(value1);
		oscHelper.linkValue(value2);
		oscHelper.linkValue(value3);
		oscHelper.linkValue(value4);
		oscHelper.linkNumber(number1);
		oscHelper.linkNumber(number2);
		oscHelper.linkNumber(number3);
		oscHelper.linkNumber(number4);
	}
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	oscHelper.setUiPtr(&ui);

	ui.setName(name);
	ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	ui.setup();

	ui.addWindowSpecial(oscHelper.bGui);
	ui.addWindowSpecial(oscHelper.bGui_Targets);
	ui.addWindowSpecial(oscHelper.bGui_Enablers);

	ui.startup();
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
			if (oscHelper.bGui) {
				ui.Indent();
				ui.Add(oscHelper.bGui_Targets, OFX_IM_TOGGLE_ROUNDED);
				ui.Unindent();
			}

#ifdef USE_local_Targets
			ui.Add(bDebug, OFX_IM_TOGGLE_ROUNDED_SMALL);
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

//--------------------------------------------------------------
void ofApp::setupTargetsCallbacks()
{
	ofAddListener(oscHelper.params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofAddListener(oscHelper.params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofAddListener(oscHelper.params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofAddListener(oscHelper.params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);
}

//--

// Local callbacks
// All the params that 
// we will send and/or 
// receive OSC changes.

//--------------------------------------------------------------
void ofApp::Changed_Bangs(ofAbstractParameter& e)//preset load/trig
{
	if (bBypass) return;

	string name = e.getName();
	string s = ofToString("ofApp | " + name + "  \t" + ofToString(e));
	ofLogNotice(__FUNCTION__) << s;
	if (bDebug) oscHelper.ui->AddToLog(s);

	if (name == "BANG_0") cout << name << ":    \t" << e << endl;
	else if (name == "BANG_1") cout << name << ":    \t" << e << endl;
	else if (name == "BANG_2") cout << name << ":    \t" << e << endl;
	else if (name == "BANG_3") cout << name << ":    \t" << e << endl;
}

//--------------------------------------------------------------
void ofApp::Changed_Toggles(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	string s = ofToString("ofApp | " + name + "\t" + ofToString(e));
	ofLogNotice(__FUNCTION__) << s;
	if (bDebug) oscHelper.ui->AddToLog(s);

	// Example
	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		if (name == "TOGGLE_" + ofToString(i))
		{
			cout << name << ":    \t" << e << endl;
			ofLogNotice(__FUNCTION__) << "\tTOGGLE\t[" << ofToString(i) << "] " << 
				(oscHelper.bToggles[i] ? "ON" : "OFF");
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	string s = ofToString("ofApp | " + name + " \t" + ofToString(e));
	ofLogNotice(__FUNCTION__) << s;
	if (bDebug) oscHelper.ui->AddToLog(s);

	// Example
	if (name == "VALUE_0") cout << name << ":    \t" << e << endl;
	else if (name == "VALUE_1") cout << name << ":    \t" << e << endl;
	else if (name == "VALUE_2") cout << name << ":    \t" << e << endl;
	else if (name == "VALUE_3") cout << name << ":    \t" << e << endl;

	/*
	// Example
	if (e.type() == typeid(ofParameter<float>).name())
	{
		ofParameter<float> p = e.cast<float>();

		ofLogNotice("ofApp") << ofToString(p, 2);

		if (_name == "values_" + ofToString(8)) // bpm
		{
			ofLogNotice("ofApp") << "BPM " << ofToString(p, 2);
		}

		//if (name == "VALUE_1") {
		//	bpm = p.get();
		//}
	}
	*/

	// Example
	for (int i = 0; i < NUM_VALUES; i++)
	{
		if (name == "VALUE_" + ofToString(i))
		{
			ofLogNotice(__FUNCTION__) << "\tVALUE\t[" << ofToString(i) << "] " << oscHelper.values[i];
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Numbers(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	string s = ofToString("ofApp | " + name + "\t" + ofToString(e));
	ofLogNotice(__FUNCTION__) << s;
	if (bDebug) oscHelper.ui->AddToLog(s);

	// Example
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		// Discard if not an int param
		if (e.type() == typeid(ofParameter<int>).name())
		{
			ofParameter<int> p = e.cast<int>();
			if (name != p.getName()) return;//skip if not been trigged!
			cout << p.getName() << ":    \t" << p.get() << endl;
			return;
		}
	}
}

#endif