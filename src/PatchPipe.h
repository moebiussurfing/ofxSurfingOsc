#pragma once

#include "ofMain.h"
//#include "ofxGui.h"

//#include "ofxBiquadFilter.h"
#include "ofxOneEuroFilter.h"
#define FREQ_FILTER_REF 3

class PatchPipeValue
{
public:

	PatchPipeValue();
	~PatchPipeValue();

	//ofxBiquadFilter1f filter;
	ofxOneEuroFilter filter;
	double frequency;

	float tempOut;

	void setup(string name, bool bNormalized = true);
	void update();
	void draw();
	void exit();
	void doReset();

	void setClamp(bool b) {
		bClamp = b;
	}
	void setRangeOutput(float min, float max) {
		//minOutput = min;
		//maxOutput = max;
		output.setMin(min);
		output.setMax(max);
	}
	void setRangeInput(float min, float max) {
		//minInput = min;
		//maxInput = max;
		input.setMin(min);
		input.setMax(max);
	}

	ofParameterGroup params;
	ofParameter<bool> enable;
	ofParameter<bool> solo;
	ofParameter<bool> bClamp;
	ofParameter<float> input;
	ofParameter<float> minInput;
	ofParameter<float> maxInput;
	ofParameter<float> minOutput;
	ofParameter<float> maxOutput;
	ofParameter<bool> enableSmooth;
	ofParameter<float> smoothVal;
	ofParameter<float> output;
	ofParameter<void> bReset;

	ofEventListeners listeners;
	void recalculate();

	float inputMinRange;
	float inputMaxRange;
	float outMinRange;
	float outMaxRange;

	//ofxPanel gui;
};

