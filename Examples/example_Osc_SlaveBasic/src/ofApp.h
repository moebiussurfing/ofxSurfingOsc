#pragma once

#include "ofMain.h"

//----

#define DEBUG_LOCAL_TARGETS // -> Debug local targets
//#define OFX_DISPLAY_VALUES // -> Debug targets

//----

// NOTE:
// If you change the OSC ports/out IP 
// you must restart the app to apply that settings! 
// OSC_Settings.xml will be saved!
// Also you can edit outside the app file 
// OSC_Settings.xml before open the app.

//--

#include "ofxSurfingOsc.h"

#include "ofxGui.h"

#ifdef DEBUG_LOCAL_TARGETS
#define NUM_VALUES 8
#endif

#ifdef OFX_DISPLAY_VALUES
#include "ofxDisplayValues.h"
#endif

#include "ofxWindowApp.h"

//--------------------------------------------------------------
class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);

	//--

	ofxSurfingOsc OscHelper;

	//--

	void setupOscManager();
	void Changed_Params_TARGETS(ofAbstractParameter &e);
	ofParameter<bool> bBypass{ "ByPass", false };

	// Gui
	ofxPanel gui;

	//--

#ifdef OFX_DISPLAY_VALUES

	void setupDebugger();

#endif

	//--

	// Local targets 
	// Where we receive the variables from the add-on.

#ifdef DEBUG_LOCAL_TARGETS

	bool bangs[NUM_VALUES];
	bool toggles[NUM_VALUES];
	float values[NUM_VALUES];
	int numbers[NUM_VALUES];

	void doRandom();
	void doUpdateTargets();

#endif

	//-

	ofxWindowApp w;
};
