//
//Created by moebiussurfing on 2019-11-19.
//

#include "ofxSurfingOsc.h"

//--------------------------------------------------------------
ofxSurfingOsc::ofxSurfingOsc()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	ofSetLogLevel(OF_LOG_NOTICE);

	ofAddListener(ofEvents().update, this, &ofxSurfingOsc::update);
	ofAddListener(ofEvents().draw, this, &ofxSurfingOsc::draw);
	ofAddListener(ofEvents().keyPressed, this, &ofxSurfingOsc::keyPressed);

	// Settings paths
	path_Global = "ofxSurfingOsc";
	setPathGlobal(path_Global);

	path_AppSettings = "App_Settings.xml";
	path_OscSettings = "Osc_Settings.xml";

	path_EnablersIn = "EnablersIn_Settings.xml";
	path_EnablersOut = "EnablersOut_Settings.xml";

	//--

	//TODO:
	// Debug info
	// should improve performance building list once
	// to show subscribed OSC addresses
	strs_inputAddresses.clear();
	strs_outputAddresses.clear();

	str_oscInputAddressesInfo = "";
	str_oscOutputAddressesInfo = "";

	//--

#ifdef USE_IM_GUI
	bGui_Internal = false;
	bGui_InternalAllowed = false;
#endif
}

//--------------------------------------------------------------
ofxSurfingOsc::~ofxSurfingOsc()
{
	ofLogWarning("ofxSurfingOsc") << (__FUNCTION__);

	ofRemoveListener(ofEvents().update, this, &ofxSurfingOsc::update);
	ofRemoveListener(ofEvents().draw, this, &ofxSurfingOsc::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingOsc::keyPressed);

	exit();
}

//----------------------------------------------------
void ofxSurfingOsc::setMode(SurfOscModes mode)
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	switch (mode)
	{
	case ofxSurfingOsc::UNKNOWN:
		setEnableOscInput(false);
		setEnableOscOutput(false);
		ofLogError("ofxSurfingOsc") << "Must define a working mode correctly. UNKNOWN Mode!";
		ofLogError("ofxSurfingOsc") << "e.g: .setMode(ofxSurfingOsc::Master);";
		name = "UNKNOWN";
		break;

	case ofxSurfingOsc::Master:
		setEnableOscInput(false);
		setEnableOscOutput(true);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode Master";
		name = "MASTER";
		break;

	case ofxSurfingOsc::Slave:
		setEnableOscInput(true);
		setEnableOscOutput(false);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode Slave";
		name = "SLAVE";
		break;

	case ofxSurfingOsc::FullDuplex:
		setEnableOscInput(true);
		setEnableOscOutput(true);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode FullDuplex";
		name = "FULLDUPLEX";
		break;
	}
}

//----------------------------------------------------
void ofxSurfingOsc::setup()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	bDISABLE_CALLBACKS = false;

	// Gui theme
	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	//--

#ifdef USE_TEXT_FLOW
	setupTextFlow();
#endif

	//--

	// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	{
		boxPlotsBg.setName("Plots");
		boxPlotsBg.setPathGlobal(path_Global);
		boxPlotsBg.bEdit.setName("EDIT PLOTS");

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.boxWidgets.bEdit.makeReferenceTo(boxPlotsBg.bEdit);
#endif
		boxPlotsBg.setup();

		// constraint sizes
		float f = 0.85f;
		glm::vec2 shapeMin(f * 240, f * 480);
		boxPlotsBg.setRectConstraintMin(shapeMin);
		glm::vec2 shapeMax(ofGetWidth(), ofGetHeight());
		boxPlotsBg.setRectConstraintMax(shapeMax);
	}
#endif

	//--

	// Help box
	boxHelp.bGui.setName("Help");
	boxHelp.setPath(path_Global);
	boxHelp.setup();

	//--

	setupParams();

	setup_Osc();

	//--

	initiate();
}

//----------------------------------------------------
void ofxSurfingOsc::buildHelp()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	strHelpInfo = "";
	strHelpInfo += "SURFING OSC \n";
	strHelpInfo += "\n";
	strHelpInfo += name;
	strHelpInfo += "\n\n";

	if (bUseIn)
	{
		strHelpInfo += "INPUT \n";
		strHelpInfo += "PORT      " + str_OSC_InputPort.get() + "\n";
		strHelpInfo += "\n";
	}

	if (bUseOut) {
		strHelpInfo += "OUTPUT \n";
		strHelpInfo += "PORT      " + str_OSC_OutputPort.get() + "\n";
		strHelpInfo += "IP        " + OSC_OutputIp.get() + "\n";
		strHelpInfo += "\n";
	}

	strHelpInfo += str_oscAddressesInfo;
	strHelpInfo += strHelpInfoExtra;

	boxHelp.setText(strHelpInfo);
}

//----------------------------------------------------
void ofxSurfingOsc::disconnectOscInput()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	ofxUnsubscribeOsc();
}

//----------------------------------------------------
void ofxSurfingOsc::setup_Osc()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//--

	// OSC Settings

	params_OscSettings.clear();
	params_OscSettings.setName("OSC_SETTINGS");

	if (bUseIn)
	{
		params_OscSettings.add(bEnableOsc_Input);
		params_OscSettings.add(OSC_InputPort);
	}

	if (bUseOut)
	{
		params_OscSettings.add(bEnableOsc_Output);
		params_OscSettings.add(OSC_OutputPort);
		params_OscSettings.add(OSC_OutputIp);

		//TODO:
		//params_OscSettings.add(params_EnablerOuts);
	}

	//--

	ofAddListener(params_OscSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_Osc);
}

//--

#ifdef USE_MIDI

//----------------------------------------------------
void ofxSurfingOsc::setupMidi()
{
	mMidiParams.setup();//TODO: splitted-out funtion to avoid crash...
	mMidiParams.setLockedGui(false);
	//mMidiParams.bShowGuiInternal = true;

	// MIDI in
	MIDI_InputPort.setMax(mMidiParams.getMidi().getNumInPorts() - 1);
	mMidiParams.connect(0, false);//default port. false to avoid auto-reconnect

	//-

#ifdef USE_MIDI_OUT
	//print the available output ports to the console
	midiOut.listOutPorts();

	//connect
	midiOut.openPort(0);//by number
	//midiOut.openPort("IAC Driver Pure Data In"); //by name
	//midiOut.openVirtualPort("ofxMidiOut"); //open a virtual port
#endif
}

#endif

//----------------------------------------------------
void ofxSurfingOsc::initiate()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//--

	// MIDI

#ifdef USE_MIDI
	mMidiParams.load(path_Global + "/" + "midi-params.xml");
	//here we store the midi assignments
#endif

	//--

	// Startup
	//TODO: force hardcoded
	// Default settings
	{
		// Remote machines

		OSC_OutputIp = "127.0.0.1";			// Local
		///OSC_OutputIp = "192.168.0.135";	// iPad
		///OSC_OutputIp = "192.168.0.122";	// macBook Air

		//--

		if (bUseOut)
		{
			bEnableOsc_Output = true;
			OSC_InputPort = 12345;
			OSC_OutputPort = 54321;
		}
		else if (bUseIn)//force
		{
			bEnableOsc_Input = true;
			OSC_InputPort = 54321;
			OSC_OutputPort = 12345;
		}
	}

	//--

	// Load File Settings
	ofxSurfingHelpers::loadGroup(params_OscSettings, path_Global + "/" + path_OscSettings);

	// Fix workaround
	if (bUseIn)
	{
		bEnableOsc_Input = bEnableOsc_Input;
	}

	//--

	// Initialize receiver mode when Input is enabled!
	//if(0)

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS

	// Setup param Targets
	setupOutputSenders();

	//--

	if (bUseIn)
	{
		// Subscribe all parameters
		// to OSC incoming messages
		setupTargetsAsReceivers();

		// Log incoming messages
		setupReceiveLogger();
	}

	//--

	if (bUseOut)
	{
		// Subscribe all parameters
		// to OSC incoming messages
		setupTargetsAsSenders();
	}

#endif

	//----

	// Load Settings

	//ofxSurfingHelpers::loadGroup(params_AppSettings, path_Global + "/" + path_AppSettings);

	ofxSurfingHelpers::loadGroup(params_EnablerIns, path_Global + "/" + path_EnablersIn);
	ofxSurfingHelpers::loadGroup(params_EnablerOuts, path_Global + "/" + path_EnablersOut);
}

//----------------------------------------------------
void ofxSurfingOsc::startupDelayed()
{

	//----

	// Customize special plots
	// Template is for my ofxSoundAnalyzer add-on

	if (bCustomTemplate)
	{
		setupPlotsCustomTemplate();
	}

	//----

	// Load Settings

	ofxSurfingHelpers::loadGroup(params_AppSettings, path_Global + "/" + path_AppSettings);

	//--
	// 
	// Debug Info
	{
		// Debug and show all subscribed addresses
		// Process all subscribed addresses

		if (bUseIn)
		{
			str_oscInputAddressesInfo = "ADDRESSES \n";
			for (auto a : strs_inputAddresses)
			{
				str_oscInputAddressesInfo += "\n";
				str_oscInputAddressesInfo += a;
			}

			str_oscAddressesInfo += str_oscInputAddressesInfo;

			ofAddListener(params_EnablerIns.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_EnablerIns);
		}

		if (bUseOut)
		{
			str_oscOutputAddressesInfo = "ADDRESSES \n";
			for (auto a : strs_outputAddresses)
			{
				str_oscOutputAddressesInfo += "\n";
				str_oscOutputAddressesInfo += a;
			}

			str_oscAddressesInfo += str_oscOutputAddressesInfo;

			ofAddListener(params_EnablerOuts.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_EnablersOut);
		}

		/*
		//TODO:
		// should draw both..
		// show one of the two
		if (strs_outputAddresses.size() >= strs_inputAddresses.size())
		{
			str_oscAddressesInfo = str_oscOutputAddressesInfo;
		}
		else
		{
			str_oscAddressesInfo = str_oscInputAddressesInfo;
		}
		*/

		//if (bUseIn) str_oscAddressesInfo += str_oscInputAddressesInfo;
		//if (bUseOut) str_oscAddressesInfo += str_oscOutputAddressesInfo;

		//--

		// Gui Internal
		if (bGui_InternalAllowed /*&& bGui_Internal*/)
		{
			refreshGui();
		}

		//--

		//TODO: fix
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		// Count enabled plots to enable hide the all not used
		amountPlotsTargetsVisible = 0;
		for (int _i = 0; _i < plotsTargets_Visible.size(); _i++)
		{
			// count how many are enabled
			if (plotsTargets_Visible[_i]) amountPlotsTargetsVisible++;
		}
#endif

		//--

		buildHelp();
	}

	//--

	// Queue enabler toggles into gui
	if (bGui_InternalAllowed)
	{
		// Out Enablers
		if (bUseOut)
		{
			auto& g = gui_Internal.getGroup(params_Osc.getName());
			auto& gout = g.getGroup(params_OscOutput.getName());
			gout.add(params_EnablerOuts);
			gout.minimize();
			if (getOutEnablersSize() != 0) gout.getGroup(params_EnablerOuts.getName()).minimize();
		}

		//TODO:
		// In Enablers
		if (bUseIn)
		{
			auto& g = gui_Internal.getGroup(params_Osc.getName());
			auto& gin = g.getGroup(params_OscInput.getName());
			gin.add(params_EnablerIns);
			gin.minimize();
			if (getInEnablersSize() != 0) gin.getGroup(params_EnablerIns.getName()).minimize();
		}
	}
}

