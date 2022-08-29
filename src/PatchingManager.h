#pragma once

/*

	Fix Number previews
	Swap euro filter to biquad

*/

//--


#include "ofMain.h"

#include "PatchPipe.h"
#include "ofxGui.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingHelpers.h"
#include "BarValue.h"
#include "CircleBeat.h"

// ranges
//float inputMinRange = 0;
//float inputMaxRange = 1;
//float outMinRange = -1000.000;
//float outMaxRange = 1000.000;

//TODO:
// think on a system to filter noisy bangs: 
// gate, hysteresis, floated .. use history plot

#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 5

#define OF_COLOR_BG_PANELS ofColor(0, 220)

//--

class PatchPipeBool
{
public:

	double frequency;

	PatchPipeBool::PatchPipeBool() {};
	PatchPipeBool::~PatchPipeBool() {};

	void setup(string name) 
	{
		params.setName(name);
		params.add(input.set("INPUT", false));
		params.add(enable.set("ENABLE", true));
		params.add(solo.set("SOLO", false));
		params.add(output.set("OUTPUT", false));

		params.add(enableSmooth.set("SMOOTH ENABLE", false));
		params.add(smoothVal.set("SMOOTH POWER", 0.1, 0.01, 1));

		// exclude
		input.setSerializable(false);
		output.setSerializable(false);
		solo.setSerializable(false);

		listeners.push(enable.newListener([&](bool &b) {
			recalculate();
		}));

		listeners.push(solo.newListener([&](bool &b) {
			recalculate();
		}));

		listeners.push(input.newListener([&](bool &b) {
			recalculate();
		}));

		//TODO:
		// filter oneEuro
		frequency = (float)FREQ_FILTER_REF; // Hz
		double mincutoff = 1.0; // FIXME
		double beta = 1.0;      // FIXME
		double dcutoff = 1.0;   // this one should be ok
		filter.setup(frequency, mincutoff, beta, dcutoff);
	}

	ofParameterGroup params;
	ofParameter<bool> enable;
	ofParameter<bool> solo;
	ofParameter<bool> input;
	ofParameter<bool> output;

	//TODO:
	ofxOneEuroFilter filter;
	ofParameter<bool> enableSmooth;
	ofParameter<float> smoothVal;

	//-

	ofEventListeners listeners;

	//TODO:
	void update() 
	{
		if (enable) 
		{
			if (enableSmooth) 
			{
				// oneEuro
				float tempIn = (input ? 1.0 : 0.0);
				float tempOut = filter.filter(tempIn, ofGetElapsedTimef());
				if (tempOut > 0.8) output = true;
				else output = false;
			}
		}
	}

	void recalculate() 
	{
		if (enable)
		{
			if (!enableSmooth)
			{
				output = input;
			}
		}

		// oneEuro
		if (enableSmooth)
		{
			filter.setMinCutoff(smoothVal * frequency);
			//filter.setBeta(smoothVal);
			//filter.setDerivateCutoff(smoothVal);
		}

		////ofLogNotice(__FUNCTION__) << "output: " << ofToString(output);
	}
};


//-----------


#define NUM_BANGS 8
#define NUM_TOGGLES 8
#define NUM_VALUES 8
#define NUM_NUMBERS 8

class PatchingManager
{
public:

	PatchingManager();
	~PatchingManager();

	void setup(string name);
	void update();
	void draw();

	void setPositionPreview(glm::vec2 position);

	ofxSurfingBoxInteractive boxPlotsBg;

	PatchPipeBool pipeBangs[NUM_BANGS];
	PatchPipeBool pipeToggles[NUM_TOGGLES];
	PatchPipeValue pipeValues[NUM_VALUES];
	PatchPipeValue pipeNumbers[NUM_NUMBERS];

	//--------------------------------------------------------------
	void setPathGlobal(string s)
	{
		path_Global = s;
		ofxSurfingHelpers::CheckFolder(path_Global);
	}

	//--

private:

	void exit();

	void recalculate();
	void doReset();

	ofEventListeners listeners;

	ofParameterGroup params_Settings;
	ofParameterGroup params_Targets;

	ofxPanel gui_Internal;
	
	ofParameter<glm::vec2> positionGui_Internal;

	//ofParameter<bool> bGui;
	ofParameter<bool> bLock;

private:

	//--

	string path_Settings = "PatchingManager";
	string path_Global = "ofxSurfingOsc/Patching";
	
	bool bCustomTemplate = false;

	//--

	// Preview
	int hSlider = 100;//not used
	int szCircle = 20;//not used
	int gapx = 10;//not used
	int gapy = 10;//not used
	float gapratio = 0.01f;//->used responsive

	bool bVisiblePreview = true;
	//--------------------------------------------------------------
	void setVisiblePreview(bool b)
	{
		bVisiblePreview = b;
	}

	void setupPreview();
	void refreshPreview();
	void updatePreview();
	void drawPreview();

	BarValue previewValues[NUM_VALUES];
	BarValue previewNumbers[NUM_NUMBERS];
	CircleBeat previewBangs[NUM_BANGS];
	CircleBeat previewToggles[NUM_TOGGLES];

	//ofEventListeners listeners;
};

