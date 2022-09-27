#pragma once

/*

TODO:


*/

//--

#include "ofMain.h"

#include "PatchPipe.h"

#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingHelpers.h"
#include "BarValue.h"
#include "CircleBeat.h"
#include "RectBeat.h"

// ranges
//float inputMinRange = 0;
//float inputMaxRange = 1;
//float outMinRange = -1000.000;
//float outMaxRange = 1000.000;

#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 5

#define OF_COLOR_BG_PANELS ofColor(0, 200)
#define OF_COLOR_WIDGETS ofColor(255, 200)

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

		//params.add(bSmooth.set("SMOOTH ENABLE", false));
		//params.add(smoothVal.set("SMOOTH POWER", 0.1, 0.01, 1));
		params.add(bSmooth.set("SMOOTH", false));
		params.add(smoothVal.set("POWER", 0.1, 0.01, 1));

		// exclude
		input.setSerializable(false);
		output.setSerializable(false);
		solo.setSerializable(false);

		listeners.push(enable.newListener([&](bool& b) {
			recalculate();
			}));

		listeners.push(solo.newListener([&](bool& b) {
			recalculate();
			}));

		listeners.push(input.newListener([&](bool& b) {
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
	ofParameter<bool> bSmooth;
	ofParameter<float> smoothVal;

	//-

	ofEventListeners listeners;

	//TODO:
	void update()
	{
		if (enable)
		{
			if (bSmooth)
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
			if (!bSmooth)
			{
				output = input;
			}
		}

		// oneEuro
		if (bSmooth)
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

	ofxSurfingBoxInteractive boxWidgets;

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

	ofxPanel gui_Internal;

	ofParameter<glm::vec2> positionGui_Internal;

	float _rounded = 0;

public:

	ofParameterGroup params_Targets;
	ofParameter<bool> bGui_PreviewWidgets;
	ofParameter<bool> bLock;

	//--------------------------------------------------------------
	void setCustomTemplate(bool b)
	{
		bCustomTemplate = b;
	}

	//--

	ofParameter<bool> bGui_Internal;
	bool bGui_InternalAllowed = true;

	// Disables ofxGui. useful when using external gui like ImGui.
	//--------------------------------------------------------------
	void disableGuiInternalAllow() {
		bGui_InternalAllowed = false;
	}
	//--------------------------------------------------------------
	void enableGuiInternalAllow() {
		bGui_InternalAllowed = true;
	}
	//--------------------------------------------------------------
	void disableGuiInternal() {
		bGui_Internal = false;
	}
	//--------------------------------------------------------------
	void setGuiInternal(bool b) {
		bGui_Internal = b;
	}

private:

	bool bCustomTemplate = false;

	//--

	string path_Settings = "PatchingManager.json";
	string path_Global = "ofxSurfingOsc/Patching";

	//--

	// Preview
	int hSlider = 100;//not used
	int szCircle = 20;//not used
	int gapx = 10;//not used
	int gapy = 10;//not used
	float gapratio = 0.01f;//->used responsive

	bool bGui_Preview = true;
	//--------------------------------------------------------------
	void setVisiblePreview(bool b)
	{
		bGui_Preview = b;
	}

	void setupPreview();
	void refreshPreview();
	void updatePreview();
	void drawPreview();

	//CircleBeat previewBangs[NUM_BANGS];
	//CircleBeat previewToggles[NUM_TOGGLES];
	RectBeat previewBangs[NUM_BANGS];
	RectBeat previewToggles[NUM_TOGGLES];
	BarValue previewValues[NUM_VALUES];
	BarValue previewNumbers[NUM_NUMBERS];

	//ofEventListeners listeners;
};

