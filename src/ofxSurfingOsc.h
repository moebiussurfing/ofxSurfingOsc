//
// Created by moebiusSurfing on 2019-11-19.
//

/*
	TODO:

	++		PatchPipeValue class:
				add extra params parallel to targets, to work as
				def params appliers to any App.: using enable/disable osc msg,
	++		add mini class receiver with mute channels,
				plotting and signal filtering, matrix patcher with ImGui
				split in smaller classes to have a minimal to use copy/paste easy to our ofApp projects?
	++		change plots to auto update to reduce bangs over update..
	++		add filter to OSC receivers? using plot? biquad lpf/hpf?
	++		sort plots by user better
	++ 		debug output addresses log too on address list
	++		API: we should make a queuer to OSC messages faster:
				auto assigning the names depending of his type.
				API like: addBool(); so params are in oscHelper
					and we put getters un ofApp::update() like @dimitris GUI...
				add method add grouped of params (faster) or any param type (only bool/int/float for now)
					sequentially adding params splitting on bool/float/int types
					should use kind of map/pairs/smartPointers/msaOrderedMap to store all added params?
	+		disabler per project for not include midi?
	+		add midi out feedback for received subscribed toggles/bool: using ofxParamsMisiSync?
				https://github.com/NickHardeman/ofxMidiParams/issues/1#issuecomment-630559720
				add bg rectangle.
	+		add 'connected' toggle, disabler for in/out/feedback,
				enabler or mute OSC input/output. issue on ofxPubOsc
	+		get and show local IP ...
	+		better gui layout/colors. add ofxGuiExtended mode?
*/

//--

/*

	// NOTES: HOW TO USE

	//// MODE A:
	//
	// 1. Easy Workflow
	// We use a standard template of params. No need to add or local targets param by param.
	// We just will update our local params on the local callback method, listening to the template params.

	oscHelper.setup(true);

	//--

	//// MODE B:

	// 2. Advanced Workflow
	// Subscribe all local target params to the controller

	oscHelper.setup(true);

	// Here we add all the local parameters (TARGETS) that we want to control from or to receive to.
	// midi assignments will be made on the addon gui by midi learn engine,
	// bc it's easier than passing all the notes/cc addresses here.

	//// Subscribe each params
	oscHelper.setModeFeedback(false); // disabled by default
	//// enabled: received OSC/MIDI input messages are auto send to the output OSC too.

*/

//------------------------------------------------------------------------------------------------

#pragma once

#include "ofMain.h"

//-----------------------

// OPTIONAL

// On Master mode / sender. these will not been used.
#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS 
#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI

//#define USE_PLOTS // Plots

//#define MODE_SLAVE_RECEIVER_PATCHING // Patcher Feature

//#define USE_MIDI // MIDI //TODO:

//-----------------------

//#define USE_TEXT_FLOW // Logging..

#define OF_COLOR_BG_PANELS ofColor(0, 220) // duplicated

//--

//TODO:
//-> duplicated into the patching manager class!
#define NUM_BANGS 8 
#define NUM_TOGGLES 8
#define NUM_VALUES 8
#define NUM_NUMBERS 8

//-----------------------

// Patch received signals to some targets (params)
// that will be processed, mapped, clamped, etc
#ifdef MODE_SLAVE_RECEIVER_PATCHING
#include "PatchingManager.h"
#endif

// OSC
#include "ofxOscSubscriber.h" // in
#include "ofxOscPublisher.h" // out

//--

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef USE_PLOTS
#include "CircleBeat.h"
#include "BarValue.h"
#include "ofxHistoryPlot.h"
#endif
#endif

#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "TextBoxWidget.h"
#include "ofxGui.h"

#ifdef USE_TEXT_FLOW
#include "ofxTextFlow.h"
#endif

//--

// MIDI

#ifdef USE_MIDI
#include "ofxMidiParams.h"
#include "ofxMidi.h"
//#define USE_MIDI_OUT
#endif

//----

class ofxSurfingOsc
{

public:

	ofxSurfingOsc();
	~ofxSurfingOsc();

	void setup();

private:

	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);
	void keyPressed(ofKeyEventArgs& eventArgs);
	void exit();

private:

	void update(); // only required to plotting
	void draw();

private:

	ofParameter<bool> bKeys;

	bool bUseOscIn = true;//slave
	bool bUseOscOut = true;//master 

public:

	enum SurfOscModes
	{
		UNKNOWN = 0,
		Master,
		Slave,
		FullDuplex
	};

	//--------------------------------------------------------------
	void setup(SurfOscModes mode) {
		setMode(mode);
		this->setup();
	};
	
	void setMode(SurfOscModes mode = FullDuplex);

	//--------------------------------------------------------------
	void setModeFeedback(bool b) // Enables output mirror sending to of the incoming messages.
	{
		bModeFeedback = b;
	}

public:

	ofParameter<bool> bGui;

