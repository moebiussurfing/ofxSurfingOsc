#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetLogLevel("ofxSurfingOsc", OF_LOG_SILENT);

	name = "ImGuiSlavePatching";
	ofSetWindowTitle(name);

	ofSetBackgroundColor(ofColor::blue);

	setupOsc();
	setupGui();

	params.add(val0);
	params.add(val1);
	params.add(val2);
	params.add(bpm);
	params.add(bBeat);
	params.add(bBang_0);
	params.add(bBang_1);

	//--

#ifdef USE_local_Targets
	//setupTargets();
#endif
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	oscHelper.enablePatchingManager();
	//oscHelper.enableGuiInternalAllow();
	//oscHelper.setCustomTemplate(true);//must call before setup!
	
	oscHelper.setup(ofxSurfingOsc::Slave);

	//--

	/*
	string Osc_Address;
	string tag = "1/";

	Osc_Address = "/bang/";
	oscHelper.addReceiver_Float(bpm, "/bpm");

	Osc_Address = "/beat";
	oscHelper.addReceiver_Bool(bBeat, Osc_Address);

	Osc_Address = "/bang/" + tag + "0";
	oscHelper.addReceiver_Bool(bBang_0, Osc_Address);

	Osc_Address = "/bang/" + tag + "1";
	oscHelper.addReceiver_Bool(bBang_1, Osc_Address);
	*/
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	ui.setName(name);
	ui.setup();

	// custom styles
	{
		ui.AddStyle(bpm, OFX_IM_HSLIDER_BIG);
		SurfingGuiTypes type = OFX_IM_TOGGLE_BIG_XXXL_BORDER;
		ui.AddStyle(bBeat, type);
		ui.AddStyle(bBang_0, type);
		ui.AddStyle(bBang_1, type);
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	val0 = oscHelper.getOutValue(0);
	val1 = oscHelper.getOutValue(1);
	val2 = oscHelper.getOutValue(2);

	bBeat = oscHelper.getOutBang(0);
	bBang_0 = oscHelper.getOutBang(1);
	bBang_1 = oscHelper.getOutBang(2);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ui.Begin();
	{
		if (ui.BeginWindow(name))
		{
			ui.Add(oscHelper.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			ui.AddSpacingBigSeparated();

			ui.Add(ui.bLog, OFX_IM_TOGGLE_ROUNDED_SMALL);
#ifdef USE_local_Targets
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

	ofParameter<bool> p = e.cast<bool>();

	if (name == "BANG_1") bBeat = p.get();
	else if (name == "BANG_2") bBang_0 = p.get();
	else if (name == "BANG_3") bBang_1 = p.get();

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

	for (int i = 0; i < NUM_TOGGLES - 1; i++)
	{
		if (name == "TOGGLE_" + ofToString(i + 1))
		{
			//ofLogNotice(__FUNCTION__) << "\tTOGGLE\t[" << ofToString(i + 1) << "] " << (bToggles[i] ? "ON" : "OFF");
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << ":" << e;

	if (e.type() == typeid(ofParameter<float>).name())
	{
		ofParameter<float> p = e.cast<float>();

		ofLogNotice("ofApp") << ofToString(p, 2);

		/*
		if (_name == "values_" + ofToString(8)) // bpm
		{
			ofLogNotice("ofApp") << "BPM " << ofToString(p, 2);
		}
		*/

		if (name == "VALUE_1") {

			bpm = p.get();
		}
	}

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

	for (int i = 0; i < NUM_NUMBERS - 1; i++)
	{
		if (name == "NUMBER_" + ofToString(i + 1))
		{
			//ofLogNotice(__FUNCTION__) << "\tNUMBER\t[" << ofToString(i + 1) << "] " << numbers[i];
			return;
		}
	}
}
#endif