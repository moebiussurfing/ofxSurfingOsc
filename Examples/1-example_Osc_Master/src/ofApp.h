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

	//--

	// Gui Local 

	void setupGui();
	ofxPanel gui;
	ofParameter<bool> bBypass{ "ByPass", false };
	ofParameter<bool> bRandom{ "Randomizer", false };

	string name;

	//--

	// Remote and control

	void setupOsc();

	// OSC Input/Output and MIDI

	ofxSurfingOsc oscHelper;
	
	// NOTE:
	// If you change the OSC ports/out IP 
	// you must restart the app to apply that settings! 
	// OSC_Settings.xml will be saved!
	// Also you can edit outside the app file 
	// OSC_Settings.xml before open the app.

	//--

	// Local targets aka receivers/senders
	
	void setupTargets();

	//--

	// Senders on MASTER mode 
	// or Targets on SLAVE mode

	ofParameterGroup params_Targets;

	//--

	// Bool Bangs / Triggers

//#define NUM_BANGS 8
	ofParameterGroup params_Bangs;
	ofParameter<bool> bBangs[NUM_BANGS];
	string bangsNames[NUM_BANGS];
	void Changed_Bangs(ofAbstractParameter &e);

	//--

	// Bool Toggles / States-on/off

//#define NUM_TOGGLES 8
	ofParameterGroup params_Toggles;
	ofParameter<bool> bToggles[NUM_TOGGLES];
	string togglesNames[NUM_TOGGLES];
	void Changed_Toggles(ofAbstractParameter &e);

	//--

	// Float Values

//#define NUM_VALUES 8
	ofParameterGroup params_Values;
	ofParameter<float> values[NUM_VALUES];
	string valuesNames[NUM_VALUES];
	void Changed_Values(ofAbstractParameter &e);

	//--

	// Int Numbers

//#define NUM_NUMBERS 8
	ofParameterGroup params_Numbers;
	ofParameter<int> numbers[NUM_NUMBERS];
	string numberNames[NUM_NUMBERS];
	void Changed_Numbers(ofAbstractParameter &e);

	//----

	ofxWindowApp w;
};