private:

	SurfOscModes mode = UNKNOWN;

	void setEnableOscInput(bool b) { bUseOscIn = b; };//must be called before setup. can not be modified on runtime!
	void setEnableOscOutput(bool b) { bUseOscOut = b; };//must be called before setup. can not be modified on runtime!

private:

	TextBoxWidget boxHelp;
	std::string strHelpInfo = "";

	ofxSurfingBoxInteractive boxPlotsBg;

	//-

	// Settings

	string path_Global = "ofxSurfingOsc/";

public:

	//--------------------------------------------------------------
	void setPathGlobal(string s)
	{
		ofxSurfingHelpers::CheckFolder(path_Global);
		path_Global = s;

		boxPlotsBg.setPathGlobal(path_Global);

	}

private:

	// Settings paths
	string path_AppSettings;
	string path_OscSettings;

	void setupParams(); // must be called at first before set ports and add params

	//--

	// API

public:

	//TODO:
	void setInputPort(int p); // must be called after setupParams is called
	void setOutputPort(int p); // must be called after setupParams is called
	void setOutputIp(string ip); // must be called after setupParams is called

	void startup(); // must be called after all params has been added!

	//--

	// Subscribers / Publishers 

	// Senders
	void addSender_Bool(ofParameter<bool>& b, string address);
	void addSender_Float(ofParameter<float>& f, string address);
	void addSender_Int(ofParameter<int>& i, string address);

	// Receivers
	void addReceiver_Bool(ofParameter<bool>& b, string address);
	void addReceiver_Float(ofParameter<float>& f, string address);
	void addReceiver_Int(ofParameter<int>& i, string address);

	//void addReceiver(ofAbstractParameter &e, string address);//TODO:

	//--

#ifdef USE_MIDI
private:
	void setupMidi();
#endif

	//--

	// Callbacks

private:

	void Changed_params(ofAbstractParameter& e);

	//TODO:
	// new feature
	//store all params on a vector to allow more possibilities
	//e.g: get all subscribed parameters from ofApp and auto-create a gui panel
	//e.g: add group parameters, add all type parameters with the same function
	//vector <ofAbstractParameter> paramsAbstract;
	//void listParams();

	void refreshGui();

	//--

public:

	//--------------------------------------------------------------
	void setVisible(bool b)
	{
		bGui = b;

#ifdef USE_TEXT_FLOW
		if (b && bGui_Log)
			ofxTextFlow::setShowing(true);
		else if (!b)
			ofxTextFlow::setShowing(false);
#endif
	}
	//--------------------------------------------------------------
	void setToggleVisible()
	{
		bGui = !bGui;

#ifdef USE_TEXT_FLOW
		if (bGui && bGui_Log)
			ofxTextFlow::setShowing(true);
		else if (!bGui)
			ofxTextFlow::setShowing(false);
#endif
	}

	//--

private:

	//--------------------------------------------------------------
	void setPosition(int x, int y)
	{
		positionGuiInternal = glm::vec2(x, y);
	}
	//--------------------------------------------------------------
	glm::vec3 getPosition()
	{
		auto p = gui_Internal.getPosition();
	}
	//--------------------------------------------------------------
	float getHeight()
	{
		return gui_Internal.getHeight();
	}

private:

	ofParameter<bool> bDISABLE_CALLBACKS{ "bDISABLE_CALLBACKS", false };
	ofParameterGroup params_OscSettings;

	void Changed_params_SettingsOSC(ofAbstractParameter& e);

private:

	ofParameterGroup params_AppSettings;

#ifdef USE_MIDI
	ofxMidiParams mMidiParams;
	ofParameterGroup padParams;
	ofParameterGroup kParams;
#endif

	//--

private:

	std::string str_oscInputAddressesInfo;
	std::string str_oscOutputAddressesInfo;
	std::string str_oscAddressesInfo;

	vector <std::string> strs_inputAddresses;
	vector <std::string> strs_outputAddresses;

	//--

	// Log
#ifdef USE_TEXT_FLOW
	void setupTextFlow();
#endif

	//--

private:

	ofParameter<bool> bDebug;
	//ofParameter<bool> bGui_OscHelper;

	ofParameterGroup params_Extra;

	//--

#ifdef USE_TEXT_FLOW
	ofParameter<bool> bGui_Log;
#endif

#ifdef USE_MIDI
	ofParameter<bool> bGui_MIDI;
	ofParameter<bool> bEnableMIDI;
	ofParameter<int> MIDI_InputPort;
	ofParameter<string> str_MIDI_InputPort_name;
	ofParameterGroup params_MIDI;
#endif

	//--

	ofParameterGroup params_Osc;
	ofParameterGroup params_OscInput;
	ofParameterGroup params_OscOutput;
	ofParameterGroup params_PlotsInput;

	//--

	ofParameter<bool> bEnableOsc;

	ofParameter<bool> bEnableOsc_Input;//not stored
	ofParameter<int> OSC_InputPort;//stored
	ofParameter<string> str_OSC_InputPort;//not stored

	ofParameter<bool> bEnableOsc_Output;//not stored
	ofParameter<int> OSC_OutputPort;//stored
	ofParameter<string> str_OSC_OutputPort;//not stored
	ofParameter<string> OSC_OutputIp;//stored

	ofParameter<bool> bModeFeedback;

	//--

