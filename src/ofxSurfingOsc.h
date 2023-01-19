//
// Created by moebiusSurfing on 2019-11-19.
//

/*

	TODO:

	+		how to log sent messages / OSC OUT ?

	+		minimize allows to hide not enabled targets
			notice that could not correlate target params
				to externally added params instead of default targets!

	PATCHING MANAGER
	+		move all classes to https://github.com/moebiussurfing/ofxPatchbayParams
	+ 		look @daan example!
	++		PatchPipeValue class:
				add extra params parallel to targets, to work as.
				def params appliers to any App: using enable/disable osc msg,
			improve linking/callbacks/getters workflow.
			fix smoothing.
			add minimize, reset behaviors.
	++		add mini class receiver with mute channels,
				plotting and signal filtering, matrix patcher with ImGui
				split in smaller classes to have a minimal to use copy/paste easy to our ofApp projects?
	++		add filter to OSC receivers? using plot? biquad lpf/hpf?

	++		change plots to auto update to reduce bangs over update..
	++		better sort plots by user.
	++ 		debug output addresses log too on address list.

	++		API: we should make a queuer to OSC messages faster:
				auto assigning the names depending of his type.
				API like: addBool(); so params are in oscHelper
					and we put getters on ofApp::update() like @dimitris GUI...
				add method add grouped of params (faster) or any param type (only bool/int/float for now)
					sequentially adding params splitting on bool/float/int types
					should use kind of map/pairs/smartPointers/msaOrderedMap to store all added params?

	+		disabler per project for not include midi?
	+		add midi out feedback for received subscribed toggles/bool:
			using ofxParamsMisiSync?
				https://github.com/NickHardeman/ofxMidiParams/issues/1#issuecomment-630559720
				add bg rectangle.
	+		add 'connected' toggle, disabler for in/out/feedback,
				enabler or mute OSC input/output. issue on ofxPubOsc
	+		get and show local IP ...

*/


//------------------------------------------------------------------------------------------------


#pragma once

#include "ofMain.h"

//-----------------------

// OPTIONAL

#define SURF_OSC__USE__TARGETS_INTERNAL_PARAMS 
#define SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI

// Plots. Requires above Targets enabled.
#define SURF_OSC__USE__TARGETS_INTERNAL_PLOTS 

// Patcher. Requires receiver / Slave mode and Targets.
#define SURF_OSC__USE__RECEIVER_PATCHING_MODE // could map senders too to normalize

//----

//#define USE_MIDI // MIDI //TODO: WIP

#define USE_IM_GUI // Replaces ofxGui!
// When disabled you must remove the ofxSurfingOsc/src/ImGui/ folder

//-----------------------

//#define USE_TEXT_FLOW // Deprecated. Currently using ImGui for logging.

#define OF_COLOR_BG_PANELS ofColor(0, 220) // duplicated

//--

//TODO:
// Duplicated into the patching manager class!
#define NUM_BANGS 8 
#define NUM_TOGGLES 8
#define NUM_VALUES 8
#define NUM_NUMBERS 8


//-----------------------

#ifdef USE_IM_GUI
#include "ofxSurfingImGui.h"
#endif

// Patch received signals to some targets (params)
// That will be processed, mapped, clamped, etc.
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
#include "PatchingManager.h"
#endif

// OSC
#include "ofxPubSubOsc.h"

//--

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
#include "CircleBeat.h"
#include "RectBeat.h"
#include "BarValue.h"
#include "ofxHistoryPlot.h"
#endif
#endif

//--

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

private:

	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);
	void keyPressed(ofKeyEventArgs& eventArgs);
	void exit();

private:

	void update(); // only required to plotting
	void draw();

	//--

	int durationBangsTrue = 20; // ms visible on the button
	//int durationBangsTrue = 2 * 1/60.f;//x frames // smaller
	//int durationBangsTrue = 100;

	//--

public:

	enum SurfOscModes
	{
		UNKNOWN = 0,
		Master,
		Slave,
		FullDuplex//TODO: WIP
	};

private:

	ofParameter<bool> bKeys;

	bool bUseIn = true;//slave
	bool bUseOut = true;//master 

public:

	//--------------------------------------------------------------
	void setup(SurfOscModes mode) { // fast setup passing the mode
		setMode(mode);
		this->setup();
	};

	void setMode(SurfOscModes mode = FullDuplex); // set mode before call setup. requires manually call setup!

	void setup();// must setMode before.

private:

	void initiate(); // must be called after setup and all target params has been added (or not)!
	void startupDelayed();

public:

	// Warning: must fix or look for a workaround
	// to allow change ports on runtime!
	void setInputPort(int p); // must be called after setupParams is called
	void setOutputPort(int p); // must be called after setupParams is called
	void setOutputIp(string ip); // must be called after setupParams is called

	//--------------------------------------------------------------
	void setModeFeedback(bool b) // Enables output mirror sending to of the incoming messages.
	{
		bModeFeedback = b;
	}

