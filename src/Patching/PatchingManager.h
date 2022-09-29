#pragma once

#include "ofMain.h"

#include "PatchPipe.h"

#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingHelpers.h"
#include "BarValue.h"
#include "CircleBeat.h"
#include "RectBeat.h"

#define OF_COLOR_BG_PANELS ofColor(0, 200)
#define OF_COLOR_WIDGETS ofColor(200)

#define NUM_BANGS 8
#define NUM_TOGGLES 8
#define NUM_VALUES 8
#define NUM_NUMBERS 8

//-----------

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

	RectBeat previewBangs[NUM_BANGS];
	RectBeat previewToggles[NUM_TOGGLES];
	BarValue previewValues[NUM_VALUES];
	BarValue previewNumbers[NUM_NUMBERS];
};

