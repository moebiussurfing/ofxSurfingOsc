#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("SURFING OSC | SlavePatching");

	ofSetBackgroundColor(ofColor::blue);

	setupOsc();
}

// Useful snippets to copy/paste to your project
//--------------------------------------------------------------
void ofApp::setupOsc()
{
	oscHelper.setup(ofxSurfingOsc::Slave);
	oscHelper.setCustomTemplate(true);
	//oscHelper.setInputPort(54321);//not working b its overwritten
	oscHelper.startup();

	//--

	// Gui panel
	gui.setup("ofApp | SlavePatching");
	gui.add(oscHelper.bGui);
	gui.add(bBypass); // bypass the callbacks, not the receiving itself!

	//--

#ifdef USE_GET_INTERNAL_TARGETS 
	// IMPORTANT:
	// This are the raw incoming OSC messages as they are: 
	// not filtered, not muted, not scaled...
	gui.add(oscHelper.params_Targets);

	auto& g = gui.getGroup(oscHelper.params_Targets.getName());
	g.getGroup("BANGS").minimize();
	g.getGroup("TOGGLES").minimize();
	g.getGroup("VALUES").minimize();
	g.getGroup("NUMBERS").minimize();

	// Local callbacks for target changes
	ofAddListener(oscHelper.params_Targets.parameterChangedE(), this, &ofApp::Changed_Targets);
#endif

	//--

	gui.setPosition(5, 10);
}

//--------------------------------------------------------------
void ofApp::update()
{
	doGetInternalTargets();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	gui.draw();
}

//--

//TODO:
// Here we can receive the incoming OSC messages to our app final destinations.
// For the moment now this are the raw messages.
#ifdef USE_GET_INTERNAL_TARGETS 
//--------------------------------------------------------------
void ofApp::Changed_Targets(ofAbstractParameter& e)
{
	if (bBypass) return;

	string _name = e.getName();

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
				ofLogNotice("ofApp") << "AUBIO BEAT";
			}

			if (_name == "bang_" + ofToString(2))
			{
				ofLogNotice("ofApp") << "AUBIO ON-SET";
			}
		}
	}

	//--

	// Floats

	else if (e.type() == typeid(ofParameter<float>).name())
	{
		ofParameter<float> p = e.cast<float>();

		ofLogNotice("ofApp") << ofToString(p, 2);

		if (_name == "values_" + ofToString(8)) // bpm
		{
			ofLogNotice("ofApp") << "BPM " << ofToString(p, 2);
		}
	}
}
#endif

//--

// Useful snippets to copy/paste to your project

//--------------------------------------------------------------
void ofApp::doGetInternalTargets()
{
	if (ofGetFrameNum() % 30 != 0) return;

	string s;

	// Get the output from the patching 

	s = "BANGS  \t";
	for (int i = 0; i < NUM_BANGS; i++)
	{
		bangs[i] = oscHelper.getOutBang(i);
		s += (bangs[i] ? "+" : "-");
		s += " ";
	}
	ofLogNotice("ofApp") << s;

	s = "TOGGLES \t";
	for (int i = 0; i < NUM_BANGS; i++)
	{
		toggles[i] = oscHelper.getOutToggle(i);
		s += (toggles[i] ? "+" : "-");
		s += " ";
	}
	ofLogNotice("ofApp") << s;

	s = "VALUES \t";
	for (int i = 0; i < NUM_VALUES; i++)
	{
		values[i] = oscHelper.getOutValue(i);
		s += ofToString(values[i], 2);
		s += " ";
	}
	ofLogNotice("ofApp") << s;

	s = "NUMBERS \t";
	for (int i = 0; i < NUM_BANGS; i++)
	{
		numbers[i] = oscHelper.getOutNumbers(i);
		s += ofToString(numbers[i], 0);
		s += " ";
	}
	ofLogNotice("ofApp") << s;
}

//--------------------------------------------------------------
void ofApp::exit()
{
#ifdef USE_GET_INTERNAL_TARGETS 
	ofRemoveListener(oscHelper.params_Targets.parameterChangedE(), this, &ofApp::Changed_Targets);
#endif
}