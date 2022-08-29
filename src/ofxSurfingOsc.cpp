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

	//--

	//TODO:
	// Debug info
	// should improve performance building list once
	// to show subscribed OSC addresses
	strs_inputAddresses.clear();
	strs_outputAddresses.clear();

	str_oscInputAddressesInfo = "";
	str_oscOutputAddressesInfo = "";
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

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	{
		boxPlotsBg.setPathGlobal(path_Global);
		boxPlotsBg.setName("Plots");
		boxPlotsBg.bEdit.setName("EDIT PLOTS");
		patchingManager.boxPlotsBg.bEdit.makeReferenceTo(boxPlotsBg.bEdit);
		boxPlotsBg.setup();
	}
#endif

	//--

	// Help box
	boxHelp.bGui.setName("Help");
	boxHelp.setPath(path_Global);
	boxHelp.setup();

	//--

	setupParams();

	setupOsc();
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

	if (bUseOscIn)
	{
		strHelpInfo += "INPUT \n";
		strHelpInfo += "PORT      " + str_OSC_InputPort.get() + "\n";
		strHelpInfo += "\n";
	}

	if (bUseOscOut) {
		strHelpInfo += "OUTPUT \n";
		strHelpInfo += "PORT      " + str_OSC_OutputPort.get() + "\n";
		strHelpInfo += "IP        " + OSC_OutputIp.get() + "\n";
		strHelpInfo += "\n";
	}

	strHelpInfo += str_oscAddressesInfo;

	boxHelp.setText(strHelpInfo);
}

//----------------------------------------------------
void ofxSurfingOsc::disconnectOscInput()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	ofxUnsubscribeOsc();
}

