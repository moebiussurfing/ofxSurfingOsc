#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	name = "OSC | SlaveBasic";
	ofSetWindowTitle(name);

	ofSetBackgroundColor(ofColor::blue);

	setupOsc();
	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupOsc()
{
	oscHelper.setGuiInternal(true);
	//oscHelper.setCustomTemplate(true);
	oscHelper.setup(ofxSurfingOsc::Slave);
	// oscHelper.startup();
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	gui.setup("ofApp " + name);
	gui.add(oscHelper.bGui); // visible toggle
	gui.add(oscHelper.bGui_Internal); // visible toggle
	gui.setPosition(10, 10);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	gui.draw();
}
