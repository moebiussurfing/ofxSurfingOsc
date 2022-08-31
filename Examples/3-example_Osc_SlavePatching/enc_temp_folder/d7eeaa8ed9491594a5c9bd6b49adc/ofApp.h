#pragma once

#include "ofMain.h"

/*

NOTE how must be the directives on "ofxSurfingOsc.h"
#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
#define SURF_OSC__USE__RECEIVER_PATCHING_MODE // Patcher
#define SURF_OSC__USE__RECEIVER_PLOTS // Plots. Only used on Slave mode

*/

//----

//#define USE_GET_INTERNAL_TARGETS // Get Targets from the add-on

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

#include "ofxWindowApp.h"

//--------------------------------------------------------------
class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void exit();

	//--

	// Gui
	ofxPanel gui;

	ofxSurfingOsc oscHelper;
	void setupOsc();
	ofParameter<bool> bBypass{ "ByPass", false };

	//--

	// Local Targets / Receivers
	// Where we receive the variables
	// from the add-on.

	bool bangs[NUM_BANGS];
	bool toggles[NUM_TOGGLES];
	float values[NUM_VALUES];
	int numbers[NUM_NUMBERS];

	void doGetInternalTargets();

	//--

#ifdef USE_GET_INTERNAL_TARGETS 

	void Changed_Targets(ofAbstractParameter &e);

#endif

	//-

	ofxWindowApp w;
};
