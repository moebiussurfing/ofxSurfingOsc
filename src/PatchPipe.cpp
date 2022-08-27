#include "PatchPipe.h"

//--------------------------------------------------------------
PatchPipeValue::PatchPipeValue() {
	bClamp = true;	
}

//--------------------------------------------------------------
PatchPipeValue::~PatchPipeValue() {
}

//--------------------------------------------------------------
void PatchPipeValue::setup(string name, bool bNormalized) {
	if (bNormalized) {
		inputMinRange = 0;
		inputMaxRange = 1;
		outMaxRange = 1;
		outMinRange = 0;
	}
	else
	{
		inputMinRange = 0;
		inputMaxRange = 1;
		outMinRange = -100;
		outMaxRange = 100;
	}

	params.setName(name);
	params.add(enable.set("ENABLE", true));
	params.add(solo.set("SOLO", false));
	params.add(bReset.set("RESET"));

	params.add(input.set("INPUT", 0, inputMinRange, inputMaxRange));
	params.add(minInput.set("MIN IN", 0, inputMinRange, inputMaxRange));
	params.add(maxInput.set("MAX IN", 1, inputMinRange, inputMaxRange));

	params.add(minOutput.set("MIN OUT", 0, outMinRange, outMaxRange));
	params.add(maxOutput.set("MAX OUT", 1, outMinRange, outMaxRange));
	params.add(output.set("OUTPUT", 0, outMinRange, outMaxRange));

	params.add(enableSmooth.set("SMOOTH ENABLE", false));
	params.add(smoothVal.set("SMOOTH POWER", 0.01, 0.01, 1));
	//params.add(bClamp.set("CLAMP", true));

	//exclude
	input.setSerializable(false);
	output.setSerializable(false);
	solo.setSerializable(false);
	bReset.setSerializable(false);

	//callbacks

	//channel
	listeners.push(enable.newListener([&](bool &b) {
		recalculate();
		}));
	listeners.push(solo.newListener([&](bool&b) {
		recalculate();
		}));
	listeners.push(bReset.newListener([&](void) {
		doReset();
		}));

	//smmooth
	listeners.push(enableSmooth.newListener([&](bool&b) {
		recalculate();
		}));
	listeners.push(smoothVal.newListener([&](float &f) {
		recalculate();
		}));

	//ranges and mapping

	//input
	listeners.push(input.newListener([&](float &f) {
		recalculate();
		}));
	listeners.push(minInput.newListener([&](float &f) {
		recalculate();
		}));
	listeners.push(maxInput.newListener([&](float &f) {
		recalculate();
		}));

	//output
	listeners.push(minOutput.newListener([&](float &f) {
		recalculate();
		}));
	listeners.push(maxOutput.newListener([&](float &f) {
		recalculate();
		}));

	//startup

	//filter oneEuro
	frequency = (float)FREQ_FILTER_REF; // Hz
	double mincutoff = 1.0; // FIXME
	double beta = 1.0;      // FIXME
	double dcutoff = 1.0;   // this one should be ok
	filter.setup(frequency, mincutoff, beta, dcutoff);

	//filter biquad
	//filter.setType(OFX_BIQUAD_TYPE_LOWPASS);
	//filter.setFc(smoothVal);
	//0.1 sets the cutoff frequency to 1/10 of the framerate

	//-

	//gui.setup("PatchPipe_" + name);
	//gui.add(params);
}

//--------------------------------------------------------------
void PatchPipeValue::update() {

	//filter
	if (enableSmooth) {
		//oneEuro
		output = filter.filter(tempOut, ofGetElapsedTimef());

		//biquad
		//filter.update(tempOut);
		//output = filter.value();
	}
	else output = tempOut;
}

//--------------------------------------------------------------
void PatchPipeValue::doReset() {
	ofLogNotice(__FUNCTION__) << "output: " << ofToString(output);
	enable = true;
	minInput = 0;
	maxInput = 1;
	minOutput = 0;
	maxOutput = 1;
	enableSmooth = false;
	smoothVal = 0.5;
	bClamp = true;
}

//--------------------------------------------------------------
void PatchPipeValue::draw() {
	//gui.draw();
}

//--------------------------------------------------------------
void PatchPipeValue::exit() {
}


//--------------------------------------------------------------
void PatchPipeValue::recalculate()
{
	if (enable) {
		tempOut = ofMap(input, minInput, maxInput, minOutput, maxOutput, bClamp);

		//filter

		//oneEuro
		filter.setMinCutoff(smoothVal * frequency);
		//filter.setBeta(smoothVal);
		//filter.setDerivateCutoff(smoothVal);

		//biquad
		//filter.setFc(smoothVal);

		//ofLogNotice(__FUNCTION__) << "output: " << ofToString(output);
	}
}
