#pragma once

#define USE_local_Targets

#include "ofMain.h"

#include "ofxSurfingOsc.h"
#include "ofxSurfingImGui.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp
{
public:

	void setup();
	void setupGui();
	void setupOsc();
	void update();
	void draw();
	void exit();

	ofxSurfingGui ui;
	ofxSurfingOsc oscHelper;

	ofParameterGroup params{ "Local Receivers" };
	ofParameter<float> bpm{ "BPM", 120, 40, 240 };
	ofParameter<float> val0{ "val0", 0, 0, 1 };
	ofParameter<float> val1{ "val1", 0, 0, 1 };
	ofParameter<float> val2{ "val2", 0, 0, 1 };
	ofParameter<bool> bBeat{ "Beat", false };
	ofParameter<bool> bBang_0{ "Bang 0", false };
	ofParameter<bool> bBang_1{ "Bang 1", false };
	ofParameter<bool> bToggle_0{ "Toggle 0", false };
	ofParameter<bool> bToggle_1{ "Toggle 1", false };

	ofxWindowApp w;
	string name;

	//----

	// Local Targets 
	// aka receivers/senders

#ifdef USE_local_Targets

	ofParameter<bool> bBypass{ "ByPass", false };

	//--

	void setupTargets();

	void Changed_Bangs(ofAbstractParameter& e);
	void Changed_Toggles(ofAbstractParameter& e);
	void Changed_Values(ofAbstractParameter& e);
	void Changed_Numbers(ofAbstractParameter& e);

	//--
	/*
	// Senders on MASTER mode 
	// or Targets on SLAVE mode

	ofParameterGroup params_Targets;

	//--

	// Bool Bangs / Triggers

	ofParameterGroup params_Bangs;
	ofParameter<bool> bBangs[NUM_BANGS];
	string bangsNames[NUM_BANGS];

	//--

	// Bool Toggles / States-on/off

	ofParameterGroup params_Toggles;
	ofParameter<bool> bToggles[NUM_TOGGLES];
	string togglesNames[NUM_TOGGLES];

	//--

	// Float Values

	ofParameterGroup params_Values;
	ofParameter<float> values[NUM_VALUES];
	string valuesNames[NUM_VALUES];

	//--

	// Int Numbers

	ofParameterGroup params_Numbers;
	ofParameter<int> numbers[NUM_NUMBERS];
	string numberNames[NUM_NUMBERS];
	*/
#endif

};
