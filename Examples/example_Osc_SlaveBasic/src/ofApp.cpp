#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowTitle("OSC | Slave");

	ofSetBackgroundColor(ofColor::blue);

	//--

	// 1. (Targets) Controllers
	// Creates the sender params
	setupTargets();

	// 2. OSC Publisher params 
	// to send remote control OSC/MIDI
	setupOsc();

	// 3. Gui 
	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	// Subscribe or prepare to Publish 
	// all local target params to the controller

	// NOTE:
	// Here we add all the local parameters (TARGETS) 
	// that we want to control-from / to-receive-from,
	// or to control-to / to-send-to.

	// MIDI linking / assignments will be made 
	// on the add-on gui by a MIDI learn workflow,
	// bc it's easier than passing all the notes/cc addresses here.

	//----

	oscHelper.setup(ofxSurfingOsc::Slave);

	//----

	// Link params
	setupReceivers();

	//----

	// Startup

	// Call it when all the local/target 
	// parameters are already added,
	// to finish initialization process!

	oscHelper.startup();
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	gui.setup("ofApp SLAVE");
	gui.add(oscHelper.bGui); // visible toggle
	//gui.add(bRandom);
	gui.add(bBypass); // bypass on the callbacks, not the receiving itself!

	// Widgets for all the params that will be:
	// 1. Updating when receiving OSC/MIDI messages 
	// or 
	// 2. Sending OSC/MIDI messages when params changes locally
	// that will depends on how params 
	// are subscribed to the add-on object: 
	// addReceiver/addSender -> OSC in/out
	gui.add(params_Targets);

	// Customize
	gui.getGroup(params_Targets.getName()).minimize();
	gui.setPosition(10, 10);
}

//--------------------------------------------------------------
void ofApp::setupTargets() 
{
	//--

	// 1. Local params

	// Create params used (to receive or) to send from the add-on class

	// NOTE:
	// Param names suffix
	// goes from 1 to 8, 
	// not starting at 0 like vectors

	//--

	// Bangs / Triggers

	params_Bangs.setName("BANGS");
	for (int i = 0; i < NUM_BANGS; i++)
	{
		bBangs[i].set("BANG " + ofToString(i + 1), false);
		params_Bangs.add(bBangs[i]);
	}
	ofAddListener(params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);

	//--

	// Toggles / State On-Off

	params_Toggles.setName("TOGGLES");
	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		bToggles[i].set("TOGGLE " + ofToString(i + 1), false);
		params_Toggles.add(bToggles[i]);
	}
	ofAddListener(params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);

	//--

	// Values / Float's

	params_Values.setName("VALUES");
	for (int i = 0; i < NUM_VALUES; i++)
	{
		values[i].set("VALUE " + ofToString(i + 1), 0.f, 0.f, 1.f);
		params_Values.add(values[i]);
	}
	ofAddListener(params_Values.parameterChangedE(), this, &ofApp::Changed_Values);

	//--

	// Numbers / Int's

	params_Numbers.setName("NUMBERS");
	int _max = 1000;
	for (int i = 0; i < NUM_VALUES; i++)
	{
		numbers[i].set("NUMBER " + ofToString(i + 1), 0, 0, _max);
		params_Numbers.add(numbers[i]);
	}
	ofAddListener(params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);

	//----

	// Group

	params_Targets.setName("Targets");
	params_Targets.add(params_Bangs);
	params_Targets.add(params_Toggles);
	params_Targets.add(params_Values);
	params_Targets.add(params_Numbers);
}