//----------------------------------------------------
void ofxSurfingOsc::setupOsc()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//--

	// OSC Settings

	params_OscSettings.clear();
	params_OscSettings.setName("OSC_SETTINGS");

	if (bUseOscIn)
	{
		params_OscSettings.add(bEnableOsc_Input);
		params_OscSettings.add(OSC_InputPort);
	}

	if (bUseOscOut)
	{
		params_OscSettings.add(bEnableOsc_Output);
		params_OscSettings.add(OSC_OutputPort);
		params_OscSettings.add(OSC_OutputIp);
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
void ofxSurfingOsc::startup()
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

	// Default settings
	{
		// Remote machines

		OSC_OutputIp = "127.0.0.1";			// Local
		///OSC_OutputIp = "192.168.0.135";	// iPad
		///OSC_OutputIp = "192.168.0.122";	// macBook Air

		//--

		if (bUseOscOut)
		{
			bEnableOsc_Output = true;
			OSC_InputPort = 12345;
			OSC_OutputPort = 54321;
		}
		else if (bUseOscIn)
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
	if (bUseOscIn)
	{
		bEnableOsc_Input = bEnableOsc_Input;
	}

	//--

	// Initialize receiver mode when Input is enabled!
	//if(0)
	if (bUseOscIn)
	{
		// Setup param Targets
		setupReceiverTargets();

		// Subscribe all parameters
		// to OSC incoming messages
		setupReceiverSubscribers();

		// Log incoming messages
		setupReceiveLogger();
	}

	//--

	// OSC

	// Debug Info
	{
		// Debug and show all subscribed addresses
		// Process all subscribed addresses

		if (bUseOscIn)
		{
			str_oscInputAddressesInfo = "ADDRESSES";
			for (auto a : strs_inputAddresses)
			{
				str_oscInputAddressesInfo += "\n";
				str_oscInputAddressesInfo += a;
			}

			str_oscAddressesInfo += str_oscInputAddressesInfo;
		}

		if (bUseOscOut)
		{
			str_oscOutputAddressesInfo = "ADDRESSES";
			for (auto a : strs_outputAddresses)
			{
				str_oscOutputAddressesInfo += "\n";
				str_oscOutputAddressesInfo += a;
			}

			str_oscAddressesInfo += str_oscOutputAddressesInfo;
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

		//if (bUseOscIn) str_oscAddressesInfo += str_oscInputAddressesInfo;
		//if (bUseOscOut) str_oscAddressesInfo += str_oscOutputAddressesInfo;

		//--

		buildHelp();
	}

	//--

	// Load Settings

	ofxSurfingHelpers::loadGroup(params_AppSettings, path_Global + "/" + path_AppSettings);

	refreshGui();
}

//----------------------------------------------------
void ofxSurfingOsc::setupParams()
{
	bGui.set("ofxSurfingOsc", true);
	bEnableOsc.set("Enable OSC", true);
	bKeys.set("Keys", true);
	bDebug.set("Debug", false);

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	bGui_Targets.set("OSC TARGETS", true);
#endif
#endif

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	bGui_Plots.set("Plots", true);
	bEnableSmoothPlots.set("Smooth", false);
	smoothPlotsPower.set("Smooth Power", 0.5, 0, 1);
	bGui_AllPlots.set("All", false);
	bPlotsMini.set("Mini", false);
#endif
#endif

#ifdef USE_MIDI
	bGui_MIDI.set("Show MIDI", false);
#endif

#ifdef USE_TEXT_FLOW
	bGui_Log.set("Log", false);
#endif

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	bGui_PatchingManager.set("PATCHING MANAGER", false);
#endif

	//--

	// Extra

	params_Extra.setName("Extra");
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	if (bDone_SetupReceiver) params_Extra.add(bGui_Targets);
#endif
#endif
	params_Extra.add(bKeys);
	params_Extra.add(boxHelp.bGui);
	params_Extra.add(bDebug);

	//--

	// OSC

	if (bUseOscIn)
	{
		bEnableOsc_Input.set("Enable Input", false);
		OSC_InputPort.set("OSC_Input_Port", 12345, 0, 99999);
		str_OSC_InputPort.set("OSC Input Port", "");//out
		str_OSC_InputPort = ofToString(OSC_InputPort);

		bModeFeedback.set("Feedback", false);
	}

	if (bUseOscOut)
	{
		bEnableOsc_Output.set("Enable_Output", true);//TODO: cant be disabled once publishers are added..
		OSC_OutputPort.set("OSC_Output_Port", 54321, 0, 99999);
		str_OSC_OutputPort.set("OSC Output Port", "");//out
		str_OSC_OutputPort = ofToString(OSC_InputPort);
		OSC_OutputIp.set("Output IP", "127.0.0.1");
	}

	if (bUseOscIn)
	{
		params_OscInput.setName("OSC INPUT");
		params_OscInput.add(bEnableOsc_Input);
		params_OscInput.add(str_OSC_InputPort);
		params_OscInput.add(bModeFeedback);
	}

	if (bUseOscOut)
	{
		params_OscOutput.setName("OSC OUTPUT");
		params_OscOutput.add(bEnableOsc_Output);
		params_OscOutput.add(OSC_OutputIp);
		params_OscOutput.add(str_OSC_OutputPort);
	}

	// exclude
	str_OSC_InputPort.setSerializable(false);
	str_OSC_OutputPort.setSerializable(false);

	if (bUseOscIn) str_OSC_InputPort = ofToString(OSC_InputPort);
	if (bUseOscOut) str_OSC_OutputPort = ofToString(OSC_OutputPort);

	//--

	params_Osc.setName("OSC");
	if (bUseOscIn) params_Osc.add(params_OscInput);
	if (bUseOscOut) params_Osc.add(params_OscOutput);

#ifdef USE_TEXT_FLOW
	params_Osc.add(bGui_Log);
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	params_PlotsInput.setName("PLOTS INPUT");
	params_PlotsInput.add(bGui_Plots);
	params_PlotsInput.add(bPlotsMini);
	params_PlotsInput.add(bGui_AllPlots);
	//params_PlotsInput.add(boxPlotsBg.bEditMode);
	//params_PlotsInput.add(bEnableSmoothPlots);
	//params_PlotsInput.add(smoothPlotsPower);
#endif

	//--

#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	params_Osc.add(bGui_PatchingManager);
#endif

	//--

	// MIDI

#ifdef USE_MIDI
	bEnableMIDI.set("Enable MIDI", true);
	params_MIDI.setName("MIDI");

	if (bUseOscIn) {
		MIDI_InputPort.set("MIDI INPUT PORT", 0, 0, 10);
		str_MIDI_InputPort_name.set("", "");
	}

#ifdef USE_MIDI_OUT
	if (bUseOscOut) {
		MIDI_OutputPort.set("MIDI OUTPUT PORT", 0, 0, 10);
		MIDI_OutPort_name.set("", "");
	}
#endif

	params_MIDI.add(bEnableMIDI);
	if (bUseOscIn) {
		params_MIDI.add(MIDI_InputPort);
		params_MIDI.add(str_MIDI_InputPort_name);
		str_MIDI_InputPort_name.setSerializable(false);
	}
#ifdef USE_MIDI_OUT
	if (bUseOscOut) {
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
	{
		gui_Internal.setup(bGui.getName());

		// OSC
		gui_Internal.add(params_Osc);

		// MIDI
#ifdef USE_MIDI
		gui_Internal.add(bGui_MIDI);
		gui_Internal.add(params_MIDI);
#endif
		// Plots
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		gui_Internal.add(params_PlotsInput);
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

	//--

	// Params Settings

	// Not into gui: just for store/recall and callbacks

	params_AppSettings.setName("Osc_AppSettings");
	params_AppSettings.add(positionGui_Internal);
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	params_AppSettings.add(positionGui_Targets);
#endif
#ifdef USE_TEXT_FLOW
	params_AppSettings.add(bGui_Log);
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
	params_AppSettings.add(bGui_PatchingManager);
#endif

	//--

	// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	params_AppSettings.add(params_PlotsInput);
	//params_AppSettings.add(bEnableSmoothPlots);//crashes
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

	if (bUseOscIn)
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
		//		setupOsc();
		//	}
		//}
	}

	//--

	if (bUseOscOut)
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

}

//--------------------------------------------------------------
void ofxSurfingOsc::update()
{
#ifdef USE_MIDI
	if (ofGetFrameNum() == 1) bEnableMIDI = bEnableMIDI; // workaround
#endif

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS 
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	update_PatchingManager();
#endif
#endif

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	updatePlots();
#endif

	if (bUseOscOut && bDebug && ofGetFrameNum() % 120 == 0)
	{
		// out
		auto ss = ofxGetPublishedAddresses(OSC_OutputIp.get(), OSC_OutputPort);
		ofLogNotice("ofxSurfingOsc::ofxGetPublishedAddresses") << ofToString(ss);

		//// in?
		//auto ss2 = ofxGetRegisteredAddresses(OSC_OutputIp.get(), OSC_OutputPort);
		//ofLogNotice("ofxSurfingOsc::ofxGetRegisteredAddresses") << ofToString(ss2);
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

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS 
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
//--------------------------------------------------------------
void ofxSurfingOsc::draw_PatchingManager()
{
	patchingManager.draw();
}
#endif
#endif

//--------------------------------------------------------------
void ofxSurfingOsc::draw()
{
	if (!bGui) return;

	//--

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	if (bGui_Plots)
	{
		drawPlots();
	}
#endif
#endif

	//--

	// GUI Targets

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS 
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	if (bGui_Targets && bDone_SetupReceiver)
	{
		gui_Targets.draw();
	}
#endif 
#endif

	//--

	// GUI Internal

	gui_Internal.draw();

	//--

	/*

	auto p = gui_Internal.getPosition();
	auto w = gui_Internal.getWidth();
	auto h = gui_Internal.getHeight();

	// OSC debug

	// Show all subscribed addresses
	if (bDebug)
	{
		//if (bGui_OscHelper && bEnableOsc)
		if (bEnableOsc)
		{
			ofPushMatrix();

			ofTranslate(p.x + 15, p.y + 25 + h); // bottom of the gui panel
			//ofTranslate(p.x + w + 10, p.y + 16); // right

			if (myFont.isLoaded())
			{
				ofPushStyle();
				ofSetColor(0);
				ofFill();
				ofDrawRectRounded(rectDebug, 3.0f);
				ofSetColor(255);
				myFont.drawString(str_oscAddressesInfo, 0, 0);
				ofPopStyle();
			}
			else
			{
				ofDrawBitmapStringHighlight(str_oscAddressesInfo, 0, 0);
			}

			ofPopMatrix();
		}
	}
	*/

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
	if (bGui_PatchingManager) draw_PatchingManager();
#endif

	//--

	boxHelp.draw();
}

//--------------------------------------------------------------
void ofxSurfingOsc::exit()
{
	ofLogWarning("ofxSurfingOsc") << (__FUNCTION__);

	ofRemoveListener(params_AppSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
	ofRemoveListener(params_Osc.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
	ofRemoveListener(params_OscSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params_Osc);

#ifdef USE_MIDI
	ofRemoveListener(params_MIDI.parameterChangedE(), this, &ofxSurfingOsc::Changed_Params);
#endif

	positionGui_Internal = gui_Internal.getPosition();
#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	positionGui_Targets = gui_Targets.getPosition();
#endif

	ofxSurfingHelpers::saveGroup(params_AppSettings, path_Global + "/" + path_AppSettings);
	ofxSurfingHelpers::saveGroup(params_OscSettings, path_Global + "/" + path_OscSettings);

	//-

#ifdef USE_MIDI_OUT
	midiOut.closePort();
#endif
}

//--

// Note that some settings can not be changed without restart the app!

//--------------------------------------------------------------
void ofxSurfingOsc::setInputPort(int p)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "setInputPort: " << p;
	OSC_InputPort = p;
	str_OSC_InputPort = ofToString(OSC_InputPort);
}

//--------------------------------------------------------------
void ofxSurfingOsc::setOutputPort(int p)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "setInputPort: " << p;
	OSC_OutputPort = p;
	str_OSC_OutputPort = ofToString(OSC_OutputPort);
}

//--------------------------------------------------------------
void ofxSurfingOsc::setOutputIp(string ip)
{
	ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "setOutputIp: " << ip;
	OSC_OutputIp = ip;
}

//----

// Receivers (Osc In)

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Int(ofParameter<int>& p, string address)
{
	if (!bUseOscIn)
	{
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Input is disabled. That param " << p.getName() << " will not be registered!";

		return;
	}

	//paramsAbstract.push_back(i);

#ifdef USE_MIDI
	mMidiParams.add(i);
#endif

	strs_inputAddresses.push_back(address);

	ofxSubscribeOsc(OSC_InputPort, address, p);

	//--

	// Feedback
	if (bModeFeedback && bUseOscOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Float(ofParameter<float>& p, string address)
{
	if (!bUseOscIn) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") <<
			"OSC Input is disabled. That param " << p.getName() << " will not be registered!";

		return;
	}

	//paramsAbstract.push_back(f);

#ifdef USE_MIDI
	mMidiParams.add(p);
#endif

	strs_inputAddresses.push_back(address);

	ofxSubscribeOsc(OSC_InputPort, address, p);

	//--

	// Feedback
	if (bModeFeedback && bUseOscOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Bool(ofParameter<bool>& p, string address)
{
	if (!bUseOscIn) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") << "OSC Input is disabled. That param " << p.getName() << " will not be registered!";

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

	//--

	// Feedback
	if (bModeFeedback && bUseOscOut)
	{
		ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	}
}

//----

// Senders (Osc Out)

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Float(ofParameter<float>& p, string address)
{
	if (!bUseOscOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") << "OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	strs_outputAddresses.push_back(address);
}

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Int(ofParameter<int>& p, string address)
{
	if (!bUseOscOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") << "OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	strs_outputAddresses.push_back(address);
}

//--------------------------------------------------------------
void ofxSurfingOsc::addSender_Bool(ofParameter<bool>& p, string address)
{
	if (!bUseOscOut) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") << "OSC Output is disabled. That param " << p.getName() << " will not be registered!";
		return;
	}

	ofxPublishOsc(OSC_OutputIp, OSC_OutputPort, address, p);
	strs_outputAddresses.push_back(address);
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
	else if (name == bGui_Log.getName())
	{
		ofxTextFlow::setShowing(bGui_Log);

		return;
	}
#endif

	//--

	// OSC

	//--

	/*
	// Input

	if (bUseOscIn)
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

	if (bUseOscOut)
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

	//--

	//TODO:
#ifdef SURF_OSC__USE__RECEIVER_PATCHING_MODE
	if (name == bGui_PatchingManager.getName())
	{
		if (bGui_PatchingManager) {
		}

		return;
	}
#endif

	//--

	// Smooth

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	if (name == bEnableSmoothPlots.getName())
	{
		int _start = NUM_BANGS + NUM_TOGGLES;

		for (int i = _start; i < _start + NUM_VALUES; i++)
		{
			if (i > plots_Targets.size() - 1) return;
			plots_Targets[i]->setShowSmoothedCurve(bEnableSmoothPlots);
		}

		return;
	}
	else if (name == smoothPlotsPower.getName())
	{
		if (bEnableSmoothPlots) {
			int _start = NUM_BANGS + NUM_TOGGLES;

			for (int i = _start; i < _start + NUM_VALUES; i++)
			{
				if (i > plots_Targets.size() - 1) return;
				plots_Targets[i]->setSmoothFilter(0.1);
			}
		}

		return;
	}
#endif

	//--

	// MIDI

#ifdef USE_MIDI

	// MIDI in

	if (name == MIDI_InputPort.getName())
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

	if (name == MIDI_OutputPort.getName())
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
	if (name == bGui_Log.getName())
	{
		// workflow
		if (bGui_Log) bGui_OscHelper = true;
		//if (!bGui_OscHelper && bGui_Log) bGui_OscHelper = true;

		return;
	}
#endif

	//--

	if (name == positionGui_Internal.getName())
	{
		gui_Internal.setPosition(positionGui_Internal.get().x, positionGui_Internal.get().y);

		return;
	}

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
	if (name == positionGui_Targets.getName())
	{
		gui_Targets.setPosition(positionGui_Targets.get().x, positionGui_Targets.get().y);

		return;
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingOsc::refreshGui()
{

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
	if (bUseOscOut) go.getGroup(params_OscOutput.getName()).minimize();
	if (bUseOscIn) go.getGroup(params_OscInput.getName()).minimize();

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	go.getGroup(params_PlotsInput.getName()).minimize();
	go.minimize();
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

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS

//--------------------------------------------------------------
void ofxSurfingOsc::setupReceiverTargets()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//--

	// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS

	// Plots Colors

	//// Colors
	//colorBangs = ofColor::aquamarine;
	//colorToggles = ofColor::aliceBlue;
	//colorValues = ofColor::cyan;
	//colorNumbers = ofColor::deepSkyBlue;

	//// Grays
	//int _min = 0;
	//int _max = 128;
	//colorBangs = ofColor((int)ofMap(0, 0, 1, _min, _max));
	//colorToggles = ofColor((int)ofMap(0.2, 0, 1, _min, _max));
	//colorValues = ofColor((int)ofMap(0.7, 0, 1, _min, _max));
	//colorNumbers = ofColor((int)ofMap(1, 0, 1, _min, _max));

	// Dark
	int _dark = 24;
	colorBangs = ofColor(_dark);
	colorToggles = ofColor(_dark);
	colorValues = ofColor(_dark);
	colorNumbers = ofColor(_dark);

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
		string _name = "BANG_" + ofToString(i + 1);
		bBangs[i].set(_name, false);
		params_Bangs.add(bBangs[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorBangs, 0);
		plots_Targets.push_back(graph);

		// BeatCircles

		bangCircles[i].setColor(colorBangs);
		bangCircles[i].setLocked(true);
#endif
	}

	ofAddListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Bangs);

	//--

	// Toggles / State On-Off

	params_Toggles.setName("TOGGLES");
	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string _name = "TOGGLE_" + ofToString(i + 1);
		bToggles[i].set(_name, false);
		params_Toggles.add(bToggles[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorToggles, 0);
		plots_Targets.push_back(graph);

		// BeatCircles

		togglesCircles[i].setColor(colorToggles);
		togglesCircles[i].setLocked(true);
#endif
	}

	ofAddListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Toggles);

	//--

	// Values / Floats

	params_Values.setName("VALUES");
	for (int i = 0; i < NUM_VALUES; i++)
	{
		string _name = "VALUE_" + ofToString(i + 1);
		values[i].set(_name, 0.f, 0.f, 1.f);
		params_Values.add(values[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorValues, 2, false);// no smooth
		plots_Targets.push_back(graph);

		// Bars

		barValues[i].setColor(colorValues);
#endif
	}

	ofAddListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Values);

	//--

	// Numbers / Int's

	params_Numbers.setName("NUMBERS");
	int _max = 1000;
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string _name = "NUMBER_" + ofToString(i + 1);
		numbers[i].set(_name, 0, 0, _max);
		params_Numbers.add(numbers[i]);

		//--

		// Plots

#ifdef SURF_OSC__USE__RECEIVER_PLOTS
		ofxHistoryPlot* graph = addGraph(_name, _max, colorNumbers, 0);
		plots_Targets.push_back(graph);
#endif
	}

	ofAddListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Numbers);

	//--

	// Customize label and selected plots
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
	 customizePlots();
#endif

	//----

	// Widgets for all the params they are:
	// 1. updating when receiving OSC/MIDI messages 
	// or 2. sending OSC/MIDI messages when params changes locally
	// that will depends on how params are subscribed to the class: 
	//addReceiver/addSender -> OSC in/out

	params_Targets.setName("TARGETS");
	params_Targets.add(params_Bangs);
	params_Targets.add(params_Toggles);
	params_Targets.add(params_Values);
	params_Targets.add(params_Numbers);

	//--

	// Gui Targets

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS_GUI
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

	bDone_SetupReceiver = true;
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupReceiverSubscribers()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//setModeFeedback(false); // disabled by default
	// When enabled, all received OSC/MIDI input messages 
	// are auto send to the output OSC too, as mirroring.

	string tag = "1/";

	//-

	// add OSC targets to the addon class
	// this will subscribe the OSC addresses with/to the target local parameters

	// NOTE:
	// '/1/' -> it's an address pattern to adapt to pages or layout
	// of some controller apps like TouchOSC

	//--

	// Bangs

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string Osc_Address = "/bang/" + tag + ofToString(i + 1);
		addReceiver_Bool(bBangs[i], Osc_Address);
	}

	//--

	// Toggles

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string Osc_Address = "/toggle/" + tag + ofToString(i + 1);
		addReceiver_Bool(bToggles[i], Osc_Address);
	}

	//--

	// Values

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string Osc_Address = "/value/" + tag + ofToString(i + 1);
		addReceiver_Float(values[i], Osc_Address);
	}

	//--

	// Numbers

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string Osc_Address = "/number/" + tag + ofToString(i + 1);
		addReceiver_Int(numbers[i], Osc_Address);
	}
}

//----------------------------------------------------
void ofxSurfingOsc::setupReceiveLogger()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	if (bUseOscIn)
	{
		ofxGetOscSubscriber(OSC_InputPort);
		// create subscriber binding explicitly

		// NOTE:
		// must add [this] into lambda:
		// https://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles

		ofxSubscribeAllOsc([&](const ofxOscMessageEx& m, bool is_leaked)
			{
				bool _b = false;
				_b += bDebug;
#ifdef USE_TEXT_FLOW
				_b += (bGui_Log);
#endif

				if (_b)
				{
					// Unrecognized message: 
					// display on the bottom of the screen

					string msgString;

					msgString = m.getAddress();

					for (size_t i = 0; i < m.getNumArgs(); i++) {

						msgString += "\t\t";

						// Get the argument type too:
						//msgString += m.getArgTypeName(i);
						//msgString += ":";

						// Display the argument 
						// make sure we get the right type
						if (m.getArgType(i) == OFXOSC_TYPE_INT32)
						{
							msgString += ofToString(m.getArgAsInt32(i));
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

					ofLogNotice("ofxSurfingOsc") << "OSC IN \t" << msgString;

#ifdef USE_TEXT_FLOW
					ofxTextFlow::addText(msgString);
#endif
				}
			});
	}
}

#endif

//--

// Local callbacks
// All the params will send 
// and receive OSC/MIDI changes

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Bangs(ofAbstractParameter& e) // preset load/trig
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_BANGS - 1; i++)
	{
		// only when true (BANG, not note-off)
		if (name == "BANG_" + ofToString(i + 1) && bBangs[i])
		{
			// bang
			ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "BANGS\t[" << ofToString(i + 1) << "] " << "BANG";

			// Plot
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
			//plots_Targets[i]->update(1);
			bangCircles[i].bang();
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

	for (int i = 0; i < NUM_TOGGLES - 1; i++)//?
	{
		if (name == "TOGGLE_" + ofToString(i + 1))
		{
			// state
			bool b = bToggles[i].get();
			ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "TOGGLE\t[" << ofToString(i + 1) << "] " << (b ? "ON" : "OFF");

			// Plot
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
			// first toggle (eg: gist note)
			if (b) { togglesCircles[i].bang(); }
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

	for (int i = 0; i < NUM_VALUES - 1; i++)
	{
		if (name == "VALUE_" + ofToString(i + 1))
		{
			// values
			ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "VALUE\t[" << ofToString(i + 1) << "] " << values[i];

			return;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_Tar_Numbers(ofAbstractParameter& e)
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	for (int i = 0; i < NUM_NUMBERS - 1; i++)
	{
		if (name == "NUMBER_" + ofToString(i + 1))
		{
			// numbers
			ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "NUMBER\t[" << ofToString(i + 1) << "] " << numbers[i];

			return;
		}
	}
}

//--

//--------------------------------------------------------------
void ofxSurfingOsc::exit_InternalParams()
{
	ofRemoveListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Bangs);
	ofRemoveListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Toggles);
	ofRemoveListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Values);
	ofRemoveListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_Tar_Numbers);
}

#endif

//----

#ifdef SURF_OSC__USE__RECEIVER_INTERNAL_PARAMS
#ifdef SURF_OSC__USE__RECEIVER_PLOTS
//--------------------------------------------------------------
void ofxSurfingOsc::customizePlots() {

	//TODO:
	// Custom template

	// Array with bool of display state of any plot
	plots_Selected.clear();
	plots_Selected.resize(NUM_PLOTS);

	// Disable all plots by default
	for (int _i = 0; _i < plots_Selected.size(); _i++)
		plots_Selected[_i] = false; 

	//----

	// Customize special plots

	// Template is for my audio analyzer
	int _i;
	int _start;

	//-

	// Bangs
	_start = 0;

	// Bang 1 as beat
	_i = _start + 1;
	plots_Targets[_i - 1]->setVariableName("BEAT");
	plots_Targets[_i - 1]->setColor(ofColor::green);
	plots_Selected[_i - 1] = true;
	bangCircles[_i - 1].setColor(ofColor::green);

	// Bang 2 as onset
	_i = _start + 2;
	plots_Targets[_i - 1]->setVariableName("ONSET");
	plots_Targets[_i - 1]->setColor(ofColor::red);
	plots_Selected[_i - 1] = true;
	bangCircles[_i - 1].setColor(ofColor::red);

	//--

	// Toggles
	_start = NUM_BANGS;

	// Toggle 1 as note
	_i = _start + 1;
	plots_Targets[_i - 1]->setVariableName("NOTE");
	plots_Targets[_i - 1]->setColor(ofColor::orange);
	plots_Selected[_i - 1] = true;
	togglesCircles[_i - 1 - _start].setColor(ofColor::orange);

	//--

	// Values
	_start = NUM_BANGS + NUM_TOGGLES;

	// Value 1 
	_i = _start + 1;
	plots_Targets[_i - 1]->setVariableName("AUBIO");
	plots_Targets[_i - 1]->setColor(ofColor::red);
	plots_Selected[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::red);

	// Value 2 
	_i = _start + 2;
	plots_Targets[_i - 1]->setVariableName("GIST");
	plots_Targets[_i - 1]->setColor(ofColor::orange);
	plots_Selected[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::orange);

	//// Value 3 
	//_i = _start + 3;
	//plots_Targets[_i - 1]->setVariableName("CIRCLE AUBIO");
	//plots_Targets[_i - 1]->setColor(ofColor::green);
	//plots_Selected[_i - 1] = true;

	//// Value 4
	//_i = _start + 4;
	//plots_Targets[_i - 1]->setVariableName("CIRCLE GIST");
	//plots_Targets[_i - 1]->setColor(ofColor::orange);
	//plots_Selected[_i - 1] = true;

	// Value 8
	_i = _start + 8; // last value
	plots_Targets[_i - 1]->setRange(40, 300);
	plots_Targets[_i - 1]->setVariableName("BPM");
	plots_Targets[_i - 1]->setPrecision(0);
	plots_Targets[_i - 1]->setColor(ofColor::lightPink);
	plots_Selected[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::lightPink);

	//----

	// Count enabled plots to enable hide the all not used
	numPlotsEnable = 0;
	for (int _i = 0; _i < plots_Selected.size(); _i++)
	{
		if (plots_Selected[_i])
			numPlotsEnable++;
	}

	//cout << "numPlotsEnable:" << numPlotsEnable << endl;
}

//--------------------------------------------------------------
void ofxSurfingOsc::updatePlots() {

	// Update plots
	for (int i = 0; i < plots_Targets.size(); i++)
	{
		int _start, _end;

		_start = 0;
		_end = NUM_BANGS;
		if (i >= _start && i < _end)
			plots_Targets[i]->update(bBangs[i]);//0?

		_start = _end;
		_end = _start + NUM_TOGGLES;
		if (i >= _start && i < _end)
			plots_Targets[i]->update(bToggles[i - _start]);

		_start = _end;
		_end = _start + NUM_VALUES;
		if (i >= _start && i < _end)
			plots_Targets[i]->update(values[i - _start]);

		_start = _end;
		_end = _start + NUM_NUMBERS;
		if (i >= _start && i < _end)
			plots_Targets[i]->update(numbers[i - _start]);
	}
}

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
		float plotHeight;
		float plotMargin = 3;
		float _size = plots_Targets.size();

		if (bGui_AllPlots)
			plotHeight = (_h - 2 * plotMargin) / (float)_size; // draw all plots
		else
			plotHeight = (_h - 2 * plotMargin) / (float)numPlotsEnable; // draw only enable plots

		int x, y, w, h;

		int p = 0; // accumulate how many we have draw to not draw empty spaces

		for (int i = 0; i < _size; i++)
		{
			if (bGui_AllPlots)
				y = plotMargin + plotHeight * i;
			else
				y = plotMargin + plotHeight * (p);

			y += _y;
			x = _x + plotMargin;
			w = _w - 2 * plotMargin;
			h = plotHeight - plotMargin;

			if (plots_Selected[i] || bGui_AllPlots)
			{
				plots_Targets[i]->draw(x, y + plotMargin, w, h);

				p++;
			}

			//----

			// Extra widgets

			float xpad = 40;
			float r = (h * 0.8 * 0.5);
			int _start, _end;

			//----

			// CircleBeats

			//--

			// Bangs

			_start = 0;
			_end = _start + NUM_BANGS;
			if (i >= _start && i < _end) {
				bangCircles[i].draw(glm::vec2(x + r + xpad, y + plotMargin + h / 2.0f), r);
			}

			//--

			// Toggles

			_start = _end;
			_end = _start + NUM_TOGGLES;
			if (i >= _start && i < _end) {
				togglesCircles[i - _start].draw(glm::vec2(x + r + xpad, y + plotMargin + h / 2.0f), r);
			}

			//--

			// Bars
			// Values

			_start = NUM_BANGS + NUM_TOGGLES;
			_end = _start + NUM_VALUES;
			float _gap = h * 0.1;

			if (i >= _start && i < _end && i != _end - 1) {//exclude bpm bc out of range
				float v = values[i - _start];

				//barValues[i - _start].draw(
				//	v,
				//	glm::vec2(x + xpad, y + plotMargin + h),
				//	2 * r, -(h - _gap));

				barValues[i - _start].draw(
					v,
					glm::vec2(x + xpad, y + plotMargin + h - 2),
					2 * r, h - _gap);
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
		ofPushStyle();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(boxPlotsBg.getRectangle(), 5);
		boxPlotsBg.draw();
		ofPopStyle();
		drawPlots(boxPlotsBg.getRectangle());
	}

	// Full Screen

	else
	{
		ofRectangle r = ofGetCurrentViewport();
		ofPushStyle();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(r, 5);
		ofPopStyle();
		drawPlots(r);
	}
}

//--------------------------------------------------------------
ofxHistoryPlot* ofxSurfingOsc::addGraph(string varName, float max, ofColor color, int precision, bool _smooth)
{
	float _line = 1.0f;

	ofxHistoryPlot* graph = new ofxHistoryPlot(NULL, varName, max, false);//true for autoupdate

	//graph->setRangeAuto();
	//graph->setAutoRangeShrinksBack(true);
	//graph2 scale can shrink back after growing if graph2 curves requires it
	graph->setRange(0, max); // this lock the auto range!

	if (_smooth) {
		graph->setShowSmoothedCurve(true);
		graph->setSmoothFilter(0.1);
	}

	// it looks like this can cause problems, maybe bc translating
	graph->setCropToRect(true);
	graph->setRespectBorders(true);

	graph->setShowNumericalInfo(true);
	graph->setPrecision(precision);

	graph->setLineWidth(_line);
	graph->setColor(color);
	graph->setDrawBackground(true);
	graph->setBackgroundColor(ofColor(0, 225));
	graph->setDrawGrid(false);

	// speed
	//graph->setMaxHistory(2000);
	graph->setMaxHistory(60 * 20);

	//graph2->setLowerRange(0);//set only the lowest part of the range upper is adaptative to curve
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
void ofxSurfingOsc::update_PatchingManager() {

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
		//patchingManager.pipeValues[i].input = plots_Targets[i + NUM_BANGS + NUM_TOGGLES];//smoothed
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		patchingManager.pipeNumbers[i].input = numbers[i];
	}

	//--

	// Custom Template
	
	if (bCustomTemplate)
	{
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


//----


//TODO:
//--------------------------------------------------------------
//void ofxSurfingOsc::addReceiver(ofAbstractParameter &e, string address)
//{
//	if (!bUseOscIn) return;
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