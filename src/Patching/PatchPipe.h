#pragma once

#include "ofMain.h"

//--

// Smoothing

#define SMOOTH_MIN 0.600f
#define SMOOTH_MAX 0.980f

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

//--

class PatchPipeBool
{
public:

	PatchPipeBool::PatchPipeBool() {};
	PatchPipeBool::~PatchPipeBool() {};

	void setup(string name)
	{
		params.setName(name);
		params.add(input.set("INPUT", false));
		params.add(enable.set("ENABLE", true));
		params.add(solo.set("SOLO", false));
		params.add(output.set("OUTPUT", false));

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
	}

	ofParameterGroup params;
	ofParameter<bool> enable;
	ofParameter<bool> solo;
	ofParameter<bool> input;
	ofParameter<bool> output;

	//-

	ofEventListeners listeners;

	//TODO:
	void update()
	{
		if (enable) output = input;
	}

	void recalculate()
	{
		if (enable) output = input;

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

