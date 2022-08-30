#pragma once

#include "ofMain.h"

//#include "ofxBiquadFilter.h"
#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 3

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

	// filter / smooth
	ofxOneEuroFilter filter;
	double frequency;
	//ofxBiquadFilter1f filter;

	float tempOut;

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

