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
	void update();
	void draw();
	void exit();

	void setupGui();
	void setupOsc();

	ofxSurfingGui ui;
	ofxSurfingOsc oscHelper;

	ofParameterGroup psettings{ "ofApp" };

	ofxWindowApp w;
	string name;

	//----

	// Local Targets 
	// aka receivers and/or senders

#ifdef USE_local_Targets

	ofParameter<bool> bBypass{ "ByPass", false }; // disable callbacks log

	void setupTargets();

	void Changed_Bangs(ofAbstractParameter& e);
	void Changed_Toggles(ofAbstractParameter& e);
	void Changed_Values(ofAbstractParameter& e);
	void Changed_Numbers(ofAbstractParameter& e);

#endif

};
