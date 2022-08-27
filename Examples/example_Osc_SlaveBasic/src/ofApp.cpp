#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("SURFING OSC | SLAVE");

	ofSetBackgroundColor(48);

	//--

	setupOscManager();

	//--

#ifdef OFX_DISPLAY_VALUES

	setupDebugger();

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


// * -->
// Useful snippets to copy/paste to your project
//--------------------------------------------------------------
void ofApp::setupOscManager()
{
	OscHelper.setMode(ofxSurfingOsc::Slave);
	//OscHelper.setInputPort(54321);//not working b its overwritten

	// MODE B:
	// For easy workflow
	// We use standard predefined template of params:
	// 8 bangs + 8 toggles + 8 values (floats) + 8 numbers (ints)
	OscHelper.setup(true);

	//--

	// Gui panel
	gui.setup("ofApp");
	gui.add(OscHelper.bGui);
	gui.add(bBypass);// bypass the callbacks, not the receiving itself!

	// IMPORTANT:
	// This are the raw incoming OSC messages as they are: 
	// not filtered, not muted, not scaled...
	gui.add(OscHelper.params_TARGETS);

	auto& g = gui.getGroup(OscHelper.params_TARGETS.getName());
	g.getGroup("BANGS").minimize();
	g.getGroup("TOGGLES").minimize();
	g.getGroup("VALUES").minimize();
	g.getGroup("NUMBERS").minimize();

	gui.setPosition(5, 10);

	//-

	// Local callbacks for target changes
	ofAddListener(OscHelper.params_TARGETS.parameterChangedE(), this, &ofApp::Changed_Params_TARGETS);
}

//TODO:
// Here we can receive the incoming OSC messages to our app final destinations.
// For the moment now this are the raw messages.

//--------------------------------------------------------------
void ofApp::Changed_Params_TARGETS(ofAbstractParameter& e)
{
	if (bBypass) return;

	string _name = e.getName();
	string _padding = "\t\t";
	string _pad = _padding + _name + _padding;

	ofLogNotice("ofApp") << (__FUNCTION__);
	ofLogNotice("ofApp") << _name << " : " << e;

	//--

	// Bools

	if (e.type() == typeid(ofParameter<bool>).name())
	{
		ofParameter<bool> b = e.cast<bool>();

		if (b)
		{
			if (_name == "bang_" + ofToString(1))
			{
				ofLogNotice("ofApp") << _pad << "AUBIO BEAT";
			}

			if (_name == "bang_" + ofToString(2))
			{
				ofLogNotice("ofApp") << _pad << "AUBIO ON-SET";
			}
		}
	}

	//--

	// Floats

	else if (e.type() == typeid(ofParameter<float>).name())
	{
		ofParameter<float> p = e.cast<float>();

		ofLogNotice("ofApp") << _pad << " : " << ofToString(p, 2);

		if (_name == "values_" + ofToString(8)) // bpm
		{
			ofLogNotice("ofApp") << _pad << "BPM " << ofToString(p, 2);
		}
	}
}

//--

// * -->
// Useful snippets to copy/paste to your project

#ifdef DEBUG_LOCAL_TARGETS
//--------------------------------------------------------------
void ofApp::doRandom() 
{

	float t = ofGetElapsedTimef();

	for (int i = 0; i < NUM_VALUES; i++)
	{
		bangs[i] = ofNoise(t * i) > 0.5 ? true : false;
		toggles[i] = ofNoise(t * i) > 0.5 ? true : false;
		values[i] = ofNoise(t * i);
		numbers[i] = ofNoise(t * i) * 100;
	}
}

//--------------------------------------------------------------
void ofApp::doUpdateTargets() 
{
	for (int i = 0; i < NUM_VALUES; i++)
	{
		bangs[i] = OscHelper.patchingManager.pipeBangs[i].output.get();
		toggles[i] = OscHelper.patchingManager.pipeToggles[i].output.get();
		values[i] = OscHelper.patchingManager.pipeValues[i].output.get();
		numbers[i] = OscHelper.patchingManager.pipeNumbers[i].output.get();
	}
}
#endif

//--------------------------------------------------------------
void ofApp::exit()
{
	ofRemoveListener(OscHelper.params_TARGETS.parameterChangedE(), this, &ofApp::Changed_Params_TARGETS);
}

//--

#ifdef OFX_DISPLAY_VALUES

//--------------------------------------------------------------
void ofApp::setupDebugger() {

	string title = "ofApp \n"; // + spaces to avoid resizing on bbox
	ofxDisplayValues::setTitle(title);
	ofxDisplayValues::loadFont("assets/fonts/JetBrainsMono-Bold.ttf", 7);
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