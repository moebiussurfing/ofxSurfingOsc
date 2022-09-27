//
// Created by moebiusSurfing on 2019-11-19.
//

/*

	TODO:

	+		store out enabler settings
			notice that could not correlate target params to externally added params.

	++		Patching Manager: look @daan example!
	++		PatchPipeValue class:
				add extra params parallel to targets, to work as
				def params appliers to any App: using enable/disable osc msg,
			improve linking/callbacks/getters workflow
			fix smoothing

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

*/


//------------------------------------------------------------------------------------------------


#pragma once

#include "ofMain.h"

//-----------------------

// OPTIONAL

#define SURF_OSC__USE__TARGETS_INTERNAL_PARAMS 
#define SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI

// Plots. Requires above Targets enabled
#define SURF_OSC__USE__TARGETS_INTERNAL_PLOTS 

// Patcher. Requires receiver / Slave mode and Targets.
#define SURF_OSC__USE__RECEIVER_PATCHING_MODE // could map senders too to normalize

//----

//#define USE_MIDI // MIDI //TODO: WIP

#define USE_IM_GUI // Replaces ofxGui!

/*
// When disabled you must remove the ofxSurfingOsc/src/ImGui/ folder
// Example parent scope with an external instantiated ImGui
//--------------------------------------------------------------
void ofxSurfingBeatSync::setupImGui()
{
	ofLogNotice("ofxSurfingBeatSync") << (__FUNCTION__);

	ui.setName("SyncSurf");
	ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	ui.setup();

	//ui.addWindowSpecial(bGui_Main);
	//ui.addWindowSpecial(ui.bGui_GameMode);

	ui.addWindowSpecial(oscHelper.bGui);
	ui.addWindowSpecial(oscHelper.bGui_Targets);
	ui.addWindowSpecial(oscHelper.bGui_Enablers);

	ui.startup();
}
*/

//-----------------------

//#define USE_TEXT_FLOW // Logging..

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
	//--------

#ifdef USE_IM_GUI

private:

	void setupImGui();
	ofxSurfingGui ui;

public:

	void drawImGui();

#endif

	//--------

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

	void setName(string n) { name = n; };

private:

	string name = "";

	SurfOscModes mode = UNKNOWN;

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
		boxPlotsBg.setPathGlobal(path_Global);
#endif
	}

private:

	// Settings paths
	string path_AppSettings;
	string path_OscSettings;

	void setupParams(); // must be called at first before set ports and add params

	void setupGuiInternal();

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
		//ofLogError("ofxSurfingOsc") << "Requires SURF_OSC__USE__TARGETS_INTERNAL_PARAMS and SURF_OSC__USE__RECEIVER_PATCHING_MODE";
#endif
	}

	//--

private:

	void setupTargetsAsReceivers();
	void setupTargetsAsSenders();

	//--