public:

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Internal;
	ofParameter<bool> bGui_Enablers;

	void setName(string n) {
		name = n;

		bGui.setName("OSC");
		ui.setLogName("LOG OSC");
	};

	string tagTarget = "T";
	string tagIn = "I";
	string tagOut = "O";

private:

	string name = "";

	SurfOscModes mode = UNKNOWN;

public:

	void setEnableOscInput(bool b) { bUseIn = b; };//must be called before setup. can not be modified on runtime!
	void setEnableOscOutput(bool b) { bUseOut = b; };//must be called before setup. can not be modified on runtime!

private:

	TextBoxWidget boxHelp;
	std::string strHelpInfo = "";
	std::string strHelpInfoExtra = "";

public:

	void setHelpInfoExtra(string s) {
		strHelpInfoExtra = s;
	}

private:

	void buildHelp();

	//----

	// Settings

	string path_Global = "ofxSurfingOsc/";

public:

	//--------------------------------------------------------------
	void setPathGlobal(string s)
	{
		ofxSurfingHelpers::CheckFolder(path_Global);
		path_Global = s;

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		boxPlotsBg.setName("PlotsOSC");
		boxPlotsBg.setPathGlobal(path_Global);
		boxPlotsBg.setup();
#endif
	}

private:

	// Settings paths
	string path_AppSettings;
	string path_OscSettings;
	string path_EnablersIn;
	string path_EnablersOut;

	void setupParams(); // must be called at first before set ports and add params

	void setupGuiInternal();

	//--------

#ifdef USE_IM_GUI

private:

	void setupImGui();
	ofxSurfingGui ui;

public:

	void drawImGui();
	// NOTE that only populate the window and their widgets. 
	// Requires ImGui instantiated externally!
	// Example
	/*
	ui.Begin();
	{
		oscHelper.drawImGui();
	}
	ui.End();
	*/

#endif

	//--------

	//--

	// API

public:

	//--

	// Subscribers / Publishers 

	// Target Linkers
	void linkBang(ofParameter<bool>& b);
	void linkToggle(ofParameter<bool>& b);
	void linkValue(ofParameter<float>& b);
	void linkNumber(ofParameter<int>& b);

	//--

	// Pass local ofParams (from ofApp) 
	// with an associated Osc address to link to.
	// will use addSender when performing a Master app / sender,
	// or addReceiver when performing our app as a Slave app / receiver.

	// Receivers
	void addReceiver_Bool(ofParameter<bool>& b, string address);
	void addReceiver_Float(ofParameter<float>& f, string address);
	void addReceiver_Int(ofParameter<int>& i, string address);

	//void addReceiver(ofAbstractParameter &e, string address);//TODO:

	// Senders
	//void addSender_Void(ofParameter<void>& b, string address);
	void addSender_Bool(ofParameter<bool>& b, string address);
	void addSender_Float(ofParameter<float>& f, string address);
	void addSender_Int(ofParameter<int>& i, string address);

	//--

	// In Enablers

	//TODO:

private:

	ofParameterGroup params_EnablerIns{ "In Enablers" };
	vector<ofParameter<bool>> bEnablerIns;
	void Changed_Params_EnablerIns(ofAbstractParameter& e);
	vector<string> addresses_bEnablerIns;

	//--

	// Out Enablers

private:

	ofParameterGroup params_EnablerOuts{ "Out Enablers" };
	vector<ofParameter<bool>> bEnablerOuts;
	void Changed_Params_EnablersOut(ofAbstractParameter& e);
	vector<string> addresses_bEnablerOuts;

public:

	// Out
	//--------------------------------------------------------------
	ofParameter<bool>& getOutEnabler(int i) {
		if (i > bEnablerOuts.size() - 1) return ofParameter<bool>();//error

		return bEnablerOuts[i];
	}

	//--------------------------------------------------------------
	int getOutEnablersSize() {
		return bEnablerOuts.size();
	}

	// In
	//--------------------------------------------------------------
	ofParameter<bool>& getInEnabler(int i) {
		if (i > bEnablerIns.size() - 1) return ofParameter<bool>();//error

		return bEnablerIns[i];
	}

	//--------------------------------------------------------------
	int getInEnablersSize() {
		return bEnablerIns.size();
	}

	//--

#ifdef USE_MIDI
private:
	void setupMidi();
#endif

	//--

	// Callbacks

private:

	void Changed_Params(ofAbstractParameter& e);

	void refreshGui();

	//--

