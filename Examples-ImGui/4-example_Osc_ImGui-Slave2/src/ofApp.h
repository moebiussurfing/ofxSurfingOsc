#pragma once

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
	void draw();
	void exit();

	ofxSurfingGui ui;
	ofxSurfingOsc oscHelper;

	ofParameterGroup params{ "OSC IN" };
	ofParameter<float> bpm{ "BPM", 120, 40, 240 };
	ofParameter<bool> bEnable_Beat{ "Beat", false };
	ofParameter<bool> bEnable_Bang_0{ "Bang 0", false };
	ofParameter<bool> bEnable_Bang_1{ "Bang 1", false };

	ofxWindowApp w;
	string name;
};
