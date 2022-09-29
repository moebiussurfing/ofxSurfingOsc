#pragma once

#include "ofMain.h"

// Smoothing

//#include "ofxBiquadFilter.h"
/*
#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 3
*/

#define SMOOTH_MIN 0.35f
#define SMOOTH_MAX 0.935f

template <typename T>
void ofxKuValueSmooth(T& value, T target, float smooth) {
	value += (target - value) * (1 - smooth);
}
/*
template <typename T>
void ofxKuValueSmoothDirected(T& value, T target, float smooth0, float smooth1) {
	float smooth = (target < value) ? smooth0 : smooth1;
	ofxKuValueSmooth(value, target, smooth);
}
*/
/*
#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 5
*/

//--

class PatchPipeBool
{
public:

	//double frequency;

	PatchPipeBool::PatchPipeBool() {};
	PatchPipeBool::~PatchPipeBool() {};

	void setup(string name)
	{
		params.setName(name);
		params.add(input.set("INPUT", false));
		params.add(enable.set("ENABLE", true));
		params.add(solo.set("SOLO", false));
		params.add(output.set("OUTPUT", false));

		/*
		//params.add(bSmooth.set("SMOOTH ENABLE", false));
		//params.add(smoothVal.set("SMOOTH POWER", 0.1, 0.01, 1));
		params.add(bSmooth.set("SMOOTH", false));
		params.add(smoothVal.set("POWER", 0.1, 0.01, 1));
		*/

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

		/*
		//TODO:
		// filter oneEuro
		frequency = (float)FREQ_FILTER_REF; // Hz
		double mincutoff = 1.0; // FIXME
		double beta = 1.0;      // FIXME
		double dcutoff = 1.0;   // this one should be ok
		filter.setup(frequency, mincutoff, beta, dcutoff);
		*/
	}

	ofParameterGroup params;
	ofParameter<bool> enable;
	ofParameter<bool> solo;
	ofParameter<bool> input;
	ofParameter<bool> output;

	/*
	//TODO:
	ofxOneEuroFilter filter;
	ofParameter<bool> bSmooth;
	ofParameter<float> smoothVal;
	*/

	//-

	ofEventListeners listeners;

	//TODO:
	void update()
	{
		/*
		if (enable)
		{
			if (bSmooth)
			{
				// oneEuro
				float tempIn = (input ? 1.0 : 0.0);
				float tempInput = filter.filter(tempIn, ofGetElapsedTimef());
				if (tempInput > 0.8) output = true;
				else output = false;
			}
		}
		*/
	}

	void recalculate()
	{
		output = input;

		/*
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
		*/

		////ofLogVerbose(__FUNCTION__) << "output: " << ofToString(output);
	}
};

//----

class PatchPipeValue
{
public:

	PatchPipeValue();
	~PatchPipeValue();

private:

	float inputMinRange;
	float inputMaxRange;
	float outMinRange;
	float outMaxRange;

	/*
	// filter / smooth
	ofxOneEuroFilter filter;
	double frequency;
	//ofxBiquadFilter1f filter;
	*/

	float tempInput;

public:

	void setup(string name, bool bNormalized = true);
	void update();
	void doReset();

private:

	void draw();
	void exit();

public:

	void setClamp(bool b) {
		bClamp = b;
	}

	void setRangeOutput(float min, float max) {
		minOutput = min;
		minOutput.setMin(min);
		minOutput.setMax(max);

		maxOutput = max;
		maxOutput.setMin(min);
		maxOutput.setMax(max);

		output.setMin(min);
		output.setMax(max);
	}

	void setRangeInput(float min, float max) {
		minInput = min;
		minInput.setMin(min);
		minInput.setMax(max);

		maxInput = max;
		maxInput.setMin(min);
		maxInput.setMax(max);

		input.setMin(min);
		input.setMax(max);
	}

	ofParameterGroup params;
	ofParameterGroup params_Map;
	ofParameterGroup params_Extra;

	ofParameter<bool> enable;
	ofParameter<bool> solo;
	ofParameter<bool> bClamp;
	ofParameter<float> input;
	ofParameter<float> minInput;
	ofParameter<float> maxInput;
	ofParameter<float> minOutput;
	ofParameter<float> maxOutput;
	ofParameter<bool> bSmooth;
	ofParameter<float> smoothVal;
	ofParameter<float> output;
	ofParameter<void> bReset;

	ofEventListeners listeners;
	void recalculate();
};

