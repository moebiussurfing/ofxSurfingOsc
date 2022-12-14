#include "PatchPipe.h"

//--------------------------------------------------------------
PatchPipeValue::PatchPipeValue() {
	bClamp = true;
}

//--------------------------------------------------------------
PatchPipeValue::~PatchPipeValue() {
}

//--------------------------------------------------------------
void PatchPipeValue::setup(string name, bool bNormalized)
{
	if (bNormalized)
	{
		inputMinRange = 0;
		inputMaxRange = 1;
		outMaxRange = 1;
		outMinRange = 0;
	}
	else
	{
		inputMinRange = 0;
		inputMaxRange = 127;
		outMinRange = 0;
		outMaxRange = 1000;
	}

	params.setName(name);
	params.add(enable.set("ENABLE", true));
	params.add(input.set("INPUT", 0, inputMinRange, inputMaxRange));
	params.add(output.set("OUTPUT", 0, outMinRange, outMaxRange));
	
	params_Extra.setName("EXTRA");
	//name hardcoded. do not rename without changing PatchingManager too!
	params_Extra.add(solo.set("SOLO", false));
	params_Extra.add(bReset.set("RESET"));
	
	params_Map.setName("MAP");
	//name hardcoded. do not rename without changing PatchingManager too!
	params_Map.add(minInput.set("IN MIN", 0, inputMinRange, inputMaxRange));
	params_Map.add(maxInput.set("IN MAX", 1, inputMinRange, inputMaxRange));
	params_Map.add(minOutput.set("OUT MIN", 0, outMinRange, outMaxRange));
	params_Map.add(maxOutput.set("OUT MAX", 1, outMinRange, outMaxRange));
	params_Extra.add(params_Map);

	params_Extra.add(bSmooth.set("SMOOTH", false));
	params_Extra.add(smoothVal.set("POWER", 0.0, 0.0f, 1));
	//params.add(bClamp.set("CLAMP", true));

	params.add(params_Extra);

	// exclude
	input.setSerializable(false);
	output.setSerializable(false);
	solo.setSerializable(false);
	bReset.setSerializable(false);

	// Callbacks

	// Channel
	listeners.push(enable.newListener([&](bool& b) {
		recalculate();
		}));
	listeners.push(solo.newListener([&](bool& b) {
		recalculate();
		}));
	listeners.push(bReset.newListener([&](void) {
		doReset();
		}));

	// Smooth
	listeners.push(bSmooth.newListener([&](bool& b) {
		recalculate();
		}));
	listeners.push(smoothVal.newListener([&](float& f) {
		recalculate();
		}));

	// Ranges and mapping

	// Input
	listeners.push(input.newListener([&](float& f) {
		recalculate();
		}));
	listeners.push(minInput.newListener([&](float& f) {
		recalculate();
		}));
	listeners.push(maxInput.newListener([&](float& f) {
		recalculate();
		}));

	// Output
	listeners.push(minOutput.newListener([&](float& f) {
		recalculate();
		}));
	listeners.push(maxOutput.newListener([&](float& f) {
		recalculate();
		}));
}

//--------------------------------------------------------------
void PatchPipeValue::update() {

	// Filter
	static float o_;
	float o = output.get();

	if (bSmooth)
	{
		float s = ofMap(smoothVal.get(), 0, 1, SMOOTH_MIN, SMOOTH_MAX);
		ofxKuValueSmooth(o, tempInput, s);

		if (o != o_) {//changed
			o_ = o;
			output.set(o);
		}
		else output.setWithoutEventNotifications(o);
	}
	else
	{
		if (tempInput != o_) {//changed
			o_ = tempInput;
			output.set(tempInput);
		}
		else output.setWithoutEventNotifications(o);
	}
}

//--------------------------------------------------------------
void PatchPipeValue::doReset() {
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__) << "output: " << ofToString(output);
	enable = true;
	minInput = 0;
	maxInput = 1;
	minOutput = 0;
	maxOutput = 1;
	bSmooth = false;
	smoothVal = 0.5;
	bClamp = true;
}

//--------------------------------------------------------------
void PatchPipeValue::recalculate()
{
	if (enable)
	{
		// 1. Map / clamp
		tempInput = ofMap(input, minInput, maxInput, minOutput, maxOutput, bClamp);

		//ofLogVerbose(__FUNCTION__) << "output: " << ofToString(output);
	}
}

//--------------------------------------------------------------
void PatchPipeValue::draw() {
}

//--------------------------------------------------------------
void PatchPipeValue::exit() {
}
