//
// Created by moebiusSurfing on 2019-11-19.
//

/*

	TODO:

	+

	++		Patching Manager: look @daan example!
	++		PatchPipeValue class:
				add extra params parallel to targets, to work as
				def params appliers to any App: using enable/disable osc msg,

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

//// On Master mode / sender, 
//// these internal targets and plots will  
//// not bee probably used/useful.
//// And can be disabled/commented!
//// ofxHistoryPlot can be then removed. 
//#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS 
//#define SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
//#define SURF_OSC__USE__RECEIVER_PATCHING_MODE // Patcher
//#define SURF_OSC__USE__RECEIVER_PLOTS // Plots. Only used on Slave mode

//#define USE_MIDI // MIDI //TODO: WIP

#define USE_IM_GUI

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
//#include "OscImGui.h"
//#include "OscImGui.cpp"
#endif

// Patch received signals to some targets (params)
// That will be processed, mapped, clamped, etc.
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
#include "PatchingManager.h"
#endif

// OSC
#include "ofxPubSubOsc.h"

//--

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
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
	//--------

#ifdef USE_IM_GUI

//private:
public:

	ofxSurfingGui* ui;

public:

	//--------------------------------------------------------------
	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
	}

public:

	//void drawImGui();
	///*
	//--------------------------------------------------------------
	void drawImGui()
	{
		if (ui == nullptr) return;

		//if (ui->BeginWindowSpecial(bGui))
		bool bIsSpecial = (ui->getModeSpecial() == IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		bool b;
		if (bIsSpecial) b = ui->BeginWindowSpecial(bGui);
		else b = ui->BeginWindow(bGui);

		if (b)
		{
			ui->AddLabelHuge("OSC");
			ui->AddSpacing();

			ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED);
			ui->AddSpacingSeparated();

			//--

			// In

			if (bUseOscIn)
			{
				if (!ui->bMinimize) ui->AddLabelBig("IN");
				ui->Add(bEnableOsc_Input, OFX_IM_TOGGLE);
				if (bEnableOsc_Input)
				{
					if (!ui->bMinimize)
					{
						string tt = "Must restart the app \nto update these settings!";
						ui->Add(OSC_InputPort, OFX_IM_DRAG);
						ui->AddTooltip(tt);
					}

					//TODO: implement
					/*
					if (getInEnablersSize() != 0) ui->AddSpacingSeparated();

					//if (!ui->bMinimize) ui->AddLabelBig("ENABLERS");
					SurfingGuiTypes s = OFX_IM_TOGGLE_SMALL;
					for (int i = 0; i < getInEnablersSize(); i++) {
						ui->Add(getInEnabler(i), s);
					}
					*/

					// Enablers
					//ui->Add(bEnable_Beat, s);
					//ui->Add(bEnable_Bang_0, s);
					//ui->Add(bEnable_Bang_1, s);
				}
			}

			//--

			// Out

			if (bUseOscOut)
			{
				if (bUseOscIn) ui->AddSpacingSeparated();

				if (!ui->bMinimize) ui->AddLabelBig("OUT");
				ui->Add(bEnableOsc_Output, OFX_IM_TOGGLE);
				if (bEnableOsc_Output)
				{
					if (!ui->bMinimize)
					{
						string tt = "Must restart the app \nto update these settings!";
						//ui->AddLabelBig(ofToString(OSC_OutputPort));
						ui->Add(OSC_OutputPort, OFX_IM_DRAG);
						ui->AddTooltip(tt);
						ui->Add(OSC_OutputIp, OFX_IM_TEXT_INPUT);
						ui->AddTooltip(tt);
					}

					if (getOutEnablersSize() != 0) ui->AddSpacingSeparated();

					//if (!ui->bMinimize) ui->AddLabelBig("ENABLERS");
					SurfingGuiTypes s = OFX_IM_TOGGLE_SMALL;
					for (int i = 0; i < getOutEnablersSize(); i++) {
						ui->Add(getOutEnabler(i), s);
					}

					//ui->Add(bEnable_Beat, s);
					//ui->Add(bEnable_Bang_0, s);
					//ui->Add(bEnable_Bang_1, s);
				}
			}

			//--

			if (!ui->bMinimize)
			{
				ui->AddSpacingSeparated();

				ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
				if (bGui_InternalAllowed) ui->Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
				ui->Add(bHelp, OFX_IM_TOGGLE_ROUNDED_MINI);
			}

			glm::vec2 p = ui->getWindowShape().getBottomLeft();
			//float w = ui->getWindowShape().getWidth();
			//w = (w / 2.f) - (boxHelp.getRectangle().getWidth() / 2.f);
			//p += glm::vec2(w, 0);
			p += glm::vec2(-11, -9);
			boxHelp.setPosition(p.x, p.y);

			//--

			if (bIsSpecial) ui->EndWindowSpecial();
			else ui->EndWindow();
		}

		//--

		ui->DrawWindowLogIfEnabled();
	}
	//*/

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

	void setName(string n) { name = n; };