#ifdef USE_IM_GUI
//----------------------------------------------------
void ofxSurfingOsc::setupImGui()
{
	string name = "ofxSurfingOsc";
	ui.setName(name);
	ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	ui.setup();

	ui.addWindowSpecial(bGui);
	ui.addWindowSpecial(bGui_Targets);
	ui.addWindowSpecial(bGui_Enablers);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUse_PatchingManager)
		ui.addWindowSpecial(bGui_PatchingManager);
#endif

	ui.startup();

	// Log
	ui.AddLogTag(SurfingLog::tagData{ tagTarget, ofColor::turquoise });
	ui.AddLogTag(SurfingLog::tagData{ tagIn, ofColor::yellow });
	ui.AddLogTag(SurfingLog::tagData{ tagOut, ofColor::blue });
}
#endif

//----------------------------------------------------
void ofxSurfingOsc::setupGuiInternal()
{
	if (!bGui_InternalAllowed || !bGui_Internal) return;

	gui_Internal.setup(bGui.getName());

	// OSC
	gui_Internal.add(params_Osc);

	// MIDI
#ifdef USE_MIDI
	gui_Internal.add(bGui_MIDI);
	gui_Internal.add(params_MIDI);
#endif

	// Plots
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	if (bUseIn) gui_Internal.add(params_PlotsInput);
#endif

	// Extra
	gui_Internal.add(params_Extra);

	// Position
	positionGui_Internal.set("Gui Position Internal",
		glm::vec2(250, 10),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight()));
	gui_Internal.setPosition(positionGui_Internal.get().x, positionGui_Internal.get().y);
}

//----------------------------------------------------
void ofxSurfingOsc::setupParams()
{
	bGui.set("OSC", true);

	bGui_Internal.set("Internal", true);
	bGui_Enablers.set("ENABLERS", true);
	bEnableOsc.set("Enable OSC", true);
	bKeys.set("Keys", true);
	bDebug.set("Debug", false);

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
	bRandomize.set("Random Timer", false);
#endif

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
	//bGui_Targets.set("OSC TARGETS", true);
	bGui_Targets.set("TARGETS", true);
#endif

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	bGui_Plots.set("PLOTS", true);
	if (bCustomTemplate) bGui_AllPlots.set("All", false);
	bPlotsMini.set("Mini", true);
#endif
#endif

#ifdef USE_MIDI
	bGui_MIDI.set("Show MIDI", false);
#endif

#ifdef USE_TEXT_FLOW
	bGui_LogFlow.set("LogFlow", false);
#endif

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUseIn)
	{
		if (bUse_PatchingManager) bGui_PatchingManager.set("PATCHER", false);
	}
#endif

	//--

	// Extra

	params_Extra.setName("Extra");

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
	if (bDone_SetupTargets) params_Extra.add(bGui_Targets);
#endif

	params_Extra.add(bKeys);
	params_Extra.add(boxHelp.bGui);
	params_Extra.add(bDebug);
	//params_Extra.add(bGui_Internal);

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
	params_Extra.add(bRandomize);
#endif

	//params_Extra.add(bSmoothPlots);
	//params_Extra.add(smoothPlotsPower);

	bHelp.makeReferenceTo(boxHelp.bGui);

	//--

	// OSC

	if (bUseIn)
	{
		bEnableOsc_Input.set("IN ENABLE", false);
		OSC_InputPort.set("IN PORT", 12345, 0, 99999);
		str_OSC_InputPort.set("IN PORT", "");//out
		str_OSC_InputPort = ofToString(OSC_InputPort);

		bModeFeedback.set("Feedback", false);
	}

	if (bUseOut)
	{
		bEnableOsc_Output.set("OUT ENABLE", true);//TODO: cant be disabled once publishers are added..
		OSC_OutputPort.set("OUT PORT", 54321, 0, 99999);
		str_OSC_OutputPort.set("OUT PORT", "");//out
		str_OSC_OutputPort = ofToString(OSC_InputPort);
		OSC_OutputIp.set("OUT IP", "127.0.0.1");
	}

	if (bUseIn)
	{
		params_OscInput.setName("INPUT");
		params_OscInput.add(bEnableOsc_Input);
		params_OscInput.add(str_OSC_InputPort);
		params_OscInput.add(bModeFeedback);
	}

	if (bUseOut)
	{
		params_OscOutput.setName("OUTPUT");
		params_OscOutput.add(bEnableOsc_Output);
		params_OscOutput.add(OSC_OutputIp);
		params_OscOutput.add(str_OSC_OutputPort);
	}

	// Exclude
	str_OSC_InputPort.setSerializable(false);
	str_OSC_OutputPort.setSerializable(false);

	if (bUseIn) str_OSC_InputPort = ofToString(OSC_InputPort);
	if (bUseOut) str_OSC_OutputPort = ofToString(OSC_OutputPort);

	//--

	params_Osc.setName("OSC");
	if (bUseIn) params_Osc.add(params_OscInput);
	if (bUseOut) params_Osc.add(params_OscOutput);

#ifdef USE_TEXT_FLOW
	params_Osc.add(bGui_LogFlow);
#endif

	//--

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	{
		params_PlotsInput.setName("PLOTS INPUT");
		params_PlotsInput.add(bGui_Plots);
		params_PlotsInput.add(bPlotsMini);
		params_PlotsInput.add(bLeft);
		if (bCustomTemplate) params_PlotsInput.add(bGui_AllPlots);
	}
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUseIn) {
		if (bUse_PatchingManager) {
			params_Osc.add(bGui_PatchingManager);
			params_Osc.add(bUse_PatchingManager);
		}
	}
#endif

	//--

	// MIDI

#ifdef USE_MIDI
	bEnableMIDI.set("Enable MIDI", true);
	params_MIDI.setName("MIDI");

	if (bUseIn) {
		MIDI_InputPort.set("MIDI INPUT PORT", 0, 0, 10);
		str_MIDI_InputPort_name.set("", "");
	}

#ifdef USE_MIDI_OUT
	if (bUseOut) {
		MIDI_OutputPort.set("MIDI OUTPUT PORT", 0, 0, 10);
		MIDI_OutPort_name.set("", "");
	}
#endif

	params_MIDI.add(bEnableMIDI);
	if (bUseIn) {
		params_MIDI.add(MIDI_InputPort);
		params_MIDI.add(str_MIDI_InputPort_name);
		str_MIDI_InputPort_name.setSerializable(false);
	}
#ifdef USE_MIDI_OUT
	if (bUseOut) {
		params_MIDI.add(MIDI_OutputPort);
		params_MIDI.add(MIDI_OutPort_name);
		MIDI_OutPort_name.setSerializable(false);
	}
#endif

#endif

	//--

	// MIDI

#ifdef USE_MIDI
	setupMidi();
#endif

	//--

	// Gui

	setupGuiInternal();

#ifdef USE_IM_GUI
	setupImGui();
#endif

	//--

	// Params Settings

	// Not into gui: just for store/recall and callbacks

	params_AppSettings.setName("Osc_AppSettings");
	params_AppSettings.add(bGui_Internal);
	params_AppSettings.add(positionGui_Internal);
	params_AppSettings.add(bGui_Enablers);
	params_AppSettings.add(bGui_Targets);

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
	params_AppSettings.add(positionGui_Targets);
#endif

#ifdef USE_TEXT_FLOW
	params_AppSettings.add(bGui_LogFlow);
#endif

	params_AppSettings.add(params_Extra);

	//--

	// MIDI

#ifdef USE_MIDI
	params_AppSettings.add(bGui_MIDI);
	params_AppSettings.add(bEnableMIDI);
	params_AppSettings.add(MIDI_InputPort);
#ifdef USE_MIDI_OUT
	params_AppSettings.add(MIDI_OutputPort);
#endif

#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUseIn) {
		if (bUse_PatchingManager) {
			params_AppSettings.add(bGui_PatchingManager);
			params_AppSettings.add(bUse_PatchingManager);
		}
	}

#endif

	//--

	// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	params_AppSettings.add(params_PlotsInput);
