#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("ofxSurfingOscofxSurfingOsc_SLAVE");
	ofSetBackgroundColor(48);

#ifndef USE_WINDOWAPP
	ofSetFrameRate(60);
	ofSetVerticalSync(true)
#endif

	//--

	setupControl();

	//--

#ifdef OFX_DISPLAY_VALUES
	setupDebug();
#endif
}

//--------------------------------------------------------------
void ofApp::update() {

#ifdef DEBUG_LOCAL_TARGETS
	doUpdateTargets();
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
#ifdef DEBUG_LOCAL_TARGETS
	if (key == OF_KEY_BACKSPACE)
	{
		doRandom();
	}
#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofRemoveListener(OSC_Helper.params_TARGETS.parameterChangedE(), this, &ofApp::Changed_Params_TARGETS);
}


// -->

// Useful snippets to copy/paste to your project
//--------------------------------------------------------------
void ofApp::setupControl()
{
	//--

	// MODE B:
	// For easy workflow
	// We use standard predefined template of params:
	// 8 bangs + 8 toggles + 8 values (floats) + 8 numbers (ints)
	OSC_Helper.setup(true);

	//--

	// Gui panel
	gui.setup("ofApp");
	gui.add(OSC_Helper.bGui);

	// IMPORTANT:
	// This are the raw incoming osc messages as they are: 
	// not filtered, not muted, not scaled...
	gui.add(OSC_Helper.params_TARGETS);

	auto &g = gui.getGroup(OSC_Helper.params_TARGETS.getName());
	g.getGroup("BANGS").minimize();
	g.getGroup("TOGGLES").minimize();
	g.getGroup("VALUES").minimize();
	g.getGroup("NUMBERS").minimize();

	gui.setPosition(5, 10);

	//-

	// Local callbacks for target changes
	ofAddListener(OSC_Helper.params_TARGETS.parameterChangedE(), this, &ofApp::Changed_Params_TARGETS);
}

//TODO:
// Here we can receive the incoming osc messages to our app final destinations.
// For the momment now this are the raw messages.

//--------------------------------------------------------------
void ofApp::Changed_Params_TARGETS(ofAbstractParameter &e)
{
		string _name = e.getName();
		string _padding = "\t\t";
		string _pad = _padding + _name + _padding;

		// exclude debugs
		if (_name != "exclude" //&& name != "exclude"
			)
		{
			ofLogVerbose(__FUNCTION__) << _name << " : " << e;
		}

		//-

		// bools

		if (e.type() == typeid(ofParameter<bool>).name())
		{
			ofParameter<bool> b = e.cast<bool>();
			if (b)
			{
				if (_name == "bang_" + ofToString(1))
				{
					ofLogVerbose(__FUNCTION__) << _pad << "AUBIO BEAT";
				}

				if (_name == "bang_" + ofToString(2))
				{
					ofLogVerbose(__FUNCTION__) << _pad << "AUBIO ON-SET";
				}
			}
		}

		//-

		// floats

		else if (e.type() == typeid(ofParameter<float>).name())
		{
			ofParameter<float> p = e.cast<float>();
			ofLogVerbose(__FUNCTION__) << _pad << " : " << ofToString(p, 2);

			if (_name == "values_" + ofToString(8)) // bpm
			{
				ofLogVerbose(__FUNCTION__) << _pad << "BPM " << ofToString(p, 2);
			}
		}
}

//--

// -->

// Useful snippets to copy/paste to your project

#ifdef DEBUG_LOCAL_TARGETS
//--------------------------------------------------------------
void ofApp::doRandom() {

	float t = ofGetElapsedTimef();

	for (int i = 0; i < NUM_VALUES; i++)
	{
		bangs[i] = ofNoise(t*i) > 0.5 ? true : false;
		toggles[i] = ofNoise(t*i) > 0.5 ? true : false;
		values[i] = ofNoise(t*i);
		numbers[i] = ofNoise(t*i) * 100;
	}
}

//--------------------------------------------------------------
void ofApp::doUpdateTargets() {
	for (int i = 0; i < NUM_VALUES; i++)
	{
		bangs[i] = OSC_Helper.pipersManager.oscPiperBangs[i].output.get();
		toggles[i] = OSC_Helper.pipersManager.oscPiperToggles[i].output.get();
		values[i] = OSC_Helper.pipersManager.oscPiperValues[i].output.get();
		numbers[i] = OSC_Helper.pipersManager.oscPiperNumbers[i].output.get();
	}
}
#endif

//-

#ifdef OFX_DISPLAY_VALUES
//--------------------------------------------------------------
void ofApp::setupDebug() {

	string title = "ofApp\n";//+spaces to avoid resizing on bbox
	ofxDisplayValues::setTitle(title);
	ofxDisplayValues::loadFont("assets/fonts/telegrama_render.otf", 7);
	//ofxDisplayValues::loadFont(OF_TTF_MONO, 9);
	//ofxDisplayValues::loadFont("assets/fonts/mono.ttf", 7);
	ofxDisplayValues::setPos(ofGetWidth() - 300, 500);
	ofxDisplayValues::setMomentary(false); // toggle mode
	ofxDisplayValues::setMarginBorders(20);
	ofxDisplayValues::setTabbed(true, 1);
	ofxDisplayValues::setFloatResolution(2);
	ofxDisplayValues::setShowing(true);
	ofxDisplayValues::setHelpKey(' ');

	//-

#ifdef DEBUG_LOCAL_TARGETS
	for (int i = 0; i < NUM_BANGS; i++)
	{
		ofxDisplayValues::addBool("bang_" + ofToString(i + 1), &bangs[i]);
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		ofxDisplayValues::addBool("toggle_" + ofToString(i + 1), &toggles[i]);
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		ofxDisplayValues::addFloat("values_" + ofToString(i + 1), &values[i]);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		ofxDisplayValues::addInt("number_" + ofToString(i + 1), &numbers[i]);
	}
#endif
}
#endif