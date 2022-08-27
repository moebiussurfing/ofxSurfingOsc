#pragma once

#include "ofMain.h"


//----

//#define DEBUG_LOCAL_TARGETS // -> debug local targets
//#define OFX_DISPLAY_VALUES // -> debug targets

#define USE_WINDOWAPP

//----

// NOTE:
// If you change the osc ports/out ip you must restart the app to apply.
// OSC_Settings.xml will be saved
// Also you can edit outside the app the file OSC_Settings.xml before open the app.

//-

#include "ofxSurfingOsc.h"

#include "ofxGui.h"

#ifdef OFX_DISPLAY_VALUES
#include "ofxDisplayValues.h"
#endif

#ifdef USE_WINDOWAPP
#include "ofxWindowApp.h"
#endif

#ifdef DEBUG_LOCAL_TARGETS
#define NUM_VALUES 8
#endif

//--------------------------------------------------------------
class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);

	//-

	ofxSurfingOsc OSC_Helper;

	//-

	void setupControl();
	void Changed_Params_TARGETS(ofAbstractParameter &e);

	// Gui
	ofxPanel gui;

	//-

#ifdef OFX_DISPLAY_VALUES
	void setupDebug();
#endif

	//-

	// Local targets 
	// Where we receive the variables from the addon.
#ifdef DEBUG_LOCAL_TARGETS
	bool bangs[NUM_VALUES];
	bool toggles[NUM_VALUES];
	float values[NUM_VALUES];
	int numbers[NUM_VALUES];
	void doRandom();
	void doUpdateTargets();
#endif

	//-

#ifdef USE_WINDOWAPP
	ofxWindowApp windowApp;
#endif
};
