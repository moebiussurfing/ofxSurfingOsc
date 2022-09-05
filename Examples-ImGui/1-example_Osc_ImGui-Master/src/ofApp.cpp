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
			ui.EndWindow();
		}

		oscHelper.drawImGui();
	}
	ui.End();
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

	/*
	//ofParameter<float> p = e.cast<float>();
	//if (name == "BANG_1") bBeat = p.get();
	//else if (name == "BANG_2") bBang_0 = p.get();
	//else if (name == "BANG_3") bBang_1 = p.get();
	*/

	/*
		for (int i = 0; i < NUM_BANGS - 1; i++)
		{
			if (name == "BANG_" + ofToString(i + 1))
			{

				if (i == 0) bBeat = p.get();
				else if (i == 1) bBang_0 = p.get();
				else if (i == 2) bBang_1 = p.get();

				ofLogNotice(__FUNCTION__) << "\tBANGS\t[" << ofToString(i + 1) << "] " << "BANG";
				return;
			}
		}
	*/
}

//--------------------------------------------------------------
void ofApp::Changed_Toggles(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));

	/*
	for (int i = 0; i < NUM_TOGGLES - 1; i++)
	{
		if (name == "TOGGLE_" + ofToString(i + 1))
		{
			//ofLogNotice(__FUNCTION__) << "\tTOGGLE\t[" << ofToString(i + 1) << "] " << (bToggles[i] ? "ON" : "OFF");
			return;
		}
	}
	*/
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + " \t" + ofToString(e)));

	/*
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

	/*
	for (int i = 0; i < NUM_VALUES - 1; i++)
	{
		if (name == "VALUE " + ofToString(i + 1))
		{
			ofLogNotice(__FUNCTION__) << "\tVALUE\t[" << ofToString(i + 1) << "] " << values[i];

			return;
		}
	}
	*/
}

//--------------------------------------------------------------
void ofApp::Changed_Numbers(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	oscHelper.ui->AddToLog(ofToString("ofApp | " + name + "\t" + ofToString(e)));

	/*
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		if (name == "NUMBER_" + ofToString(i))
		{
			return;
		}
	}
	*/
}

#endif

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