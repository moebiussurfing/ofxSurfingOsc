#pragma once

#include "ofMain.h"

#include "ofxSurfingOsc.h"

#include "ofxGui.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp 
{
public:

	void setup();
	void draw();
	void exit();
	void keyPressed(int key);

	//-

	// Gui local 
	void setupGui();
	ofxPanel gui;
	ofParameter<bool> bBypass{ "ByPass", false };

	//-

	// Remote and control
	void setupOscManager();

	// OSC Input/Output and MIDI
	ofxSurfingOsc OscHelper;
	
	// NOTE:
	// If you change the OSC ports/out ip you must restart the app to apply. 
	// OSC_Settings.xml will be saved!
	// Also you can edit outside the app file 
	// OSC_Settings.xml before open the app.

	//-

	// Local targets aka receivers/senders
	
	void setupParamsTargets();

	//-

	ofParameterGroup params_Targets;

	// Bool bangs / triggers

#define NUM_BANGS 8
	ofParameterGroup params_Bangs;
	ofParameter<bool> bBangs[NUM_BANGS];
	string bangsNames[NUM_BANGS];
	void Changed_Params_Bangs(ofAbstractParameter &e);

	//-

	// Bool toggles / states-on/off

#define NUM_TOGGLES 8
	ofParameterGroup params_Toggles;
	ofParameter<bool> bToggles[NUM_TOGGLES];
	string togglesNames[NUM_TOGGLES];
	void Changed_params_Toggles(ofAbstractParameter &e);

	//-

	// Float values

#define NUM_VALUES 8
	ofParameterGroup params_Values;
	ofParameter<float> values[NUM_VALUES];
	string valuesNames[NUM_VALUES];
	void Changed_params_Values(ofAbstractParameter &e);

	//-

	// Int numbers

#define NUM_NUMBERS 8
	ofParameterGroup params_Numbers;
	ofParameter<int> numbers[NUM_NUMBERS];
	string numberNames[NUM_NUMBERS];
	void Changed_params_Numbers(ofAbstractParameter &e);

	//-

	ofxWindowApp w;
};