//--------------------------------------------------------------
void ofApp::setupReceivers()
{
	// Add OSC targets to the add-on class.
	// this will subscribe the OSC addresses with/to the target local parameters.

	// NOTE:
	// Extra '1/' -> it's an address pattern to 
	// better adapt to pages or different layouts 
	// of some controller apps like TouchOSC!
	string tag = "1/";

	//--

	// Bangs

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string Osc_Address = "/bang/" + tag + ofToString(i + 1);
		//oscHelper.addSender_Bool(bBangs[i], Osc_Address);
		oscHelper.addReceiver_Bool(bBangs[i], Osc_Address);
	}

	//--

	// Toggles

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string Osc_Address = "/toggle/" + tag + ofToString(i + 1);
		//oscHelper.addSender_Bool(bToggles[i], Osc_Address);
		oscHelper.addReceiver_Bool(bToggles[i], Osc_Address);
	}

	//--

	// Values (Float's)

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string Osc_Address = "/value/" + tag + ofToString(i + 1);
		//oscHelper.addSender_Float(values[i], Osc_Address);
		oscHelper.addReceiver_Float(values[i], Osc_Address);
	}

	//--

	// Numbers (Int's)

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string Osc_Address = "/number/" + tag + ofToString(i + 1);
		//oscHelper.addSender_Int(numbers[i], Osc_Address);
		oscHelper.addReceiver_Int(numbers[i], Osc_Address);
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	gui.draw();

	//--

	/*
	// Timed Randomizer
	if (bRandom && ofGetFrameNum() % 6 == 0)
	{
		static bool b = false;
		if (ofGetFrameNum() % 60 == 0) b = !b;
		int o = b ? 0 : 4;//offset
		const float r = ofRandom(1);
		const int i0 = (ofRandom(1) < ofRandom(0.5)) ? 0 + o : 1 + o;
		const int i1 = (ofRandom(1) < ofRandom(0.5)) ? 2 + o : 3 + o;

		if (r < 0.1) bBangs[i0] = !bBangs[i0];
		else if (r < 0.2) bBangs[i1] = !bBangs[i1];
		else if (r < 0.3) bToggles[i0] = !bToggles[i0];
		else if (r < 0.4) bToggles[i1] = !bToggles[i1];
		else if (r < 0.5) values[i0] = ofRandom(values[i0].getMin(), values[i0].getMax());
		else if (r < 0.6) values[i1] = ofRandom(values[i1].getMin(), values[i1].getMax());
		else if (r < 0.7) values[i0] = ofRandom(values[i0].getMin(), values[i0].getMax());
		else if (r < 0.8) values[i1] = ofRandom(values[i1].getMin(), values[i1].getMax());
		else if (r < 0.9) numbers[i0] = ofRandom(numbers[i0].getMin(), numbers[i0].getMax());
		else if (r < 1) numbers[i1] = ofRandom(numbers[i1].getMin(), numbers[i1].getMax());
	}
	*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	/*
	// Make some changes or random to,
	// if all is settled correctly,
	// auto send OSC messages!

	if (key == '1') bBangs[0] = !bBangs[0];
	if (key == '2') bBangs[1] = !bBangs[1];
	if (key == '3') bBangs[2] = !bBangs[2];
	if (key == '4') bBangs[3] = !bBangs[3];

	if (key == '5') bToggles[0] = !bToggles[0];
	if (key == '6') bToggles[1] = !bToggles[1];
	if (key == '7') bToggles[2] = !bToggles[2];
	if (key == '8') bToggles[3] = !bToggles[3];

	if (key == 'q') values[0] = ofRandom(values[0].getMin(), values[0].getMax());
	if (key == 'w') values[1] = ofRandom(values[1].getMin(), values[1].getMax());
	if (key == 'e') values[2] = ofRandom(values[2].getMin(), values[2].getMax());
	if (key == 'r') values[3] = ofRandom(values[3].getMin(), values[3].getMax());

	if (key == 't') numbers[0] = ofRandom(numbers[0].getMin(), numbers[0].getMax());
	if (key == 'y') numbers[1] = ofRandom(numbers[1].getMin(), numbers[1].getMax());
	if (key == 'u') numbers[2] = ofRandom(numbers[2].getMin(), numbers[2].getMax());
	if (key == 'i') numbers[3] = ofRandom(numbers[3].getMin(), numbers[3].getMax());
	*/
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

	for (int i = 0; i < NUM_BANGS - 1; i++)
	{
		if (name == "BANG " + ofToString(i + 1) && bBangs[i])//only when true (BANG, not note-off)
		{
			ofLogNotice(__FUNCTION__) << "\tBANGS\t[" << ofToString(i + 1) << "] " << "BANG";
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Toggles(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();

	for (int i = 0; i < NUM_TOGGLES - 1; i++)
	{
		if (name == "TOGGLE " + ofToString(i + 1))
		{
			ofLogNotice(__FUNCTION__) << "\tTOGGLE\t[" << ofToString(i + 1) << "] " << (bToggles[i] ? "ON" : "OFF");
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Values(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();

	for (int i = 0; i < NUM_VALUES - 1; i++)
	{
		if (name == "VALUE " + ofToString(i + 1))
		{
			ofLogNotice(__FUNCTION__) << "\tVALUE\t[" << ofToString(i + 1) << "] " << values[i];
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed_Numbers(ofAbstractParameter& e)
{
	if (bBypass) return;

	string name = e.getName();

	for (int i = 0; i < NUM_NUMBERS - 1; i++)
	{
		if (name == "NUMBER " + ofToString(i + 1))
		{
			ofLogNotice(__FUNCTION__) << "\tNUMBER\t[" << ofToString(i + 1) << "] " << numbers[i];
			return;
		}
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofRemoveListener(params_Bangs.parameterChangedE(), this, &ofApp::Changed_Bangs);
	ofRemoveListener(params_Toggles.parameterChangedE(), this, &ofApp::Changed_Toggles);
	ofRemoveListener(params_Values.parameterChangedE(), this, &ofApp::Changed_Values);
	ofRemoveListener(params_Numbers.parameterChangedE(), this, &ofApp::Changed_Numbers);
}