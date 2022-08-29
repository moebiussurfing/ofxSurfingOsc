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

	void setupGui();
	ofxPanel gui;

	void setupOsc();
	ofxSurfingOsc oscHelper;

	ofxWindowApp w;

	string name;
};