public:

	//--------------------------------------------------------------
	void setVisible(bool b)
	{
		bGui = b;

#ifdef USE_TEXT_FLOW
		if (b && bGui_LogFlow)
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
		if (bGui && bGui_LogFlow)
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
		positionGui_Internal = glm::vec2(x, y);
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
	void Changed_Params_Osc(ofAbstractParameter& e);

private:

	ofParameterGroup params_AppSettings;

	//--

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

	vector<std::string> strs_inputAddresses;
	vector<std::string> strs_outputAddresses;

	//--

	// Log
#ifdef USE_TEXT_FLOW
	void setupTextFlow();
#endif

	//--

private:

	ofParameterGroup params_Extra;

	//--

#ifdef USE_TEXT_FLOW
	ofParameter<bool> bGui_LogFlow;
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

	// Exposed / useful for external GUI like ImGui 

public:

	ofParameter<bool> bEnableOsc;

	ofParameter<bool> bEnableOsc_Input;//not stored
	ofParameter<int> OSC_InputPort;//stored
	ofParameter<string> str_OSC_InputPort;//not stored

	ofParameter<bool> bEnableOsc_Output;//not stored
	ofParameter<int> OSC_OutputPort;//stored
	ofParameter<string> str_OSC_OutputPort;//not stored
	ofParameter<string> OSC_OutputIp;//stored

	ofParameter<bool> bModeFeedback;

	ofParameter<bool>bHelp;
	ofParameter<bool> bDebug;

	// Disables ofxGui. useful when using external gui like ImGui.
	//--------------------------------------------------------------
	void disableGuiInternalAllow() {
		bGui_InternalAllowed = false;

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.disableGuiInternalAllow();
#endif
	}
	//--------------------------------------------------------------
	void enableGuiInternalAllow() {
		bGui_InternalAllowed = true;

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.enableGuiInternalAllow();
#endif
	}
	//--------------------------------------------------------------
	void disableGuiInternal() {
		bGui_Internal = false;

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.bGui_Internal = false;
#endif
	}
	//--------------------------------------------------------------
	void setGuiInternal(bool b) {
		bGui_Internal = b;

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.bGui_Internal = b;
#endif
	}

	//--

private:

	// Gui
	ofxPanel gui_Internal;
	ofParameter<glm::vec2> positionGui_Internal;
	bool bGui_InternalAllowed = true;

	//--

private:

	//--

	//TODO:
	// to implement on connect/disconnect on runtime
	void setup_Osc();

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

	void setupReceiveLogger();

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS

public:

	//--------------------------------------------------------------
	void setCustomTemplate(bool b)//must call before setup!
	{
		bCustomTemplate = b;
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.setCustomTemplate(b);
#endif
	}

	//--

private:

	void setupTargetsAsReceivers();
	void setupTargetsAsSenders();

	//--

private:

	void setupOutputSenders();

	bool bDone_SetupTargets = false;

private:

	void exit_Targets();

	//--

	// Local Targets 
	// For receivers or senders

public:

	ofParameter<bool> bBangs[NUM_BANGS];
	ofParameter<bool> bToggles[NUM_TOGGLES];
	ofParameter<float> values[NUM_VALUES];
	ofParameter<int> numbers[NUM_NUMBERS];

private:

	// maintain bangs enable for a short time to ui display purposes
	int t_bBangs[NUM_BANGS];

	//--

private:

	// Currently used as receivers from the osc messages.
	// then will be feeded into these params.

	// 8 bangs / 8 toggles / 8 float's / 8 int's
	// for the moment this callbacks are only used to plotting purposes..

	// Bool Bangs
	string bangsNames[NUM_BANGS];
	void Changed_Tar_Bangs(ofAbstractParameter& e);

	// Bool Toggles
	string togglesNames[NUM_TOGGLES];
	void Changed_Tar_Toggles(ofAbstractParameter& e);

	// Float Values
	string valuesNames[NUM_VALUES];
	void Changed_Tar_Values(ofAbstractParameter& e);

	ofParameter<float> smoothValues[NUM_VALUES];

	// Int Values
	string numberNames[NUM_NUMBERS];
	void Changed_Tar_Numbers(ofAbstractParameter& e);

	//--

	bool bCustomTemplate = false;
	// when is disabled. all the plots will be drawn.
	// when enabled will draw only the selected!
	// default colors can be changed too.

	//--

public:

	//TODO:
	ofParameterGroup params_Targets;

	ofParameterGroup params_Bangs;
	ofParameterGroup params_Toggles;
	ofParameterGroup params_Values;
	ofParameterGroup params_Numbers;

	//// To help ofApp gui builder
	////----------------------------------------------------
	//ofParameterGroup getParameters_Targets()
	//{
	//	return params_Targets;
	//}

	//--

	// Gui Panel
	// for Targets
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
private:

	ofxPanel gui_Targets;
	ofParameter<glm::vec2> positionGui_Targets;

public:

	ofParameter<bool> bGui_Targets;
#endif

	//--

	// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS

public:

	ofParameter<bool> bGui_Plots;

	std::map<int, int> mapPlots;
	//TODO: WIP: add organize plots

private:

	ofxSurfingBoxInteractive boxPlotsBg;

	//----------------------------------------------------
	void setVisiblePlots(bool b)
	{
		bGui_Plots = b;
	}

	// We can select which plots to draw
	ofParameter<bool> bPlotsMini;

	/*
private:
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
	*/

private:

	ofParameter<bool> bGui_AllPlots;
	//only used if bCustomTemplate active. 
	//bc then we don't have any picked channels! 

	const int amount_Plots_Targets = NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS;
	vector<bool> plotsTargets_Visible; // array with bool of display state of any plot
	int amountPlotsTargetsVisible = 0;
	ofParameter<bool> bLeft{ "Left" ,false };

private:

	void updatePlots();

	void drawPlots(); // only to draw plots. gui draws is disabled and used locally on ofApp
	void drawPlots(float x, float y, float w, float h);
	void drawPlots(ofRectangle rect);

public:

	enum plotTarget
	{
		plotTarget_Bang = 0,
		plotTarget_Toggle,
		plotTarget_Value,
		plotTarget_Number
	};

	struct plotStyle
	{
		plotTarget target;
		ofColor color;
		int index;//starting at 0
		string name;
		glm::vec2 range = glm::vec2(-1, -1);//min, max
	};

	//--------------------------------------------------------------
	void setupPlotCustom(plotStyle style);

	//--

private:

	//TODO: add method to individual channels: plot + widget
	// To help to organize channels too.
	//TODO: auto updatable pointer
	//ofxHistoryPlot* addPlot(float* ptr, string varName, float min, float max, ofColor color, int precision, bool _smooth = false);

	ofxHistoryPlot* addPlot(string varName, float min, float max, ofColor color, int precision, bool _smooth = false);

	vector<ofxHistoryPlot*>plotsTargets;

	ofColor colorValues, colorNumbers, colorBangs, colorToggles;

	void setupPlots();
	void setupPlotsCustomTemplate();
	void setupPlotsColors();

	//--

	// Extra widgets
	RectBeat bangRects[NUM_BANGS];
	RectBeat togglesRects[NUM_TOGGLES];
	BarValue valuesBars[NUM_VALUES];
	BarValue numbersBars[NUM_VALUES];

	float _rounded = 0;

#endif

#endif

	//----

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE

public:

	PatchingManager patchingManager;

private:

	void updatePatchingManager();
	void drawPatchingManager();

	ofParameter<bool> bGui_PatchingManager;

	ofParameter<bool> bUse_PatchingManager{ "use patching manager", false };//must enable before setup

public:

	//--------------------------------------------------------------
	void enablePatchingManager() {
		bUse_PatchingManager = true;
	}
	/*
	//--------------------------------------------------------------
	void disablePatchingManager(){
		bUse_PatchingManager=false;
	}
	*/

	//--

public:

	// API getters for the out of the patching engine

	//TODO: add callbacks template/mode

	//--------------------------------------------------------------
	bool getOutBang(int i) const {
		if (i > NUM_BANGS - 1) return false;
		return (bool)patchingManager.pipeBangs[i].output.get();
	}
	//--------------------------------------------------------------
	bool getOutToggle(int i) const {
		if (i > NUM_TOGGLES - 1) return false;
		return (bool)patchingManager.pipeToggles[i].output.get();
	}
	//--------------------------------------------------------------
	float getOutValue(int i)const {
		if (i > NUM_VALUES - 1) return -1;
		return (float)patchingManager.pipeValues[i].output.get();
	}
	//--------------------------------------------------------------
	int getOutNumbers(int i) const {
		if (i > NUM_NUMBERS - 1) return -1;
		return (int)patchingManager.pipeNumbers[i].output.get();
	}

#endif

	//--

private:

	ofParameter<bool> bRandomize;

	void doTesterRandomPlay();
	void doTesterRandom();

#endif

	//--

public:

	// To get Osc settings 
	// bc sometimes will use external 
	// extra senders/receivers maybe.
	//--------------------------------------------------------------
	int getOutPort() const {
		return OSC_OutputPort.get();
	}
	//--------------------------------------------------------------
	string getOutIp() const {
		return OSC_OutputIp.get();
	}
	//--------------------------------------------------------------
	int getInPort() const {
		return OSC_InputPort.get();
	}

	//--------------------------------------------------------------
	bool getOutEnabled() const {
		return bEnableOsc_Output.get();
	}
	//--------------------------------------------------------------
	bool getInEnabled() const {
		return bEnableOsc_Input.get();
	}
};