#endif

	//--

	// Callbacks

	ofAddListener(params_AppSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
	ofAddListener(params_Osc.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);

#ifdef USE_MIDI
	ofAddListener(params_MIDI.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
#endif

	//--

	//// Load file with midi assignments to params
	//mMidiParams.load(path_Global + "/" + "midi-params.xml");
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Params_Osc(ofAbstractParameter& e)
{
	string name = e.getName();

	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

	// Settings

	if (bUseIn)
	{
		if (name == bEnableOsc_Input.getName())
		{
			ofxSetOscSubscriberActive(OSC_InputPort, bEnableOsc_Input);

			buildHelp();

			return;
		}

		if (name == OSC_InputPort.getName())
		{
			str_OSC_InputPort = ofToString(OSC_InputPort);

			buildHelp();

			return;
		}

		//if (name == bEnableOsc_Input.getName())
		//{
		//	if (!bEnableOsc_Output) {
		//		disconnectOscInput();
		//	}
		//	else {
		//		setup_Osc();
		//	}
		//}
	}

	//--

	if (bUseOut)
	{
		if (name == bEnableOsc_Output.getName())
		{
			auto addresses = ofxGetPublishedAddresses(OSC_OutputIp.get(), OSC_OutputPort);
			ofLogNotice("ofxSurfingOsc::ofxGetPublishedAddresses") << ofToString(addresses);

			for (auto& a : addresses)
			{
				if (bEnableOsc_Output.get())
					ofxGetOscPublisher(OSC_OutputIp.get(), OSC_OutputPort.get()).resumePublish(a);
				else
					ofxGetOscPublisher(OSC_OutputIp.get(), OSC_OutputPort.get()).stopPublishTemporary(a);
			}

			buildHelp();

			return;
		}

		if (name == OSC_OutputPort.getName())
		{
			str_OSC_OutputPort = ofToString(OSC_OutputPort);

			buildHelp();

			return;
		}

		if (name == OSC_OutputIp.getName())
		{
			buildHelp();

			return;
		}
	}

	//else if (name == OSC_OutputIp.getName())
	//{
	//	setOutputIp(OSC_OutputIp);//not required, it's the same param, there's no int
	//}

	//--

	// OSC

	//--

	/*
	// Input

	if (bUseIn)
	{
		if (name == str_OSC_InputPort.getName())
		{
			//TODO: should verify that correspond to a valid port structure!
			int p = ofToInt(str_OSC_InputPort);

			setInputPort(p);

			return;
		}
	}

	//--

	// Out

	if (bUseOut)
	{
		if (name == str_OSC_OutputPort.getName())
		{
			int p = ofToInt(str_OSC_OutputPort);
			setOutputPort(p);

			return;
		}

		//else if (name == OSC_OutputIp.getName())
		//{
		//	setOutputIp(OSC_OutputIp);
		// // not required, it's the same param, there's no int
		//}
	}
	*/
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Params_EnablersOut(ofAbstractParameter& e)
{
	if (!bUseOut) return;
	if (!bEnableOsc_Output.get()) return;

	string name = e.getName();

	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

	for (int i = 0; i < bEnablerOuts.size(); i++)
	{
		if (name == bEnablerOuts[i].getName())
		{
			string a = addresses_bEnablerOuts[i];

			if (bEnablerOuts[i].get()) ofxGetOscPublisher(OSC_OutputIp.get(), OSC_OutputPort.get()).resumePublish(a);
			else ofxGetOscPublisher(OSC_OutputIp.get(), OSC_OutputPort.get()).stopPublishTemporary(a);

			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Params_EnablerIns(ofAbstractParameter& e)
{
	if (!bUseIn) return;
	if (!bEnableOsc_Input.get()) return;

	string name = e.getName();

	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

	for (int i = 0; i < bEnablerIns.size(); i++)
	{
		if (name == bEnablerIns[i].getName())
		{
			string a = addresses_bEnablerIns[i];

			bool b = bEnablerIns[i].get();
			//ofxGetOscSubscriber(OSC_InputPort.get()).setEnable(b);

			if (b)
			{
				//ofxGetOscSubscriber(OSC_InputPort.get()).subscribe(a);

				//ofxSubscribeOsc(OSC_InputPort.get(), a);
				//ofxUnsubscribeOSC(int port, const string & address);
				//ofxGetOscSubscriberActive
			}
			else
			{
				//ofxUnsubscribeOsc(OSC_InputPort.get(), a);
				//ofxGetOscSubscriber(OSC_InputPort.get()).unsubscribe(a);

				//ofxUnsubscribeOsc(OSC_InputPort.get(), a);
			}
			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::update()
{
	if (ofGetFrameNum() == 1)
	{
		startupDelayed();
	}

	//----

#ifdef USE_MIDI
	if (ofGetFrameNum() == 1) bEnableMIDI = bEnableMIDI; // workaround
#endif

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUseIn) updatePatchingManager();
#endif

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	updatePlots();
#endif

	//--

	// When activated bDebug, 
	// Log input subscribed addresses
	//if (ofGetFrameNum() % 600 == 0)
	static bool _bDebug = false;// = !bDebug;
	if (bDebug != _bDebug) // changed
	{
		_bDebug = bDebug;
		if (bDebug)
		{
			// In

			if (bUseIn)
			{
#ifdef USE_IM_GUI
				ui.AddToLog("OSC IN \n", tagIn);
				ui.AddToLog("addresses \n\n", tagIn);
#endif	
				auto ss = ofxGetSubscribedAddresses(OSC_InputPort);
#ifdef USE_IM_GUI
				for (size_t i = 0; i < ss.size(); i++)
				{
					ui.AddToLog(ss[i], tagIn);
				}
#endif	
				auto si = "OSC IN \n\n" + ofToString(ss) + "\n\n";
				ofLogNotice("ofxSurfingOsc") << "OSC IN  Addresses: " << ofToString(si);
				/*
#ifdef USE_IM_GUI
				ui.AddToLog(ofToString(si),tagIn);
#endif
				*/
			}

			//--

			// Out

			if (bUseOut)
			{
				auto so = "OSC OUT \n\n" + ofToString(ofxGetPublishedAddresses(OSC_OutputIp.get(), OSC_OutputPort)) + "\n\n";
				ofLogNotice("ofxSurfingOsc") << "OSC OUT Addresses: " << ofToString(so);

#ifdef USE_IM_GUI
				ui.AddToLog(ofToString(so), tagOut);
#endif
			}
		}
	}

	//--

	// Random
	//if (bDebug)
	{
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
		//if (bUseIn)
		{
			doTesterRandomPlay();
		}
#endif
	}

	//--

	//TODO:
	// Handle bangs
	// Timed off. 
	// Required bc ofxPubSubOsc sends one state per frame.
	auto t = ofGetElapsedTimeMillis();
	for (int i = 0; i < NUM_BANGS; i++)
	{
		if (bBangs[i] && (t - t_bBangs[i] > durationBangsTrue))
		{
			bBangs[i] = false;

			int _start = 0;

			//plotsTargets[_start + i]->update(false);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::update(ofEventArgs& args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingOsc::draw(ofEventArgs& args)
{
	draw();
}

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
//--------------------------------------------------------------
void ofxSurfingOsc::drawPatchingManager()
{
	patchingManager.draw();
}
#endif

//--------------------------------------------------------------
void ofxSurfingOsc::draw()
{
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	if (bGui_Plots)
	{
		drawPlots();
	}
#endif
#endif

	//--

	if (!bGui) return;

	//--

	// GUI Targets

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS 
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
	if (bGui_InternalAllowed && bGui_Internal)
		if (bGui_Targets && bDone_SetupTargets)
		{
			gui_Targets.draw();
		}
#endif 
#endif

	//--

	// GUI Internal
	if (bGui_InternalAllowed && bGui_Internal) gui_Internal.draw();

	//--

	//TODO:
	// could improve not updating on every frame. 
	// just when gui panel position changed with a callback

	// MIDI

#ifdef USE_MIDI
	if (bGui_MIDI && bEnableMIDI)
	{
		//int pad = 5;
		//mMidiParams.setPosition(p.x + w + pad, p.y); // bottom
		////mMidiParams.setPosition(p.x + w + rectDebug.width + pad, p.y); // right

		mMidiParams.draw();
		//mMidiParams.setVisible(true);
		//mMidiParams.setVisible(true);
	}
#endif

	//--

	// OSC Log

#ifdef USE_TEXT_FLOW
	glm::vec2 posOSC;
	float widthOSC;
	if (bGui_MIDI)
	{
		auto mp = mMidiParams.getPosition();
		auto mw = mMidiParams.getWidth();
		posOSC = mp;
		widthOSC = mw;
	}
	else
	{
		posOSC = glm::vec2(p.x + w + 5, p.y);
		widthOSC = 0;
	}
	ofxTextFlow::setPosition(posOSC.x + widthOSC + 5, posOSC.y);
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUseIn) if (bGui_PatchingManager && bUse_PatchingManager) drawPatchingManager();
#endif

	//--

	boxHelp.draw();
}

#ifdef USE_IM_GUI
//--------------------------------------------------------------
void ofxSurfingOsc::drawImGui()
{
	if (!bGui) return;

	bool bIsSpecial = (ui.getModeSpecial() == IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);

	//--

	// Main
	{
		bool b;
		if (bIsSpecial) b = ui.BeginWindowSpecial(bGui);
		else b = ui.BeginWindow(bGui);
		if (b)
		{
			ui.AddLabelHuge("OSC");
			ui.AddSpacing();

			ui.Add(ui.bMinimize, OFX_IM_TOGGLE_ROUNDED);
			ui.AddSpacingSeparated();

			if (bUseOut) {
				ui.Add(bGui_Enablers, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				ui.AddTooltip("Enable which addresses\nyou want to be sent \nor to be ignored.");
			}
			ui.Add(bGui_Targets, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			string s = "All the controls will send OSC messages.\n";
			s += "(If they are not in use!)\n";
			s += "Can be used for testing purposes.\n\n";
			s += "WIP \nSource commands can be patched\n";
			s += "to map different destinations!\n\n";
			s += "Example: \nFrom Enablers to different TARGETS.\n";
			s += "Allowing different layouts.\n";
			ui.AddTooltip(s);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
			if (bUse_PatchingManager) {
				ui.AddSpacingSeparated();
				if (!ui.bMinimize)
					ui.AddLabelBig("PATCHING MANAGER");
				ui.Add(bGui_PatchingManager, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				if (!ui.bMinimize)
					if (patchingManager.bGui_InternalAllowed) ui.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
			}
#endif
			ui.AddSpacingSeparated();

			if (ui.bMinimize) {
				ui.Add(bGui_Plots, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			}

			ui.Add(ui.bLog, OFX_IM_TOGGLE_ROUNDED_SMALL);

			ui.AddSpacingSeparated();

			//--

			// In

			if (bUseIn)
			{
				if (!ui.bMinimize) ui.AddLabelBig("IN");
				ui.Add(bEnableOsc_Input, OFX_IM_TOGGLE);
				if (bEnableOsc_Input)
				{
					if (!ui.bMinimize)
					{
						string tt = "Must restart the app \nto update these settings!";
						ui.Add(OSC_InputPort, OFX_IM_DRAG);
						ui.AddTooltip(tt);
					}

					//TODO: implement
					/*
					*
					if (!ui.bMinimize) {

						if (ui.BeginTree("ENABLERS", false))
						{
							if (getInEnablersSize() != 0) ui.AddSpacingSeparated();

							//if (!ui.bMinimize) ui.AddLabelBig("ENABLERS");
							SurfingGuiTypes s = OFX_IM_TOGGLE_SMALL;
							for (int i = 0; i < getInEnablersSize(); i++) {
								ui.Add(getInEnabler(i), s);
							}
							ui.EndTree();
						}
					}
					*/
				}
			}

			//--

			// Out

			if (bUseOut)
			{
				if (bUseIn) ui.AddSpacingSeparated();

				if (!ui.bMinimize) ui.AddLabelBig("OUT");
				ui.Add(bEnableOsc_Output, OFX_IM_TOGGLE);
				if (bEnableOsc_Output)
				{
					if (!ui.bMinimize)
					{
						string tt = "Must restart the app \nto update these settings!";
						//ui.AddLabelBig(ofToString(OSC_OutputPort));
						ui.Add(OSC_OutputPort, OFX_IM_DRAG);
						ui.AddTooltip(tt);
						ui.Add(OSC_OutputIp, OFX_IM_TEXT_INPUT);
						ui.AddTooltip(tt);
					}
				}
			}

			//--

			// Plots

			if (!ui.bMinimize)
			{
				ui.AddSpacingSeparated();

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
				ui.AddLabelBig("PLOTS");
				ui.Indent();
				ui.Add(bGui_Plots, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				if (bGui_Plots) {
					ui.Add(bPlotsMini, OFX_IM_TOGGLE_ROUNDED_SMALL);
					if (bCustomTemplate) ui.Add(bGui_AllPlots, OFX_IM_TOGGLE_ROUNDED_MINI);
					ui.Indent();
					ui.Add(bLeft, OFX_IM_TOGGLE_ROUNDED_MINI);
					ui.Unindent();
					ui.Unindent();
				}

				ui.AddSpacingSeparated();
#endif					
				if (ui.BeginTree("TESTER")) {
					if (ui.AddButton("Random", OFX_IM_BUTTON_SMALL)) {
						for (size_t i = 0; i < 16; i++)
						{
							doTesterRandom();
						}
					};
					ui.Add(bRandomize, OFX_IM_TOGGLE_SMALL_BORDER_BLINK);
					ui.Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
					if (bGui_InternalAllowed) ui.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
					ui.EndTree();
				}

				ui.AddSpacingSeparated();

				ui.Add(bHelp, OFX_IM_TOGGLE_ROUNDED_MINI);
				string s = "List all OSC Addresses\nfor Input and/or Output";
				ui.AddTooltip(s);
			}

			bool block = false;
			if (block) {
				glm::vec2 p = ui.getWindowShape().getBottomLeft();
				//float w = ui.getWindowShape().getWidth();
				//w = (w / 2.f) - (boxHelp.getRectangle().getWidth() / 2.f);
				//p += glm::vec2(w, 0);
				p += glm::vec2(-11, -9);
				boxHelp.setPosition(p.x, p.y);
			}

			//--

			if (bIsSpecial) ui.EndWindowSpecial();
			else ui.EndWindow();
		}
	}

	//--

	// Targets
	{
		static bool bdone = false;
		if (!bdone) {
			bdone = true;
			ui.ClearStyles();
			ui.AddStyleGroupForBools(params_Bangs, OFX_IM_TOGGLE_SMALL);
			ui.AddStyleGroupForBools(params_Toggles, OFX_IM_TOGGLE_SMALL);
		}

		bool b;
		bool bExist = ui.isThereSpecialWindowFor(bGui_Targets);
		if (bIsSpecial && bExist) b = ui.BeginWindowSpecial(bGui_Targets);
		else b = ui.BeginWindow(bGui_Targets);
		if (b)
		{
			//if (bUseIn)
			{
				ui.AddGroup(params_Targets);
			}

			//--

			if (bIsSpecial) ui.EndWindowSpecial();
			else ui.EndWindow();
		}
	}

	//--

	// Enablers (only out!)

	if (bUseOut)
	{
		bool b;
		bool bExist = ui.isThereSpecialWindowFor(bGui_Enablers);
		if (bIsSpecial && bExist) b = ui.BeginWindowSpecial(bGui_Enablers);
		else b = ui.BeginWindow(bGui_Enablers);
		if (b)
		{
			ui.AddLabelBig("OSC OUT");

			{
				//if (getOutEnablersSize() != 0) ui.AddSpacingSeparated();
				//if (ui.BeginTree("ENABLERS", false))
				{
					SurfingGuiTypes s = OFX_IM_CHECKBOX;

					if (ui.AddButton("All", OFX_IM_BUTTON, 2, true))
					{
						for (int i = 0; i < bEnablerOuts.size(); i++) bEnablerOuts[i] = true;
					}
					if (ui.AddButton("None", OFX_IM_BUTTON, 2))
					{
						for (int i = 0; i < bEnablerOuts.size(); i++) bEnablerOuts[i] = false;
					}

					for (int i = 0; i < getOutEnablersSize(); i++)
					{
						ui.Add(getOutEnabler(i), s);
					}

					//WIP
					/*
					for (int i = 0; i < getOutEnablersSize(); i++)
					{
						static bool b0 = false;
						static bool b1 = false;
						static bool b2 = false;
						static bool b3 = false;

						if (i == 0) b0 = ui.BeginTree("BANGS");
						else if (i == NUM_BANGS) b1 = ui.BeginTree("TOGGLES");
						else if (i == NUM_BANGS + NUM_TOGGLES) b2 = ui.BeginTree("VALUES");
						else if (i == NUM_BANGS + NUM_TOGGLES + NUM_VALUES) b3 = ui.BeginTree("NUMBERS");

						if (i < NUM_BANGS && !b0) continue;
						else if (i < NUM_BANGS + NUM_TOGGLES && !b1) continue;
						else if (i < NUM_BANGS + NUM_TOGGLES + NUM_VALUES && !b2) continue;
						else if (i < NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS && !b3) continue;

						ui.Add(getOutEnabler(i), s);

						if (i == NUM_BANGS - 1 && b0) ui.EndTree();
						else if (i == NUM_BANGS + NUM_TOGGLES - 1 && b1) ui.EndTree();
						else if (i == NUM_BANGS + NUM_TOGGLES + NUM_VALUES - 1 && b2) ui.EndTree();
						else if (i == NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS - 1 && b3) ui.EndTree();
					}
					*/

					//ui.EndTree();
				}
			}

			if (bIsSpecial) ui.EndWindowSpecial();
			else ui.EndWindow();
		}
	}

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (bUse_PatchingManager) {
		if (ui.BeginWindowSpecial(bGui_PatchingManager))
		{
			// custom styles
			{
				static bool bDone = false;
				static bool bMinimize_;
				if (ui.bMinimize != bMinimize_) {
					bMinimize_ = ui.bMinimize;
					bDone = false;
				}
				if (!bDone) {
					bDone = true;

					ui.UpdateStyle("SMOOTH", OFX_IM_TOGGLE_ROUNDED_MINI);
					ui.UpdateStyle("ENABLE", bMinimize_ ? OFX_IM_HIDDEN : OFX_IM_TOGGLE_SMALL_BORDER_BLINK);
					ui.UpdateStyleGroup("MAP", bMinimize_ ? SurfingGuiGroupStyle_Hidden : SurfingGuiGroupStyle_None);
					ui.UpdateStyleGroup("EXTRA", bMinimize_ ? SurfingGuiGroupStyle_NoHeader : SurfingGuiGroupStyle_None);
					ui.UpdateStyle("SOLO", bMinimize_ ? OFX_IM_HIDDEN : OFX_IM_TOGGLE_SMALL_BORDER_BLINK);
					ui.UpdateStyle("INPUT", bMinimize_ ? OFX_IM_HIDDEN : OFX_IM_DEFAULT);
				}
			}

			ui.Add(patchingManager.bGui_PreviewWidgets, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			ui.AddSpacingSeparated();

			ui.AddGroup(patchingManager.params_Targets, SurfingGuiGroupStyle_Collapsed);
			ui.EndWindowSpecial();
		}
	}
#endif

	//--

	ui.DrawWindowLogIfEnabled();
}
#endif

//--------------------------------------------------------------
void ofxSurfingOsc::exit()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	ofRemoveListener(params_AppSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
	ofRemoveListener(params_Osc.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
	ofRemoveListener(params_OscSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_Osc);

#ifdef USE_MIDI
	ofRemoveListener(params_MIDI.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
#endif

	if (bGui_InternalAllowed && bGui_Internal)
	{
		positionGui_Internal = gui_Internal.getPosition();

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
		positionGui_Targets = gui_Targets.getPosition();
#endif
	}

	ofxSurfingHelpers::saveGroup(params_AppSettings, path_Global + "/" + path_AppSettings);
	ofxSurfingHelpers::saveGroup(params_OscSettings, path_Global + "/" + path_OscSettings);

	ofxSurfingHelpers::saveGroup(params_EnablerIns, path_Global + "/" + path_EnablersIn);
	ofxSurfingHelpers::saveGroup(params_EnablerOuts, path_Global + "/" + path_EnablersOut);

	//-

#ifdef USE_MIDI_OUT
	midiOut.closePort();
#endif

	if (bUseOut)
	{
		ofRemoveListener(params_EnablerOuts.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_EnablersOut);
	}

	if (bUseIn)
	{
		ofRemoveListener(params_EnablerIns.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_EnablerIns);
	}

	//--

//#ifdef USE_IM_GUI
//	ui.exit();
//#endif

}

//----

// Note that some settings can not be changed without restart the app!

//--------------------------------------------------------------
void ofxSurfingOsc::setInputPort(int p)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << " : " << p;
	OSC_InputPort = p;
	str_OSC_InputPort = ofToString(OSC_InputPort);
}

//--------------------------------------------------------------
void ofxSurfingOsc::setOutputPort(int p)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << " : " << p;
	OSC_OutputPort = p;
	str_OSC_OutputPort = ofToString(OSC_OutputPort);
}

//--------------------------------------------------------------
void ofxSurfingOsc::setOutputIp(string ip)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << " : " << ip;
	OSC_OutputIp = ip;
}

//----

// Receivers (Osc In)

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Bool(ofParameter<bool>& p, string address)
{
	if (!bUseIn) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Input is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	//paramsAbstract.push_back(b);

#ifdef USE_MIDI
	mMidiParams.add(b);

	//TODO:
	// store names and note nums of all added params
	// add method to get note of a param name
	//mMidiParams.getId
#endif

	strs_inputAddresses.push_back(address);

	ofxSubscribeOsc(OSC_InputPort, address, p);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerIns.push_back(b);
	params_EnablerIns.add(b);
	addresses_bEnablerIns.push_back(address);

	//--

	//TODO: WIP
	// Feedback
	if (bModeFeedback && bUseOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Int(ofParameter<int>& p, string address)
{
	if (!bUseIn)
	{
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Input is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

#ifdef USE_MIDI
	mMidiParams.add(i);
#endif

	strs_inputAddresses.push_back(address);

	ofxSubscribeOsc(OSC_InputPort, address, p);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerIns.push_back(b);
	params_EnablerIns.add(b);
	addresses_bEnablerIns.push_back(address);

	//--

	//TODO: WIP
	// Feedback
	if (bModeFeedback && bUseOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Float(ofParameter<float>& p, string address)
{
	if (!bUseIn) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Input is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

#ifdef USE_MIDI
	mMidiParams.add(p);
#endif

	strs_inputAddresses.push_back(address);

	OSC_InputPort = 54321;
	ofxSubscribeOsc(OSC_InputPort, address, p);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerIns.push_back(b);
	params_EnablerIns.add(b);
	addresses_bEnablerIns.push_back(address);

	//--

	//TODO: WIP
	// Feedback
	if (bModeFeedback && bUseOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//----

// Senders (Osc Out)

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Bool(ofParameter<bool>& p, string address)
{
	if (!bUseOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	//https://github.com/2bbb/ofxPubSubOsc/issues/38
	/*
	auto condition = [&]() {
		static bool b = false;//previous
		bool bChanged = false;
		if (b != p.get()) {//changed
			b = p.get();
			bChanged = true;
		}
		return bChanged && p.get();
	};
	ofxPublishOscIf(condition, OSC_OutputIp, OSC_OutputPort, address, p);
	*/

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);

	strs_outputAddresses.push_back(address);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerOuts.push_back(b);
	params_EnablerOuts.add(b);
	addresses_bEnablerOuts.push_back(address);
}

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Float(ofParameter<float>& p, string address)
{
	if (!bUseOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	//TODO:
	bool bAlways = false;

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p, !bAlways);
	strs_outputAddresses.push_back(address);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerOuts.push_back(b);
	params_EnablerOuts.add(b);
	addresses_bEnablerOuts.push_back(address);
}

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Int(ofParameter<int>& p, string address)
{
	if (!bUseOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	strs_outputAddresses.push_back(address);

	// Enabler
	ofParameter<bool>b{ p.getName(), true };
	bEnablerOuts.push_back(b);
	params_EnablerOuts.add(b);
	addresses_bEnablerOuts.push_back(address);
}

//----

// Callbacks

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Params(ofAbstractParameter& e)
{
	string name = e.getName();

	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

	//--

#ifdef USE_TEXT_FLOW
	if (0) ofxTextFlow::addText(name + " : " + ofToString(e));
#endif

	//--

	if (false) {}

#ifdef USE_TEXT_FLOW
	else if (name == bGui_LogFlow.getName())
	{
		ofxTextFlow::setShowing(bGui_LogFlow);

		return;
	}
#endif

	//--

	//TODO:
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	else if (name == bGui_PatchingManager.getName())
	{
		if (bUseIn)
		{
			if (bUse_PatchingManager) {
				if (bGui_PatchingManager) {
				}
			}
		}

		return;
	}
#endif

	//--

	// MIDI

#ifdef USE_MIDI

	// MIDI in

	else if (name == MIDI_InputPort.getName())
	{
		ofLogNotice(__FUNCTION__) << "MIDI INPUT PORT: " << (MIDI_InputPort);
		mMidiParams.disconnect();
		MIDI_InputPort.setMax(mMidiParams.getMidi().getNumInPorts() - 1);

		mMidiParams.connect(MIDI_InputPort, false);

		str_MIDI_InputPort_name = mMidiParams.getMidi().getName();

		return;
	}
	else if (name == bEnableMIDI.getName())
	{
		ofLogNotice(__FUNCTION__) << "ENABLE MIDI: " << (bEnableMIDI ? "ON" : "OFF");
		if (bEnableMIDI)
		{
			mMidiParams.connect(MIDI_InputPort, false);
		}
		else
		{
			mMidiParams.disconnect();
		}

		return;
	}
	else if (name == bGui_MIDI.getName())
	{
		refreshGui();

		return;
	}

	//--

	// MIDI out

#ifdef USE_MIDI_OUT

	else if (name == MIDI_OutputPort.getName())
	{
		ofLogNotice(__FUNCTION__) << "MIDI OUT PORT: " << (MIDI_OutputPort);
		midiOut.listOutPorts();

		int size = midiOut.getNumOutPorts();
		MIDI_OutputPort.setMax(size - 1);

		// connect
		bool b = midiOut.openPort(MIDI_OutputPort);

		if (b)
		{
			MIDI_OutPort_name = midiOut.getName();
			auto strs = ofSplitString(MIDI_OutPort_name, "Wavetable");//shortname
			MIDI_OutPort_name = strs[0];
		}
		else
		{
			MIDI_OutPort_name = "ERROR";
		}

		return;
	}

#endif

#endif

	//--

#ifdef USE_TEXT_FLOW
	else if (name == bGui_LogFlow.getName())
	{
		// workflow
		if (bGui_LogFlow) bGui_OscHelper = true;
		//if (!bGui_OscHelper && bGui_LogFlow) bGui_OscHelper = true;

		return;
	}
#endif

	//--

	else if (name == positionGui_Internal.getName())
	{
		if (bGui_InternalAllowed && bGui_Internal)
			gui_Internal.setPosition(positionGui_Internal.get().x, positionGui_Internal.get().y);

		return;
	}

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
	else if (name == positionGui_Targets.getName())
	{
		if (bGui_InternalAllowed && bGui_Internal)
			gui_Targets.setPosition(positionGui_Targets.get().x, positionGui_Targets.get().y);

		return;
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingOsc::refreshGui()
{
	if (!bGui_InternalAllowed || !bGui_Internal) return;

	//--

#ifdef USE_MIDI
	auto& gm = gui_Internal.getGroup(params_MIDI.getName());
	gm.minimize();
	if (bGui_MIDI)
	{
		gm.maximize();
	}
	else
	{
		gm.minimize();
	}
#endif

	//--

	auto& go = gui_Internal.getGroup(params_Osc.getName());
	go.minimize();
	if (bUseOut)
	{
		auto& gout = go.getGroup(params_OscOutput.getName());
		gout.minimize();
		//if (getOutEnablersSize() != 0) gout.getGroup(params_EnablerOuts.getName()).minimize();
	}
	if (bUseIn) go.getGroup(params_OscInput.getName()).minimize();

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	if (bUseIn) go.getGroup(params_PlotsInput.getName()).minimize();
#endif

	auto& ge = gui_Internal.getGroup(params_Extra.getName());
	ge.minimize();

	//if (bGui_OscHelper)
	//{
	//	go.maximize();
	//}
	//else
	//{
	//	go.minimize();
	//}
}

//--------------------------------------------------------------
void ofxSurfingOsc::keyPressed(ofKeyEventArgs& eventArgs)
{
	if (!bKeys) return;

	const int& key = eventArgs.key;

	if (key == 'g')
	{
		setToggleVisible();
	}

	else if (key == OF_KEY_RETURN)
	{
		for (size_t i = 0; i < 16; i++)
		{
			doTesterRandom();
		}
	}

	//else if (key == ' ')
	//{
	//	//listParams();
	//}

#ifdef USE_TEXT_FLOW
	else if (key == OF_KEY_BACKSPACE) {
		ofxTextFlow::clear();
	}
#endif
}

#ifdef USE_TEXT_FLOW

//--------------------------------------------------------------
void ofxSurfingOsc::setupTextFlow() {
	string str = "assets/fonts/JetBrainsMono-Bold.ttf";
	ofxTextFlow::loadFont(str, 8);
	ofxTextFlow::setMaxLineNum(40);
	ofxTextFlow::setShowing(true);
	ofxTextFlow::setBBoxWidth(200);
	ofxTextFlow::setShowingBBox(true);
	ofxTextFlow::setMarginBorders(10);
	ofxTextFlow::setRounded(true, 5);
	//ofxTextFlow::setTitle("OSC INPUT LOG: " + OSC_InputPort);//crash?
	ofxTextFlow::setShowFPS(false);
}

#endif

#ifdef USE_MIDI_OUT

//TODO:
//--------------------------------------------------------------
void ofxSurfingOsc::noteOut(int note, bool state)
{
	if (midiOut.isOpen())
	{
		int channel = 1;
		int velocity;
		if (state)
		{
			velocity = 127;
		}
		else
		{
			velocity = 0;
		}
		midiOut.sendNoteOn(channel, note, velocity);

		//print out both the midi note and the frequency
		ofLogNotice() << __FUNCTION__ << "note: " << note
			<< " freq: " << ofxMidi::mtof(note) << " Hz";
	}
	else
	{
		ofLogError() << __FUNCTION__ << "midi out port is closed!";
	}
}

#endif

//----

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS

//--------------------------------------------------------------
void ofxSurfingOsc::setupOutputSenders()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	setupPlotsColors();
#endif

	//--

	// 1. Params

	// Create params used to receive from the add-on class

	// NOTE:
	// param names goes from 1 to 8, not starting at 0 like vectors

	//--

	// Bangs / Triggers

	params_Bangs.setName("BANGS");

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string _name = "BANG_" + ofToString(i);
		bBangs[i].set(_name, false);
		params_Bangs.add(bBangs[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		//ofxHistoryPlot* graph = addPlot((float*)&bBangs[i].get(), _name, 0, 1.0f, colorBangs, 0);
		ofxHistoryPlot* graph = addPlot(_name, 0, 1.0f, colorBangs, 0);

		plotsTargets.push_back(graph);

		// BangRects

		bangRects[i].setColor(colorBangs);
		bangRects[i].setColorBackground(ofColor(0, 128));
		bangRects[i].setLocked(true);
		bangRects[i].setEnableBorder(true);
#endif
	}

	ofAddListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Bangs);

	//--

	// Toggles / State On-Off

	params_Toggles.setName("TOGGLES");

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string _name = "TOGGLE_" + ofToString(i);
		bToggles[i].set(_name, false);
		params_Toggles.add(bToggles[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		//ofxHistoryPlot* graph = addPlot((float*)&bToggles[i].get(), _name, 0, 1.0f, colorToggles, 0);
		ofxHistoryPlot* graph = addPlot(_name, 0, 1.0f, colorToggles, 0);

		plotsTargets.push_back(graph);

		// ToggleRects

		togglesRects[i].setColor(colorToggles);
		togglesRects[i].setColorBackground(ofColor(0, 128));
		togglesRects[i].setLocked(true);
		togglesRects[i].setEnableBorder(true);
		togglesRects[i].setToggleMode(true);
#endif
	}

	ofAddListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Toggles);

	//--

	// Values / Floats

	params_Values.setName("VALUES");

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string _name = "VALUE_" + ofToString(i);
		values[i].set(_name, 0.f, 0.f, 1.f);
		params_Values.add(values[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		//ofxHistoryPlot* graph = addPlot(values[i], _name, 0, 1.0f, colorValues, 2, false); // no smooth
		ofxHistoryPlot* graph = addPlot(_name, 0, 1.0f, colorValues, 2, false); // no smooth

		plotsTargets.push_back(graph);

		// Bars

		valuesBars[i].setColor(colorValues);
		valuesBars[i].setColorBackground(ofColor(0, 128));
		valuesBars[i].setRounded(_rounded);
		valuesBars[i].setEnableBorder(true);
#endif
	}

	ofAddListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Values);

	//--

	// Numbers / Int's

	int _min = 0;
	int _max = 1000;

	params_Numbers.setName("NUMBERS");

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string _name = "NUMBER_" + ofToString(i);
		numbers[i].set(_name, 0, _min, _max);
		params_Numbers.add(numbers[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
		//ofxHistoryPlot* graph = addPlot((float*)&numbers[i].get(), _name, _min, _max, colorNumbers, 0);
		ofxHistoryPlot* graph = addPlot(_name, _min, _max, colorNumbers, 0);

		plotsTargets.push_back(graph);

		// Bars

		numbersBars[i].setColor(colorNumbers);
		numbersBars[i].setColorBackground(ofColor(0, 128));
		numbersBars[i].setRounded(_rounded);
		numbersBars[i].setValueMax(_max);
		numbersBars[i].setValueMin(_min);
		numbersBars[i].setEnableBorder(true);
#endif
	}

	ofAddListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Numbers);

	//----

	//TODO: make external methods to improve customize workflow
	// Customize label and selected plots
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
	setupPlots();
#endif

	//----

	// Widgets for all the params they are:
	// 1. updating when receiving OSC/MIDI messages 
	// or 2. sending OSC/MIDI messages when params changes locally
	// that will depends on how params are subscribed to the class: 
	//addReceiver/addSender -> OSC in/out

	//params_Targets.setName("TARGETS");
	params_Targets.setName("OSC TARGETS");
	params_Targets.add(params_Bangs);
	params_Targets.add(params_Toggles);
	params_Targets.add(params_Values);
	params_Targets.add(params_Numbers);

	//--

	// Gui Targets

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS_GUI
	gui_Targets.setup("OSC TARGETS");
	gui_Targets.add(params_Targets);

	// Position
	positionGui_Targets.set("Gui Position Targets",
		glm::vec2(500, 10),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight()));
	gui_Targets.setPosition(positionGui_Targets.get().x, positionGui_Targets.get().y);
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	patchingManager.setPathGlobal(path_Global);
	patchingManager.setup("PATCHING MANAGER");
#endif

	//--

	bDone_SetupTargets = true;
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupTargetsAsReceivers()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//setModeFeedback(false); // disabled by default
	// When enabled, all received OSC/MIDI input messages 
	// are auto send to the output OSC too, as mirroring.

	//-

	// NOTE:
	// '/1/' -> it's an address pattern to adapt to pages or layout
	// of some controller apps like TouchOSC

	string tag = "1/";

	// add OSC targets to the addon class
	// this will subscribe the OSC addresses with/to the target local parameters

	//--

	// Bangs

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string Osc_Address = "/bang/" + tag + ofToString(i);
		addReceiver_Bool(bBangs[i], Osc_Address);
	}

	//--

	// Toggles

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string Osc_Address = "/toggle/" + tag + ofToString(i);
		addReceiver_Bool(bToggles[i], Osc_Address);
	}

	//--

	// Values

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string Osc_Address = "/value/" + tag + ofToString(i);
		addReceiver_Float(values[i], Osc_Address);
	}

	//--

	// Numbers

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string Osc_Address = "/number/" + tag + ofToString(i);
		addReceiver_Int(numbers[i], Osc_Address);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupTargetsAsSenders()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//setModeFeedback(false); // disabled by default
	// When enabled, all received OSC/MIDI input messages 
	// are auto send to the output OSC too, as mirroring.

	//-

	// NOTE:
	// '/1/' -> it's an address pattern to adapt to pages or layout
	// of some controller apps like TouchOSC

	string tag = "1/";

	// add OSC targets to the addon class
	// this will subscribe the OSC addresses with/to the target local parameters

	//--

	// Bangs

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string Osc_Address = "/bang/" + tag + ofToString(i);
		addSender_Bool(bBangs[i], Osc_Address);
	}

	//--

	// Toggles

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string Osc_Address = "/toggle/" + tag + ofToString(i);
		addSender_Bool(bToggles[i], Osc_Address);
	}

	//--

	// Values

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string Osc_Address = "/value/" + tag + ofToString(i);
		addSender_Float(values[i], Osc_Address);
	}

	//--

	// Numbers

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string Osc_Address = "/number/" + tag + ofToString(i);
		addSender_Int(numbers[i], Osc_Address);
	}
}

#endif

//----------------------------------------------------
void ofxSurfingOsc::setupReceiveLogger()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//TODO:
	//return;

	if (bUseIn)
	{
		ofxGetOscSubscriber(OSC_InputPort);
		// Create subscriber binding explicitly

		// NOTE:
		// must add [this] into lambda:
		// https://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles

		ofxSubscribeAllOsc([&](const ofxOscMessageEx& m, bool is_leaked)
			{
				bool _b = false;
		_b += bDebug;
#ifdef USE_TEXT_FLOW
		_b += (bGui_LogFlow);
#endif
		if (_b)
		{
			bool bskip = false;
			// workaround to ignore booleans false

			// Unrecognized message: 
			// Display on the bottom of the screen

			string msgString;
			msgString = "";
			msgString += "OSC IN \t";
			msgString += m.getAddress();

			for (size_t i = 0; i < m.getNumArgs(); i++) {

				msgString += "\t";
				//msgString += "\t";

				// Get the argument type too:
				//msgString += m.getArgTypeName(i);
				//msgString += ":";

				// Display the argument 
				// Make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32)
				{
					if (m.getArgAsInt32(i) == 0) bskip = true;
					// ignore bools arg as it is a bang
					if (m.getArgAsInt32(i) != 1) {
						msgString += ofToString(m.getArgAsInt32(i));
					}
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT)
				{
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING)
				{
					msgString += m.getArgAsString(i);
				}
				else
				{
					msgString += "Not handled argument type " + m.getArgTypeName(i);
				}
			}

			// More debug
			//msgString += " ";
			//msgString += m.getRemotePort() + " ";
			//msgString += m.getRemoteHost() + " ";
			//ofLogNotice("ofxSurfingOsc")
			//	//<< m.getWaitingPort() << " : "
			//	<< m.getAddress() << " "
			//	<< m.getRemoteHost() << " "
			//	<< m.getRemotePort() << " "
			//	//<< m.getTypeString() << " "
			//	//<< m.getNumArgs()
			//	;

			ofLogNotice("ofxSurfingOsc") << "OSC \t" << msgString;

#ifdef USE_IM_GUI
			if (!bskip) ui.AddToLog(msgString, tagIn);
#endif

#ifdef USE_TEXT_FLOW
			ofxTextFlow::addText(msgString);
#endif
		}
			});
	}
}

//--

// Local callbacks
// All the params will send 
// and receive OSC/MIDI changes

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Bangs(ofAbstractParameter& e) // preset load/trig
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_BANGS; i++)
	{
		// Only when true (BANG, not note-off)
		if (name == "BANG_" + ofToString(i) && bBangs[i])
		{
			// Bang
			ofLogNotice("ofxSurfingOsc") << (__FUNCTION__) << "\t BANGS   \t[" << ofToString(i) << "] " << "BANG";

			// Plot
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
			//if (bEnablerOuts[i])
			bangRects[i].bang();

			//TODO: 
			// Timed off. Required bc ofxPubSubOsc sends one state per frame.
			t_bBangs[i] = ofGetElapsedTimeMillis();
#endif

#ifdef USE_IM_GUI
			string s = name + "  \t" + (bBangs[i].get() ? "TRUE" : "FALSE");
			ui.AddToLog(ofToString(s), tagTarget);
#endif
			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Toggles(ofAbstractParameter& e)
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		if (name == "TOGGLE_" + ofToString(i))
		{
			// State
			bool b = bToggles[i].get();
			ofLogNotice("ofxSurfingOsc") << (__FUNCTION__) << "\t TOGGLE \t[" << ofToString(i) << "] " << (b ? "ON" : "OFF");

			// Plot
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS
			// first toggle (e.g: gist note)
			//if (b) { togglesRects[i].bang(); }

			if (b) { togglesRects[i].toggle(true); }
			else { togglesRects[i].toggle(false); }

			/*
			if (bEnablerOuts[i + NUM_BANGS])
				if (b) { togglesRects[i].toggle(true); }
				else { togglesRects[i].toggle(false); }
			*/
#endif

#ifdef USE_IM_GUI
			string s = name + "\t" + (bToggles[i].get() ? "TRUE" : "FALSE");
			ui.AddToLog(ofToString(s), tagTarget);
#endif
			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Values(ofAbstractParameter& e)
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_VALUES; i++)
	{
		if (name == "VALUE_" + ofToString(i))
		{
			// Values
			ofLogNotice("ofxSurfingOsc") << (__FUNCTION__) << "\t VALUE   \t[" << ofToString(i) << "] " << values[i];

#ifdef USE_IM_GUI
			string s = name + " \t" + ofToString(values[i].get(), 2);
			ui.AddToLog(ofToString(s), tagTarget);
#endif

			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Numbers(ofAbstractParameter& e)
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		if (name == "NUMBER_" + ofToString(i))
		{
			// numbers
			ofLogNotice("ofxSurfingOsc") << (__FUNCTION__) << "\t NUMBER \t[" << ofToString(i) << "] " << numbers[i];

#ifdef USE_IM_GUI
			string s = name + "\t" + ofToString(numbers[i].get(), 0);
			ui.AddToLog(ofToString(s), tagTarget);
#endif
			return;
		}
	}
}

//--

//--------------------------------------------------------------
void ofxSurfingOsc::exit_Targets()
{
	ofRemoveListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Bangs);
	ofRemoveListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Toggles);
	ofRemoveListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Values);
	ofRemoveListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Numbers);
}

#endif

//----

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PLOTS

//--------------------------------------------------------------
void ofxSurfingOsc::setupPlotsColors()
{
	//--

	// Plots Colors

	//// 0. Mono
	//int a = 200;
	//colorBangs = ofColor(ofColor::yellow, a);
	//colorToggles = ofColor(ofColor::yellow, a);
	//colorValues = ofColor(ofColor::yellow, a);
	//colorNumbers = ofColor(ofColor::yellow, a);

	//// 1. Colors
	//colorBangs = ofColor::green;
	//colorToggles = ofColor::yellow;
	//colorValues = ofColor::red;
	//colorNumbers = ofColor::blue;

	//// 2. Dark
	//int _dark = 24;
	//colorBangs = ofColor(_dark);
	//colorToggles = ofColor(_dark);
	//colorValues = ofColor(_dark);
	//colorNumbers = ofColor(_dark);

	// 3. White
	int c = 200;
	colorBangs = ofColor(c);
	colorToggles = ofColor(c);
	colorValues = ofColor(c);
	colorNumbers = ofColor(c);

	//// 4. Grays
	//int _min = 0;
	//int _max = 128;
	//colorBangs = ofColor((int)ofMap(0, 0, 1, _min, _max));
	//colorToggles = ofColor((int)ofMap(0.2, 0, 1, _min, _max));
	//colorValues = ofColor((int)ofMap(0.7, 0, 1, _min, _max));
	//colorNumbers = ofColor((int)ofMap(1, 0, 1, _min, _max));
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupPlotsCustomTemplate()
{
	int _a = 170;
	plotStyle s;

	// Disable all plots
	for (int _i = 0; _i < plotsTargets_Visible.size(); _i++)
	{
		plotsTargets_Visible[_i] = false;

		//TODO: WIP:
		// A. correlative organization
		mapPlots[_i] = _i;
	}

	////TODO: WIP:
	//// B. custom organization
	//mapPlots[0] = 0;//beat
	//mapPlots[0] = 0;//beat

	//----

	// Note that index starts at 1 instead of 0.

	// Bangs

	// Bang 1 
	s.name = "BEAT";
	s.target = plotTarget_Bang;
	s.index = 0;
	//s.index = mapPlots[0] + 1;
	s.color = ofColor(ofColor::white, _a);
	setupPlotCustom(s);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	patchingManager.setupStyleWidget(0, s.color);
#endif

	//--

	// Bang 2 
	s.name = "SIGNAL_0";
	s.target = plotTarget_Bang;
	s.index = 1;
	s.color = ofColor(ofColor::turquoise, _a);
	setupPlotCustom(s);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	patchingManager.setupStyleWidget(1, s.color);
#endif

	//--

	// Bang 3 
	s.name = "SIGNAL_1";
	s.target = plotTarget_Bang;
	s.index = 2;
	s.color = ofColor(ofColor::orange, _a);
	setupPlotCustom(s);

	//--

	// Values

	s.name = "SIGNAL_0";
	s.target = plotTarget_Value;
	s.index = 0;
	s.color = ofColor(ofColor::turquoise, _a);
	setupPlotCustom(s);

	s.name = "SIGNAL_1";
	s.target = plotTarget_Value;
	s.index = 1;
	s.color = ofColor(ofColor::orange, _a);
	setupPlotCustom(s);

	// Value 1 
	// Bpm
	s.name = "BPM";
	s.target = plotTarget_Value;
	s.index = 2;
	s.range = glm::vec2(40, 240);//min, max
	s.color = ofColor(ofColor::white, _a);
	setupPlotCustom(s);
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupPlotCustom(plotStyle style)
{
	// BeatCircles
	if (style.target == plotTarget_Bang)
	{
		int _start = 0;
		int _i = _start + style.index;//related to style
		ofColor _c = style.color;
		plotsTargets[_i]->setVariableName(style.name);
		plotsTargets[_i]->setColor(_c);
		plotsTargets_Visible[_i] = true;

		bangRects[_i].setColor(_c);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.setupStyleWidget(_i, _c);
#endif
	}

	// Toggles
	if (style.target == plotTarget_Toggle)
	{
		int _start = NUM_BANGS;
		int _i = _start + style.index;//related to style
		ofColor _c = style.color;
		plotsTargets[_i]->setVariableName(style.name);
		plotsTargets[_i]->setColor(_c);
		plotsTargets_Visible[_i] = true;

		togglesRects[_i - _start].setColor(_c);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.setupStyleWidget(_i, _c);
#endif

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
		// plot
		plotsTargets[_i]->setVariableName(style.name);
		plotsTargets[_i]->setColor(_c);
		plotsTargets_Visible[_i] = true;
		plotsTargets[_i]->setRange(_min, _max);

		// widget
		int _ii = _i - _start;
		valuesBars[_ii].setColor(_c);
		valuesBars[_ii].setValueMin(_min);
		valuesBars[_ii].setValueMax(_max);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.setupStyleWidget(_i, _c);
#endif
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
		// plot
		plotsTargets[_i]->setVariableName(style.name);
		plotsTargets[_i]->setColor(_c);
		plotsTargets_Visible[_i] = true;
		plotsTargets[_i]->setRange(_min, _max);

		// widget
		int _ii = _i - _start;
		numbersBars[_ii].setColor(_c);
		numbersBars[_ii].setValueMin(_min);
		numbersBars[_ii].setValueMax(_max);

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
		patchingManager.setupStyleWidget(_i, _c);
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupPlots()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//--

	//TODO:
	// Custom template

	// Array with bool of display state of any plot.
	plotsTargets_Visible.clear();
	plotsTargets_Visible.resize(amount_Plots_Targets);
	// amount total plots. Marked as selected or not.

	// Enable all plots by default
	for (int _i = 0; _i < plotsTargets_Visible.size(); _i++)
	{
		plotsTargets_Visible[_i] = true;
	}

	//----

	//TODO:
	//moved to startup..
	// Customize special plots
	// Template is for my ofxSoundAnalyzer add-on
	/*
	if (bCustomTemplate)
	{
		setupPlotsCustomTemplate();
	}
	*/

	//----

	// Count enabled plots to enable hide the all not used
	amountPlotsTargetsVisible = 0;
	for (int _i = 0; _i < plotsTargets_Visible.size(); _i++)
	{
		// count how many are enabled
		if (plotsTargets_Visible[_i]) amountPlotsTargetsVisible++;
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::updatePlots() {
	//return;//using pointer

	for (int i = 0; i < plotsTargets.size(); i++)
	{
		int _start, _end;

		_start = 0;
		_end = NUM_BANGS;
		if (i >= _start && i < _end)
			plotsTargets[i]->update(bBangs[i]); // ?
		//plotsTargets[i]->update(bBangs[i] && bEnablerOuts[i]); // ?

		_start = _end;
		_end = _start + NUM_TOGGLES;
		if (i >= _start && i < _end)
			plotsTargets[i]->update(bToggles[i - _start]);
		//plotsTargets[i]->update(bToggles[i - _start] && bEnablerOuts[i]);

		_start = _end;
		_end = _start + NUM_VALUES;
		if (i >= _start && i < _end)
			plotsTargets[i]->update(values[i - _start]);
		/*
			if (bEnablerOuts[i]) plotsTargets[i]->update(values[i - _start]);
			else plotsTargets[i]->update(0);//bypass
		*/

		_start = _end;
		_end = _start + NUM_NUMBERS;
		if (i >= _start && i < _end)
			plotsTargets[i]->update(numbers[i - _start]);
		/*
			if (bEnablerOuts[i]) plotsTargets[i]->update(numbers[i - _start]);
			else plotsTargets[i]->update(0);//bypass
		*/
	}
}

//--

// Plots

//--------------------------------------------------------------
void ofxSurfingOsc::drawPlots(ofRectangle rect)
{
	float _x = rect.getX();
	float _y = rect.getY();
	float _w = rect.getWidth();
	float _h = rect.getHeight();

	drawPlots(_x, _y, _w, _h);
}

//--------------------------------------------------------------
void ofxSurfingOsc::drawPlots(float _x, float _y, float _w, float _h)
{
	if (bGui_Plots)
	{
		int _amount;
		float _amountPlotsTargets = plotsTargets.size();
		float plotMargin = 3;
		float plotHeight;

		bool bDrawAll = bGui_AllPlots || !bCustomTemplate;

		// draw all plots
		// or draw only enable plots
		if (bDrawAll) {
			plotHeight = (_h - 2 * plotMargin) / (float)_amountPlotsTargets;
			//_amount = _amountPlotsTargets;
		}
		else {
			//amountPlotsTargetsVisible = 4;
			plotHeight = (_h - 2 * plotMargin) / (float)amountPlotsTargetsVisible;
			//_amount = amountPlotsTargetsVisible;
		}

		_amount = _amountPlotsTargets;

		float x, y, w, h;
		float _xpad = 30;
		float _xpadR = 5;
		float p = 0;//picked plot
		float _r;//widgets size
		bool bIsSmall;

		// accumulate how many we have drawn to not left empty spaces.

		//for (int j = 0; j < _amount; j++)
		for (int i = 0; i < _amount; i++)
		{
			//--

			// 1. Plots

			if (bDrawAll) y = plotMargin + plotHeight * i;
			else y = plotMargin + plotHeight * p;

			y += _y;
			x = _x + plotMargin;
			w = _w - 2 * plotMargin;
			h = plotHeight - plotMargin;

			_r = ofClamp((h * 0.8 * 0.5), h * 0.5 * 0.9, _w / 6.f);

			// when small will simplify drawing stuff like disabling borders.
			bIsSmall = (_r < 100);
			//bIsSmall = true;

			//layout
			float offsetR = 75;

			float xx;
			float yy;
			float ww;
			float hh;

			//--

			if (bDrawAll)
			{
				plotsTargets[i]->draw(x, y + plotMargin, w, h);
				p++;
			}
			else
			{
				if (plotsTargets_Visible[i])
				{
					plotsTargets[i]->draw(x, y + plotMargin, w, h);
					p++;
				}

				/*
				if (plotsTargets_Visible[i])
				{
					//TODO: WIP:
					// B. custom organization
					mapPlots[0] = 0;//beat
					mapPlots[1] = 5;//bpm
					//mapPlots[2] = 5;//bpm

					//TODO: WIP:
					// re arrange
					int j = mapPlots[i];

					plotsTargets[j]->draw(x, y + plotMargin, w, h);
					p++;
				}
				*/

				else
				{
					continue;
				}
			}

			//----

			// 2. Widgets

			//float _r = (h * 0.8 * 0.5);
			float _gap = h * 0.1;
			int _start, _end;
			int ii;

			//----

			// 2.1. CircleBeats

			//--

			// 2.1.1 Bangs

			_start = 0;
			_end = _start + NUM_BANGS;

			if (i >= _start && i < _end)
			{
				//bangRects[i].setEnableBorder(!bIsSmall);

				if (bLeft) {
					xx = x + _r + _xpad;
					yy = y + plotMargin + h / 2.0f;
				}
				else {
					xx = x + w - (_r + _xpadR) - offsetR;
					yy = y + 2 * plotMargin + h / 2.0f /*+ 3*/;//fix
				}
				ww = _r;

				bangRects[i].draw(glm::vec2(xx, yy), ww);
			}

			//--

			// 2.1.2 Toggles

			_start = _end;
			_end = _start + NUM_TOGGLES;
			ii = i - _start;

			if (i >= _start && i < _end)
			{
				//togglesRects[i].setEnableBorder(!bIsSmall);

				if (bLeft) {
					xx = x + _r + _xpad;
					yy = y + plotMargin + h / 2.0f;
				}
				else {
					xx = x + w - (_r + _xpadR) - offsetR;
					yy = y + plotMargin + h / 2.0f /*+ 3*/;//fix
				}
				ww = _r;

				togglesRects[ii].draw(glm::vec2(xx, yy), ww);
			}

			//--

			// 2.2. Bars

			// 2.2.1 Values

			//_start = NUM_BANGS + NUM_TOGGLES;
			_start = _end;
			_end = _start + NUM_VALUES;
			ii = i - _start;

			//if (i >= _start && i < _end && i != _end - 1) {//exclude bpm bc out of range
			if (i >= _start && i < _end)
			{
				float v = values[ii];

				//if (!bEnablerOuts[i]) v = 0;//bypass

				//valuesBars[i].setEnableBorder(!bIsSmall);

				ww = 2 * _r;
				hh = h - _gap;
				if (bLeft) {
					xx = x + _xpad;
					yy = y + plotMargin + h - 2;
				}
				else {
					xx = x + w - _xpadR - ww - offsetR;
					yy = y + plotMargin + h - 2;
				}

				valuesBars[ii].draw(
					v,
					glm::vec2(xx, yy),
					ww, hh);
			}

			//--

			// 2.2.2 Numbers

			_start = _end;
			_end = _start + NUM_NUMBERS;
			ii = i - _start;

			if (i >= _start && i < _end)
			{
				int v = numbers[ii];

				//if (!bEnablerOuts[i]) v = 0;//bypass

				//numbersBars[i].setEnableBorder(!bIsSmall);

				ww = 2 * _r;
				hh = h - _gap;
				if (bLeft) {
					xx = x + _xpad;
					yy = y + plotMargin + h - 2;
				}
				else {
					xx = x + w - _xpadR - ww - offsetR;
					yy = y + plotMargin + h - 2;
				}

				numbersBars[ii].draw(
					v,
					glm::vec2(xx, yy),
					ww, hh);
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::drawPlots()
{
	if (!bGui_Plots) return;

	// Mini size

	if (bPlotsMini)
	{
		// Bg
		ofPushStyle();
		ofFill();
		ofSetColor(OF_COLOR_BG_PANELS);
		float r = 3;
		ofDrawRectRounded(boxPlotsBg.getRectangle(), r);
		boxPlotsBg.draw();
		ofPopStyle();

		drawPlots(boxPlotsBg.getRectangle());
	}

	// Full Screen

	else
	{
		// Bg
		ofRectangle r = ofGetCurrentViewport();
		ofPushStyle();
		ofFill();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(r, 5);
		ofPopStyle();

		drawPlots(r);
	}
}

//--------------------------------------------------------------
//TODO: auto updatable pointer
//ofxHistoryPlot* ofxSurfingOsc::addPlot(float* ptr, string varName, float min, float max, ofColor color, int precision, bool _smooth)
//--------------------------------------------------------------
ofxHistoryPlot* ofxSurfingOsc::addPlot(string varName, float min, float max, ofColor color, int precision, bool _smooth)
{
	//TODO: min hardcoded to 0

	float _line = 1.0f;
	float _maxHistory = 60 * 5;

	//ofxHistoryPlot* graph = new ofxHistoryPlot((float*)&ptr, varName, _maxHistory, true);//true for autoupdate
	ofxHistoryPlot* graph = new ofxHistoryPlot(NULL, varName, _maxHistory, false);//true for autoupdate

	//graph->setRangeAuto();
	//graph->setAutoRangeShrinksBack(true);
	//graph2 scale can shrink back after growing if graph2 curves requires it
	graph->setRange(min, max); // this lock the auto range!

	if (_smooth)
	{
		graph->setShowSmoothedCurve(true);
		graph->setSmoothFilter(0.1);
	}

	// it looks like this can cause problems, maybe bc translating.
	graph->setCropToRect(true);
	graph->setRespectBorders(true);

	graph->setShowNumericalInfo(true);
	graph->setPrecision(precision);

	graph->setLineWidth(_line);
	graph->setColor(color);

	//graph->setDrawBackground(true);
	//graph->setBackgroundColor(ofColor(0, 225));
	graph->setDrawBackground(false);

	graph->setDrawGrid(false);

	//graph->setRangeAuto();

	// speed
	//graph->setMaxHistory(60 * 20);
	//graph->setMaxHistory(2000);

	//graph->setLowerRange(0);//set only the lowest part of the range upper is adaptative to curve

	//graph->setGridColor(colorGrid);//grid lines color
	//graph->setGridUnit(10);
	//graph->setDrawFromRight(true);//should move all widgets..

	return graph;
};
#endif
#endif

//----

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
//--------------------------------------------------------------
void ofxSurfingOsc::updatePatchingManager() {

	// IMPORTANT:
	// Reads incoming OSC messages 
	// feed to targets. 
	// To be processed now by the PatchingManager class.

	for (int i = 0; i < NUM_BANGS; i++)
	{
		patchingManager.pipeBangs[i].input = bBangs[i];
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		patchingManager.pipeToggles[i].input = bToggles[i];
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		patchingManager.pipeValues[i].input = values[i];
		//patchingManager.pipeValues[i].input = plotsTargets[i + NUM_BANGS + NUM_TOGGLES];//smoothed
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		patchingManager.pipeNumbers[i].input = numbers[i];
	}

	//--

	// Custom Template
	// Template is for my ofxSoundAnalyzer add-on

	if (bCustomTemplate)
	{
		// Bpm?
		// Customize some params:
		int _i = 8;
		patchingManager.pipeValues[_i - 1].setClamp(false);
		patchingManager.pipeValues[_i - 1].setRangeOutput(40, 300);
	}

	//--

	// Update pipe manager
	// to process filters!
	patchingManager.update();
}
#endif

//--

#ifdef SURF_OSC__USE__TARGETS_INTERNAL_PARAMS
//--------------------------------------------------------------
void ofxSurfingOsc::doTesterRandomPlay()
{
	// Timed Randomizer
	if (bRandomize && ofGetFrameNum() % 6 == 0)
	{
		doTesterRandom();
	}
}
//--------------------------------------------------------------
void ofxSurfingOsc::doTesterRandom()
{
	// Timed Randomizer
	{
		static bool b = false;
		if (ofGetFrameNum() % 60 == 0) b = !b;
		int o = b ? 0 : 4;//offset
		const float r = ofRandom(1);
		const int i0 = (ofRandom(1) < ofRandom(0.5)) ? 0 + o : 1 + o;
		const int i1 = (ofRandom(1) < ofRandom(0.5)) ? 2 + o : 3 + o;

		// here to be used as senders instead of receivers
		if (r < 0.1) bBangs[i0] = !bBangs[i0];
		else if (r < 0.2) bBangs[i1] = !bBangs[i1];
		else if (r < 0.3) bToggles[i0] = !bToggles[i0];
		else if (r < 0.4) bToggles[i1] = !bToggles[i1];
		else if (r < 0.5) values[i0] = ofRandom(values[i0].getMin(), values[i0].getMax());
		else if (r < 0.6) values[i1] = ofRandom(values[i1].getMin(), values[i1].getMax());
		else if (r < 0.7) values[i0] = ofRandom(values[i0].getMin(), values[i0].getMax());
		else if (r < 0.8) values[i1] = ofRandom(values[i1].getMin(), values[i1].getMax());
		else if (r < 0.9) numbers[i0] = ofRandom(numbers[i0].getMin(), numbers[i0].getMax());
		else if (r < 1) numbers[i1] = ofRandom(numbers[i1].getMin(), numbers[i1].getMax());
	}
}
#endif


//----

//--------------------------------------------------------------
void ofxSurfingOsc::linkBang(ofParameter<bool>& p)
{
	static int i = 0;
	p.makeReferenceTo(bBangs[i++]);
	//p.makeReferenceTo(bBangs[++i]);//starting at 1 instead of 0
}
//--------------------------------------------------------------
void ofxSurfingOsc::linkToggle(ofParameter<bool>& p)
{
	static int i = 0;
	p.makeReferenceTo(bToggles[i++]);
	//p.makeReferenceTo(bToggles[++i]);
}
//--------------------------------------------------------------
void ofxSurfingOsc::linkValue(ofParameter<float>& p)
{
	static int i = 0;
	p.makeReferenceTo(values[i++]);
	//p.makeReferenceTo(values[++i]);
}
//--------------------------------------------------------------
void ofxSurfingOsc::linkNumber(ofParameter<int>& p)
{
	static int i = 0;
	p.makeReferenceTo(numbers[i++]);
	//p.makeReferenceTo(numbers[++i]);
}


//TODO:
//--------------------------------------------------------------
//void ofxSurfingOsc::addReceiver(ofAbstractParameter &e, string address)
//{
//	if (!bUseIn) return;
//  ofxSubscribeOsc(12345, address, &e);
//}
//

//TODO:
////--------------------------------------------------------------
//void ofxSurfingOsc::listParams()
//{
//	int i = 0;
//	for (auto &p : paramsAbstract)
//	{
//		cout << "[" << ofToString(i++) << "] " << p.getName() << " : " << p.type << endl;
//	}
//}

////--------------------------------------------------------------
//void ofxSurfingOsc::addReceiver_Float(ofParameter<float> &f, string address)
//{
//	paramsAbstract.push_back(f);
//}
