#pragma once

// TODO: swap euro filter to biquad

#include "ofMain.h"

#include "ofxGui.h"
#include "PatchPipe.h"

#include "ofxSurfingHelpers.h"
#include "BarValue.h"
#include "CircleBeat.h"

#include "ofxInteractiveRect.h"

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

//----

class PatchingManager
{
public:

	PatchingManager();
	~PatchingManager();

	void setup(string name);
	void update();
	void draw();
	void exit();

	void doReset();

	ofEventListeners listeners;
	void recalculate();

	ofParameterGroup params_Settings;

public:

	ofxInteractiveRect boxPlotsBg = { "OSC_Manager", "ofxOSC_Manager/" };

public:

	// Patching Manager

#define NUM_BANGS 8
#define NUM_TOGGLES 8
#define NUM_VALUES 8
#define NUM_NUMBERS 8

	PatchPipeBool pipeBangs[NUM_BANGS];
	PatchPipeBool pipeToggles[NUM_TOGGLES];
	PatchPipeValue pipeValues[NUM_VALUES];
	PatchPipeValue pipeNumbers[NUM_NUMBERS];

	ofxPanel gui;

	//-

	string path_Settings = "PatchingManager.xml";
	string path_Global = "ofxSurfingOsc/Patching";

	void setPathGlobal(string s)
	{
		ofxSurfingHelpers::CheckFolder(path_Global);
		path_Global = s;
	}

	void loadParams(ofParameterGroup& g, string path);
	void saveParams(ofParameterGroup& g, string path);

	//-

	// Preview
	int hSlider = 100;//not used
	int szCircle = 20;//not used
	int gapx = 10;//not used
	int gapy = 10;//not used
	float gapratio = 0.01f;//->used responsive

	bool bVisiblePreview = true;
	void setVisiblePreview(bool b) {
		bVisiblePreview = b;
	}
	void setupPreview();
	void refreshPreviewLayout();
	void updatePreview();
	void drawPreview();

	BarValue previewValues[NUM_VALUES];
	BarValue previewNumbers[NUM_NUMBERS];
	CircleBeat previewBangs[NUM_BANGS];
	CircleBeat previewToggles[NUM_TOGGLES];

	//ofEventListeners listeners;
};