private:

	// Gui
	ofxPanel gui_Internal;
	ofParameter<glm::vec2> positionGuiInternal;

	//--

private:

	//--

	//TODO:
	// to implement on connect/disconnect on runtime
	void setupOsc();

	//TODO:
	void disconnectOscInput();

	//--

	//TODO:

#ifdef USE_MIDI_OUT

private:

	ofxMidiOut midiOut;
	ofParameter<int> MIDI_OutputPort;
	ofParameter<string> MIDI_OutPort_name;

public:

	void noteOut(int note, bool state);

#endif

	//----

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS

	//--

private:

	void setupReceiverSubscribers();

	//--

private:
	void setupReceiver();

	bool bDone_SetupReceiver = false;

#ifdef MODE_SLAVE_RECEIVER_PATCHING
private:
	void updateManager();
#endif

private:
	void exitManager();

	//--

////public:
//private:
//
//	//TODO: remove
//	// This is an ugly workaround to created a fixed template of params
//	void setup(bool standardTemplate);
//	// 8 bangs / 8 toggles / 8 float's / 8 int's

	//--

	// Local receivers/targets
	// for the moment this callbacks are only used to plotting purposes..

	// Bool Bangs
	ofParameterGroup params_Bangs;
	ofParameter<bool> bBangs[NUM_BANGS];
	string bangsNames[NUM_BANGS];
	void Changed_Tar_Bangs(ofAbstractParameter& e);

	// Bool Toggles
	ofParameterGroup params_Toggles;
	ofParameter<bool> bToggles[NUM_TOGGLES];
	string togglesNames[NUM_TOGGLES];
	void Changed_Tar_Toggles(ofAbstractParameter& e);

	// Float Values
	ofParameterGroup params_Values;
	ofParameter<float> values[NUM_VALUES];
	string valuesNames[NUM_VALUES];
	void Changed_Tar_Values(ofAbstractParameter& e);

	ofParameter<float> smoothValues[NUM_VALUES];

	// Int Values
	ofParameterGroup params_Numbers;
	ofParameter<int> numbers[NUM_NUMBERS];
	string numberNames[NUM_NUMBERS];
	void Changed_Tar_Numbers(ofAbstractParameter& e);

	//--

public:

	ofParameterGroup params_TARGETS;

	//// To help ofApp gui builder
	////----------------------------------------------------
	//ofParameterGroup getParameters_TARGETS()
	//{
	//	return params_TARGETS;
	//}

	//--

	// Gui Panel
	// for Targets
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
private:
	ofxPanel gui_TARGETS;
	ofParameter<bool> bGui_Targets;
#endif

	//--

#ifdef USE_PLOTS

private:

	// Plots

	//----------------------------------------------------
	void setVisiblePlots(bool b)
	{
		bGui_Plots = b;
	}

	// We can select which plots to draw
	ofParameter<bool> bGui_Plots;
	ofParameter<bool> bEnableSmoothPlots;
	ofParameter<float> smoothPlotsPower;
	ofParameter<bool> bModePlotsMini;

	enum PLOTS_MINI_POS
	{
		POS_RIGHT,
		POS_LEFT,
	};
	PLOTS_MINI_POS plotsMiniPos = POS_RIGHT;

public:

	//----------------------------------------------------
	void setPositionPlotsMiniMode(bool bLeft) {
		if (bLeft) plotsMiniPos = POS_LEFT;
		else plotsMiniPos = POS_RIGHT;
	}

private:

	ofParameter<bool> bGui_AllPlots;
	const int NUM_PLOTS = NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS;
	vector<bool> plotsSelected_TARGETS; // array with bool of display state of any plot
	int numPlostEnabled;

public:

	void updatePlots();
	void drawPlots(); // only to draw plots. gui draws is disabled and used locally on ofApp
	void drawPlots(float x, float y, float w, float h);
	void drawPlots(ofRectangle rect);

private:

	ofxHistoryPlot* addGraph(string varName, float max, ofColor color, int precision, bool _smooth = false);
	vector<ofxHistoryPlot*>plots_TARGETS;
	ofColor colorValues, colorNumbers, colorBangs, colorToggles;

	void customizePlots_TARGETS();

	float wPlotsView;
	float hPlotsView;
	float xPlotsView;
	float yPlotsView;

	//--

	// Extra widgets
	CircleBeat bangCircles[NUM_BANGS];
	CircleBeat togglesCircles[NUM_TOGGLES];
	BarValue barValues[NUM_VALUES];

#endif

#endif

	//----

#ifdef MODE_SLAVE_RECEIVER_PATCHING

public:

	void draw_PatchingManager();

	PatchingManager patchingManager;

private:

	ofParameter<bool> bGui_PatchingManager;

#endif

	//----

private:

	void buildHelp();
};