private:

	string name = "";

	SurfOscModes mode = UNKNOWN;

	void setEnableOscInput(bool b) { bUseOscIn = b; };//must be called before setup. can not be modified on runtime!
	void setEnableOscOutput(bool b) { bUseOscOut = b; };//must be called before setup. can not be modified on runtime!

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

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		boxPlotsBg.setPathGlobal(path_Global);
#endif
	}

private:

	// Settings paths
	string path_AppSettings;
	string path_OscSettings;

	void setupParams(); // must be called at first before set ports and add params

	void setupGui();

	//--

	// API

public:

	//--

	// Subscribers / Publishers 
	// 
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
	}
	//--------------------------------------------------------------
	void disableGuiInternal() {
		bGui_Internal = false;
	}
	//--------------------------------------------------------------
	void setGuiInternal(bool b) {
		bGui_Internal = b;
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

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS

public:

	//--------------------------------------------------------------
	void setCustomTemplate(bool b)
	{
		bCustomTemplate = b;

		patchingManager.setCustomTemplate(b);
		//ofLogError("ofxSurfingOsc") << "Requires SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS and SURF_OSC__USE__RECEIVER_PATCHING_MODE";
	}

	//--

private:

	void setupReceiverSubscribers();

	//--

private:

	void setupReceiverTargets();

	bool bDone_SetupReceiver = false;

private:

	void exit_InternalParams();

	//--

	// Local receivers/targets

	// 8 bangs / 8 toggles / 8 float's / 8 int's
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

	bool bCustomTemplate = false;
	// when is enabled. all the plots will be drawn.
	// can't be selective drawing only tre selected!
	//--

public:

	//TODO:

	ofParameterGroup params_Targets;

	//// To help ofApp gui builder
	////----------------------------------------------------
	//ofParameterGroup getParameters_Targets()
	//{
	//	return params_Targets;
	//}

	//--

	// Gui Panel
	// for Targets
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
private:
	ofxPanel gui_Targets;
	ofParameter<glm::vec2> positionGui_Targets;
	ofParameter<bool> bGui_Targets;
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PLOTS

	// Plots

private:

	ofxSurfingBoxInteractive boxPlotsBg;

	//----------------------------------------------------
	void setVisiblePlots(bool b)
	{
		bGui_Plots = b;
	}

	// We can select which plots to draw
	ofParameter<bool> bGui_Plots;
	ofParameter<bool> bPlotsMini;

	// smooth
	ofParameter<bool> bSmoothPlots;
	ofParameter<float> smoothPlotsPower;

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
	//only used if bCustomTemplate active. bc then we dont have any picked channels! 

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
			bangCircles[_i - 1].setColor(_c);
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
			togglesCircles[_i - 1 - _start].setColor(_c);
		}

		// Values
		if (style.target == plotTarget_Value)
		{
			int _start = NUM_BANGS + NUM_TOGGLES;
			int _i = _start + style.index;//related to style
			float _min = style.range.x;
			float _max = style.range.y;
			ofColor _c = style.color;
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;
			valuesBar[_i - 1 - _start].setColor(_c);
			if (_min != -1 && _max != -1) plotsTargets[_i - 1]->setRange(_min, _max);
		}

		// Numbers
		if (style.target == plotTarget_Number)
		{
			int _start = NUM_BANGS + NUM_TOGGLES + NUM_VALUES;
			int _i = _start + style.index;//related to style
			int _min = style.range.x;
			int _max = style.range.y;
			ofColor _c = style.color;
			plotsTargets[_i - 1]->setVariableName(style.name);
			plotsTargets[_i - 1]->setColor(_c);
			plotsTargets_Visible[_i - 1] = true;
			numbersBars[_i - 1 - _start].setColor(_c);
			if (_min != -1 && _max != -1) plotsTargets[_i - 1]->setRange(_min, _max);
		}
	}

	//--

private:

	ofxHistoryPlot* addGraph(string varName, float min, float max, ofColor color, int precision, bool _smooth = false);

	vector<ofxHistoryPlot*>plotsTargets;

	ofColor colorValues, colorNumbers, colorBangs, colorToggles;

	void setupPlots();

	//--

	// Extra widgets
	CircleBeat bangCircles[NUM_BANGS];
	CircleBeat togglesCircles[NUM_TOGGLES];
	BarValue valuesBar[NUM_VALUES];
	BarValue numbersBars[NUM_VALUES];
	float _rounded = 0;

#endif

#endif

	//----

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE

public:

	PatchingManager patchingManager;

private:

	void updatePatchingManager();
	void drawPatchingManager();
	ofParameter<bool> bGui_PatchingManager;

	// API getters for the out of the patching engine

	//--

public:

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

	//--

	ofParameter<bool> bRandom;
	void doTesterRandom();

#endif
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
			ui.Add(bEnable_Beat, s);
			ui.Add(bEnable_Bang_0, s);
			ui.Add(bEnable_Bang_1, s);
		}

		ui.AddSpacingSeparated();

		ui.Add(oscHelper.bHelp, OFX_IM_TOGGLE_ROUNDED_MINI);
		ui.EndWindowSpecial();
	}
}

*/