#pragma once

//#define USE_local_Targets 
// More useful to use as receiver / slave
// But can be used as common ofParms callbacks!

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

	ofParameterGroup params{"Params"};
	ofParameter<bool> bang1{ "bang1", false };
	ofParameter<bool> bang2{ "bang2", false };
	ofParameter<bool> bang3{ "bang3", false };
	ofParameter<bool> bang4{ "bang4", false };
	ofParameter<bool> toggle1{ "toggle1", false };
	ofParameter<bool> toggle2{ "toggle2", false };
	ofParameter<bool> toggle3{ "toggle3", false };
	ofParameter<bool> toggle4{ "toggle4", false };
	ofParameter<float> value1{ "value1", 0, 0, 1 };
	ofParameter<float> value2{ "value2", 0, 0, 1 };
	ofParameter<float> value3{ "value3", 0, 0, 1 };
	ofParameter<float> value4{ "value4", 0, 0, 1 };
	ofParameter<int> number1{ "number1", 0, 0, 1000 };
	ofParameter<int> number2{ "number2", 0, 0, 1000 };
	ofParameter<int> number3{ "number3", 0, 0, 1000 };
	ofParameter<int> number4{ "number4", 0, 0, 1000 };

	//----

	// Local Targets 
	// aka receivers and/or senders

#ifdef USE_local_Targets
	void setupTargetsCallbacks();
	void Changed_Bangs(ofAbstractParameter& e);
	void Changed_Toggles(ofAbstractParameter& e);
	void Changed_Values(ofAbstractParameter& e);
	void Changed_Numbers(ofAbstractParameter& e);
	ofParameter<bool> bBypass{ "ByPass", false };
#endif
};
