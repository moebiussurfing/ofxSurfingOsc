#pragma once

//#define USE_local_Targets // More useful to use as receiver / slave

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

	ofParameter<bool> bBypass{ "ByPass", false };

	void setupTargets();

	void Changed_Bangs(ofAbstractParameter& e);
	void Changed_Toggles(ofAbstractParameter& e);
	void Changed_Values(ofAbstractParameter& e);
	void Changed_Numbers(ofAbstractParameter& e);

#endif

	//----

	ofParameterGroup params{"Params"};
	ofParameter<bool> state1{ "state1", false };
	ofParameter<bool> state2{ "state2", false };
	ofParameter<bool> state3{ "state3", false };
	ofParameter<bool> state4{ "state4", false };
	ofParameter<float> value1{ "value1", 0, 0, 1 };
	ofParameter<float> value2{ "value2", 0, 0, 1 };
	ofParameter<float> value3{ "value3", 0, 0, 1 };
	ofParameter<float> value4{ "value4", 0, 0, 1 };
};