private:

	void setupTargets();

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

	int t_bBangs[NUM_BANGS];
	//int durationBangsTrue = 2 * 1/60.f;//x frames // smaller
	int durationBangsTrue = 17; // ms visible on the button
	//int durationBangsTrue = 100;

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
	// when is enabled. all the plots will be drawn.
	// can't be selective drawing only tre selected!
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
	//only used if bCustomTemplate active. 
	//bc then we don't have any picked channels! 

	const int amount_Plots_Targets = NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS;
	vector<bool> plotsTargets_Visible; // array with bool of display state of any plot
	int amountPlotsTargetsVisible = 0;

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
		int index;//starting at 1 instead of 0
		string name;
		glm::vec2 range = glm::vec2(-1, -1);//min, max
	};

	void setupPlotCustom(plotStyle style)
	{
		// BeatCircles
		if (style.target == plotTarget_Bang)
		{
			int _start = 0;
			int _i = _start + style.index;//related to style
			ofColor _c = style.color;
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;

			bangRects[_i - 1].setColor(_c);
		}

		// Toggles
		if (style.target == plotTarget_Toggle)
		{
			int _start = NUM_BANGS;
			int _i = _start + style.index;//related to style
			ofColor _c = style.color;
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;

			togglesRects[_i - 1 - _start].setColor(_c);
		}

		// Values
		if (style.target == plotTarget_Value)
		{
			int _start = NUM_BANGS + NUM_TOGGLES;
			int _i = _start + style.index;//related to style
			float _min = style.range.x;
			float _max = style.range.y;
			if (_min == -1 && _max == -1) {//default
				_min = 0;
				_max = 1;
			}
			ofColor _c = style.color;
			//plot
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;
			plotsTargets[_i - 1]->setRange(_min, _max);

			//widget
			int _ii = _i - 1 - _start;
			valuesBars[_ii].setColor(_c);
			valuesBars[_ii].setValueMin(_min);
			valuesBars[_ii].setValueMax(_max);
		}

		// Numbers
		if (style.target == plotTarget_Number)
		{
			int _start = NUM_BANGS + NUM_TOGGLES + NUM_VALUES;
			int _i = _start + style.index;//related to style
			int _min = style.range.x;
			int _max = style.range.y;
			if (_min == -1 && _max == -1) {//default
				_min = 0;
				_max = 1;
			}
			ofColor _c = style.color;
			//plot
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;
			plotsTargets[_i - 1]->setRange(_min, _max);

			//widget
			int _ii = _i - 1 - _start;
			numbersBars[_ii].setColor(_c);
			numbersBars[_ii].setValueMin(_min);
			numbersBars[_ii].setValueMax(_max);
		}
	}

	//--

private:

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
	ofParameter<bool> bUse_PatchingManager{ "use patching manager", false };

	//--

public:

	/*
	//--------------------------------------------------------------
	void disablePatchingManager(){
		bUse_PatchingManager=false;
	}
	*/
	//--------------------------------------------------------------
	void enablePatchingManager() {
		bUse_PatchingManager = true;
	}

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


/*
*
* EXAMPLE SNIPPET
*
	oscHelper.disableGuiInternalAllow();
	oscHelper.setup(ofxSurfingOsc::Master);

	// custom
	oscHelper.bGui.setName("OSC");
	string s += "/beat";
	oscHelper.setHelpInfoExtra(s);

	// register
	string Osc_Address;
	string tag = "1/";
	Osc_Address = "/bpm";
	oscHelper.addSender_Float(bpm, Osc_Address);

	oscHelper.startup();

	//-

	// ui
	ui.addWindowSpecial(oscHelper.bGui);
	ui.Add(oscHelper.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
	if (oscHelper.bGui) drawImGui_Osc();

//--------------------------------------------------------------
void ofxSurfingBeatSync::drawImGui_Osc()
{
	if (ui.BeginWindowSpecial(oscHelper.bGui))
	{
		ui.AddLabelHuge("OSC");
		//if (!ui.bMinimize) ui.AddLabelBig("OUT");
		ui.AddSpacing();

		ui.Add(oscHelper.bEnableOsc_Output, OFX_IM_TOGGLE);
		if (oscHelper.bEnableOsc_Output) {
			if (!ui.bMinimize)
			{
				//ui.AddLabelBig(ofToString(oscHelper.OSC_OutputPort));
				ui.Add(oscHelper.OSC_OutputPort, OFX_IM_DRAG);
				ui.Add(oscHelper.OSC_OutputIp, OFX_IM_TEXT_INPUT);
			}
			ui.AddSpacingSeparated();

			if (!ui.bMinimize) ui.AddLabelBig("ENABLERS");
			SurfingGuiTypes s = OFX_IM_TOGGLE_SMALL;
			for (int i = 0; i < oscHelper.getOutEnablersSize(); i++) {
				ui.Add(oscHelper.getOutEnabler(i), s);
			}
			ui.Add(bBeat, s);
			ui.Add(bBang_0, s);
			ui.Add(bBang_1, s);
		}

		ui.AddSpacingSeparated();

		ui.Add(oscHelper.bHelp, OFX_IM_TOGGLE_ROUNDED_MINI);
		ui.EndWindowSpecial();
	}
}
*/