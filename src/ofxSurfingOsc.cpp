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
	path_AppSettings = "App_Settings.xml";
	path_OscSettings = "OSC_Settings.xml";
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
		break;

	case ofxSurfingOsc::Master:
		setEnableOscInput(false);
		setEnableOscOutput(true);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode Master";
		break;

	case ofxSurfingOsc::Slave:
		setEnableOscInput(true);
		setEnableOscOutput(false);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode Slave";
		break;

	case ofxSurfingOsc::FullDuplex:
		setEnableOscInput(true);
		setEnableOscOutput(true);
		ofLogNotice("ofxSurfingOsc") << "Setup Mode FullDuplex";
		break;
	}

}

//----------------------------------------------------
void ofxSurfingOsc::setup()
{
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	bDISABLE_CALLBACKS = false;

	//--

#ifdef USE_TEXT_FLOW
	setupTextFlow();
#endif

	//--

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	string path = "assets/fonts/JetBrainsMono-Bold.ttf";
	fontSize = 7;
	bool b = myFont.loadFont(path, fontSize);
	if (!b) myFont.loadFont(OF_TTF_MONO, fontSize);

	//--

	// Plots
	// draggable bg
	{
		boxPlotsBg.bEditMode.setName("EDIT PLOTS");
		//boxPlotsBg.bEditMode.makeReferenceTo(patchingManager.boxPlotsBg.bEditMode);

		// default plots position
		//ofRectangle r = ofGetCurrentViewport();
		if (0)
		{
			int w = 300;
			int pad = 30;
			ofRectangle r = ofRectangle(ofGetWidth() - w - 2 * pad, pad, w, ofGetHeight() - 2 * pad);
			boxPlotsBg.setRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());
		}

		ofColor c0(0, 90);
		boxPlotsBg.setColorEditingHover(c0);
		boxPlotsBg.setColorEditingMoving(c0);
		boxPlotsBg.enableEdit();
	}

	//--

	//TODO:

	// Help box
	boxHelp.bGui.setName("Help");
	//boxHelp.setTextMode(true);
	boxHelp.setPath(path_Global);
	boxHelp.setup();
	//boxHelp.setTheme(false);//light

	//--

	setupParams();
}

//----------------------------------------------------
void ofxSurfingOsc::buildHelp() {
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	strHelpInfo = "";
	strHelpInfo += "SURFING OSC \n";
	strHelpInfo += "\n";

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


	boxHelp.setText(strHelpInfo);
	boxHelp.bGui.set(false);
}

//----------------------------------------------------
void ofxSurfingOsc::disconnectOscInput() {
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	ofxUnsubscribeOsc();
}

//----------------------------------------------------
void ofxSurfingOsc::setupOscInput()
{
	//ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);

	//-

	// Log incoming OSC messages

	if (bUseOscIn)
	{
		ofxGetOscSubscriber(OSC_InputPort);
		// create subscriber binding 26666 explicitly

		//NOTE:
		//must add [this] into lambda:
		//https://stackoverflow.com/questions/26903602/an-enclosing-function-local-variable-cannot-be-referenced-in-a-lambda-body-unles

		ofxSubscribeAllOsc([&](const ofxOscMessageEx& m,
			bool is_leaked)
			{
				bool _b;
				_b |= bGui_Debug;
#ifdef USE_TEXT_FLOW
				_b |= (bGui_Log);
#endif

				if (_b)
				{
					// Unrecognized message: 
					// display on the bottom of the screen

					string msgString;
					msgString = m.getAddress();
					//msgString += ":";

					for (size_t i = 0; i < m.getNumArgs(); i++) {

						// Get the argument type

						//msgString += " ";
						msgString += "\t\t";

						//show type too:
						//msgString += m.getArgTypeName(i);
						//msgString += ":";

						// display the argument 
						// make sure we get the right type
						if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
							msgString += ofToString(m.getArgAsInt32(i));
						}
						else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
							msgString += ofToString(m.getArgAsFloat(i));
						}
						else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
							msgString += m.getArgAsString(i);
						}
						else {
							msgString += "unhandled argument type " + m.getArgTypeName(i);
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

					ofLogNotice("ofxSurfingOsc") << "OscIn \t" << msgString;
					//ofLogNotice(__FUNCTION__) << "\t" << msgString;

#ifdef USE_TEXT_FLOW
					ofxTextFlow::addText(msgString);
#endif
				}
			});
	}

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
		//params_OscSettings.add(bEnableOsc_Output);//TODO: cant be disabled once publishers are added..
		// Looking for something like: https://github.com/2bbb/ofxPubSubOsc/issues/41

		params_OscSettings.add(OSC_OutputPort);
		params_OscSettings.add(OSC_OutputIp);
	}

	//--

	ofAddListener(params_OscSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_SettingsOSC);

	// include
	OSC_InputPort.setSerializable(true);
	OSC_OutputPort.setSerializable(true);
	OSC_OutputIp.setSerializable(true);

	// exclude
	str_OSC_InputPort.setSerializable(false);
	str_OSC_OutputPort.setSerializable(false);

	if (bUseOscIn) str_OSC_InputPort = ofToString(OSC_InputPort);
	if (bUseOscOut) str_OSC_OutputPort = ofToString(OSC_OutputPort);
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

	// OSC

	// Debug Info
	{
		// debug and show all subscribed addresses
		// process all subscribed addresses

		if (bUseOscIn)
		{
			str_oscInputAddressesInfo = "INPUT OSC ADDRESSES \n";
			for (auto a : strs_inputAddresses)
			{
				str_oscInputAddressesInfo += "\n";
				str_oscInputAddressesInfo += a;
			}
		}

		if (bUseOscOut)
		{
			str_oscOutputAddressesInfo = "OUTPUT OSC ADDRESSES \n";
			for (auto a : strs_outputAddresses)
			{
				str_oscOutputAddressesInfo += "\n";
				str_oscOutputAddressesInfo += a;
			}
		}

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

		//if (bUseOscIn) str_oscAddressesInfo += str_oscInputAddressesInfo;
		//if (bUseOscOut) str_oscAddressesInfo += str_oscOutputAddressesInfo;

		//--

		// Debug Info Box
		//TODO: replace with a text box widget!

		if (myFont.isLoaded())
			rectDebug = myFont.getStringBoundingBox(str_oscAddressesInfo, 0, 0);
		else
			rectDebug = ofRectangle(100, 100, 100, 100);

		float pad = 8;
		rectDebug.x = rectDebug.x - pad;
		rectDebug.y = rectDebug.y - pad;
		rectDebug.width = rectDebug.width + 2 * pad;
		rectDebug.height = rectDebug.height + 2 * pad;
	}

	//--

	guiInternal.setPosition(positionGuiInternal.get().x, positionGuiInternal.get().y);

	//----

	// Startup

	// Default settings
	{
		OSC_InputPort = 12345;
		OSC_OutputPort = 54321;

		OSC_OutputIp = "127.0.0.1"; // local
		///OSC_OutputIp = "192.168.0.135"; // iPad
		///OSC_OutputIp = "192.168.0.122"; // Air
	}

	//----

	// Load File Settings
	ofxSurfingHelpers::loadGroup(params_OscSettings, path_Global + "/" + path_OscSettings);

	// Fix workaround
	if (bUseOscIn)
	{
		bEnableOsc_Input = bEnableOsc_Input;
	}

	buildHelp();
}

//----------------------------------------------------
void ofxSurfingOsc::setupParams()
{
	bGui.set("ofxSurfingOsc", true);
	bEnableOsc.set("Enable OSC", true);
	//bGui_OscHelper.set("Show OSC", false);

	bGui_Debug.set("Debug", false);
	bKeys.set("Keys", true);

	bGui_Plots.set("Plots", true);
	bEnableSmoothPlots.set("Smooth", false);
	smoothPlotsPower.set("Smooth Power", 0.5, 0, 1);
	bGui_AllPlots.set("All Plots", false);
	bModePlotsMini.set("Mini", false);

#ifdef USE_MIDI
	bGui_MIDI.set("Show MIDI", false);
#endif

#ifdef USE_TEXT_FLOW
	bGui_Log.set("Log", false);
#endif

#ifdef MODE_SLAVE_RECEIVER_PATCHING
	bGui_PatchingManager.set("Patching Manager", false);
#endif

	//--

	// OSC

	if (bUseOscIn) {
		bEnableOsc_Input.set("Enable Input", false);
		OSC_InputPort.set("OSC_Input_Port", 12345, 0, 99999);
		str_OSC_InputPort.set("OSC Input Port", "");//out
		str_OSC_InputPort = ofToString(OSC_InputPort);
	}

	if (bUseOscOut) {
		bEnableOsc_Output.set("Enable_Output", true);//TODO: cant be disabled once publishers are added..
		OSC_OutputPort.set("OSC_Output_Port", 54321, 0, 99999);
		str_OSC_OutputPort.set("OSC Output Port", "");//out
		str_OSC_OutputPort = ofToString(OSC_InputPort);
		OSC_OutputIp.set("Output IP", "127.0.0.1");
		bModeFeedback.set("Feedback", true);
	}

	if (bUseOscIn) {
		params_OscInput.setName("OSC INPUT");
		params_OscInput.add(bEnableOsc_Input);
		params_OscInput.add(str_OSC_InputPort);
	}

	if (bUseOscOut) {
		params_OscOutput.setName("OSC OUTPUT");
		///TODO: cant be disabled once publishers are added..
		//params_OscOutput.add(bEnableOsc_Output);
		// Looking for something like: https://github.com/2bbb/ofxPubSubOsc/issues/41
		params_OscOutput.add(str_OSC_OutputPort);
		params_OscOutput.add(OSC_OutputIp);
		params_OscOutput.add(bModeFeedback);
	}

	params_Osc.setName("OSC");

	if (bUseOscIn) params_Osc.add(params_OscInput);
	if (bUseOscOut) params_Osc.add(params_OscOutput);

#ifdef USE_TEXT_FLOW
	params_Osc.add(bGui_Log);
#endif

	//--

	params_PlotsInput.setName("PLOTS INPUT");
	params_PlotsInput.add(bGui_Plots);
	params_PlotsInput.add(bGui_AllPlots);
	params_PlotsInput.add(bModePlotsMini);
	//params_PlotsInput.add(boxPlotsBg.bEditMode);
	//params_PlotsInput.add(bEnableSmoothPlots);
	//params_PlotsInput.add(smoothPlotsPower);
	params_Osc.add(params_PlotsInput);

#ifdef MODE_SLAVE_RECEIVER_PATCHING
	params_Osc.add(bGui_PatchingManager);
#endif

	//--

#ifdef USE_MIDI

	// MIDI

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

	//-

	// Gui

	guiInternal.setup(bGui.getName());
	//guiInternal.add(bGui_OscHelper);

	// OSC
	guiInternal.add(params_Osc);

	// Midi
#ifdef USE_MIDI
	guiInternal.add(bGui_MIDI);
	guiInternal.add(params_MIDI);
#endif

	// Extra
	params_Extra.setName("Extra");
	params_Extra.add(bKeys);
	params_Extra.add(boxHelp.bGui);
	params_Extra.add(bGui_Debug);
	guiInternal.add(params_Extra);

	// Position
	positionGuiInternal.set("GUI POSITION",
		glm::vec2(500, 500),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight()));
	guiInternal.setPosition(positionGuiInternal.get().x, positionGuiInternal.get().y);

	//--

	// Callbacks

	ofAddListener(params_AppSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
	ofAddListener(params_Osc.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
#ifdef USE_MIDI
	ofAddListener(params_MIDI.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
#endif

	//--

	// Settings

	// Not into gui: just for store/recall and callbacks

	params_AppSettings.setName("ofxSurfingOsc");
	params_AppSettings.add(bGui_Debug);
	//params_AppSettings.add(bGui_OscHelper);

#ifdef USE_TEXT_FLOW
	params_AppSettings.add(bGui_Log);
#endif
	params_AppSettings.add(positionGuiInternal);

	//--

	// MIDI

#ifdef USE_MIDI
	params_AppSettings.add(bGui_MIDI);
#endif

#ifdef USE_MIDI
	params_AppSettings.add(bEnableMIDI);
	params_AppSettings.add(MIDI_InputPort);
#ifdef USE_MIDI_OUT
	params_AppSettings.add(MIDI_OutputPort);
#endif
#endif

	//--

	// OSC

	if (bUseOscIn)
	{
		params_AppSettings.add(OSC_InputPort);
	}
	if (bUseOscOut)
	{
		params_AppSettings.add(OSC_OutputPort);
		params_AppSettings.add(str_OSC_OutputPort);
		params_AppSettings.add(OSC_OutputIp);
	}

	//--

#ifdef MODE_SLAVE_RECEIVER_PATCHING
	params_AppSettings.add(bGui_PatchingManager);
#endif

	// Plots

	params_AppSettings.add(params_PlotsInput);

	/*
	params_AppSettings.add(bGui_Plots);
	params_AppSettings.add(bGui_AllPlots);
	params_AppSettings.add(bModePlotsMini);
	//params_AppSettings.add(bEnableSmoothPlots);//crashes
	*/

	//--

	//TODO:
	// Exclude settings for this group
	OSC_InputPort.setSerializable(false);
	OSC_OutputPort.setSerializable(false);
	OSC_OutputIp.setSerializable(false);

	//--

	// Load Settings

	ofxSurfingHelpers::loadGroup(params_AppSettings, path_Global + "/" + path_AppSettings);

	refreshGui();

	//--

	//// Load file with midi assignments to params
	//mMidiParams.load(path_Global + "/" + "midi-params.xml");

	//--

	//TODO:
	// Debug info
	// should improve performance building list once
	// to show subscribed OSC addresses
	strs_inputAddresses.clear();
	strs_outputAddresses.clear();

	//--

	setupOscInput();

	//--
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params_SettingsOSC(ofAbstractParameter& e)
{
	string name = e.getName();
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

	// Settings

	if (bUseOscIn)
	{
		if (name == OSC_InputPort.getName())
		{
			str_OSC_InputPort = ofToString(OSC_InputPort);

			buildHelp();
		}

		//if (name == bEnableOsc_Input.getName())
		//{
		//	if (!bEnableOsc_Output) {
		//		disconnectOscInput();
		//	}
		//	else {
		//		setupOscInput();
		//	}
		//}
	}

	//--

	if (bUseOscOut)
	{
		if (name == OSC_OutputPort.getName())
		{
			str_OSC_OutputPort = ofToString(OSC_OutputPort);

			buildHelp();
		}

		if (name == OSC_OutputIp.getName())
		{
			buildHelp();
		}

		if (name == bEnableOsc_Output.getName())
		{
			//looking for something like: https://github.com/2bbb/ofxPubSubOsc/issues/41
			////ofxSetOscSubscriberActive(OSC_InputPort, bENABLE_Input);
			//if (!bEnableOsc_Input) {
			//}
			//else {
			//	setupOscInput();
			//}

			buildHelp();
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
	if (ofGetFrameNum() == 1) bEnableMIDI = bEnableMIDI;//workaround
#endif

	updateManager();
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

#ifdef MODE_SLAVE_RECEIVER_PATCHING
//--------------------------------------------------------------
void ofxSurfingOsc::draw_PatchingManager() {

	////auto p = guiInternal.getPosition();
	////auto h = guiInternal.getHeight();
	//auto p = patchingManager.gui.getPosition();
	//auto h = patchingManager.gui.getHeight();
	//patchingManager.setPositionPreview(glm::vec2(p.x + 5, p.y + h + 5));

	patchingManager.draw();
}
#endif

//--------------------------------------------------------------
void ofxSurfingOsc::draw()
{
	if (!bGui) return;

	//--

	if (bGui_Plots)
	{
		drawPlots();
	}

	//--

//#ifdef INCLUDE_GUI
//		if (bGui)
//		{
//			gui_TARGETS.draw();
//		}
//#endif

	//--

	// GUI
	{
		guiInternal.draw();

		auto p = guiInternal.getPosition();
		auto w = guiInternal.getWidth();
		auto h = guiInternal.getHeight();

		// OSC debug 
		// 
		// show all subscribed addresses
		if (bGui_Debug)
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

		// OSC log

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
	}

	//--

#ifdef MODE_SLAVE_RECEIVER_PATCHING
	if (bGui_PatchingManager) draw_PatchingManager();
#endif

	//--

	boxHelp.draw();
}

//--------------------------------------------------------------
void ofxSurfingOsc::exit()
{
	ofLogWarning("ofxSurfingOsc") << (__FUNCTION__);

	ofRemoveListener(params_AppSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
	ofRemoveListener(params_Osc.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
	ofRemoveListener(params_OscSettings.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_SettingsOSC);

#ifdef USE_MIDI
	ofRemoveListener(params_MIDI.parameterChangedE(), this, &ofxSurfingOsc::Changed_params);
#endif

	positionGuiInternal = guiInternal.getPosition();

	// exclude settings for this group
	OSC_InputPort.setSerializable(false);
	OSC_OutputPort.setSerializable(false);
	OSC_OutputIp.setSerializable(false);
	ofxSurfingHelpers::saveGroup(params_AppSettings, path_Global + "/" + path_AppSettings);

	// include settings for this group
	OSC_InputPort.setSerializable(true);
	OSC_OutputPort.setSerializable(true);
	OSC_OutputIp.setSerializable(true);
	ofxSurfingHelpers::saveGroup(params_OscSettings, path_Global + "/" + path_OscSettings);

	//-

#ifdef USE_MIDI_OUT
	// clean up				
	midiOut.closePort();
#endif
}

//--

// Note that some settings can not be changed without restart the app!
// 
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

//--

//TODO:
//--------------------------------------------------------------
//void ofxSurfingOsc::addReceiver(ofAbstractParameter &e, string address)
//{
//	if (!bUseOscIn) return;
//  ofxSubscribeOsc(12345, address, &e);
//}
//

//--

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

//----

// Receivers

//--------------------------------------------------------------
void ofxSurfingOsc::addReceiver_Int(ofParameter<int>& p, string address)
{
	if (!bUseOscIn) {
		ofLogError("ofxSurfingOsc") << (__FUNCTION__);
		ofLogError("ofxSurfingOsc") << "OSC Input is disabled. That param " << p.getName() << " will not be registered!";
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
		ofLogError("ofxSurfingOsc") << "OSC Input is disabled. That param " << p.getName() << " will not be registered!";
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

// Senders

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

//--------------------------------------------------------------
ofxSurfingOsc::~ofxSurfingOsc()
{
	ofLogWarning("ofxSurfingOsc") << (__FUNCTION__);

	ofRemoveListener(ofEvents().update, this, &ofxSurfingOsc::update);
	ofRemoveListener(ofEvents().draw, this, &ofxSurfingOsc::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingOsc::keyPressed);

	exit();
}

// control callbacks
//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params(ofAbstractParameter& e)
{
	string name = e.getName();
	ofLogNotice("ofxSurfingOsc") << (__FUNCTION__);
	ofLogNotice("ofxSurfingOsc") << name << " : " << e;

#ifdef USE_TEXT_FLOW
	if (0) ofxTextFlow::addText(name + " : " + ofToString(e));
#endif

	//--

	if (false) {}

#ifdef USE_TEXT_FLOW
	else if (name == bGui_Log.getName())
	{
		ofxTextFlow::setShowing(bGui_Log);
	}
#endif

	//--

	// OSC

	//else if (name == "ENABLE OSC")
	//{
	//}

	//TODO: required?
	// settings
	// all are strings

	//--

	// Input

	if (bUseOscIn)
	{
		if (name == str_OSC_InputPort.getName())
		{
			//TODO: should verify that correspond to a valid port structure!
			int p = ofToInt(str_OSC_InputPort);

			setInputPort(p);
		}

		else if (name == bEnableOsc_Input.getName())
		{
			ofxSetOscSubscriberActive(OSC_InputPort, bEnableOsc_Input);

			//bool b = ofxGetOscSubscriberActive(OSC_InputPort);
			//cout << "ofxGetOscSubscriberActive: " << b << endl;
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
		}

		//else if (name == OSC_OutputIp.getName())
		//{
		//	setOutputIp(OSC_OutputIp);
		// // not required, it's the same param, there's no int
		//}
	}

	//--

	//else if (name == bGui_OscHelper.getName())
	//{
	//	refreshGui();
	//}

	//--


	//TODO:
#ifdef MODE_SLAVE_RECEIVER_PATCHING
	if (name == bGui_PatchingManager.getName())
	{
		if (bGui_PatchingManager) {
		}
	}
#endif

	//--

	// Smooth
	if (name == bEnableSmoothPlots.getName())
	{
		int _start = NUM_BANGS + NUM_TOGGLES;

		for (int i = _start; i < _start + NUM_VALUES; i++)
		{
			if (i > plots_TARGETS.size() - 1) return;
			plots_TARGETS[i]->setShowSmoothedCurve(bEnableSmoothPlots);
		}
	}
	else if (name == smoothPlotsPower.getName())
	{
		if (bEnableSmoothPlots) {
			int _start = NUM_BANGS + NUM_TOGGLES;

			for (int i = _start; i < _start + NUM_VALUES; i++)
			{
				if (i > plots_TARGETS.size() - 1) return;
				plots_TARGETS[i]->setSmoothFilter(0.1);
			}
		}
	}

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
	}
	else if (name == bGui_MIDI.getName())
	{
		refreshGui();
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
	}

#endif//midiOut

#endif//midi

	//--

#ifdef USE_TEXT_FLOW
	if (name == bGui_Log.getName())
	{
		// workflow
		if (bGui_Log) bGui_OscHelper = true;
		//if (!bGui_OscHelper && bGui_Log) bGui_OscHelper = true;
	}
#endif

	//--

	if (name == positionGuiInternal.getName())
	{
		guiInternal.setPosition(positionGuiInternal.get().x, positionGuiInternal.get().y);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::refreshGui()
{

#ifdef USE_MIDI

	auto& gm = guiInternal.getGroup(params_MIDI.getName());
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

	auto& ge = guiInternal.getGroup(params_Extra.getName());
	ge.minimize();

	auto& go = guiInternal.getGroup(params_Osc.getName());
	if (bUseOscOut) go.getGroup(params_OscOutput.getName()).minimize();
	if (bUseOscIn) go.getGroup(params_OscInput.getName()).minimize();
	go.getGroup(params_PlotsInput.getName()).minimize();
	go.minimize();
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

////--------------------------------------------------------------
//void ofxSurfingOsc::loadParams(ofParameterGroup& g, string path)
//{
//	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";
//	ofLogNotice(__FUNCTION__) << path;
//
//	ofXml settings;
//	settings.load(path);
//	ofLogNotice(__FUNCTION__) << "params: \n" << settings.toString();
//	ofDeserialize(settings, g);
//}
//
////--------------------------------------------------------------
//void ofxSurfingOsc::ofxSurfingHelpers::saveGroup(ofParameterGroup& g, string path)
//{
//	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";
//	ofLogNotice(__FUNCTION__) << path;
//
//	ofXml settings;
//	ofSerialize(settings, g);
//	ofLogNotice(__FUNCTION__) << "params: \n" << settings.toString();
//	settings.save(path);
//}

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


#ifndef USE_ADDON_PARAMS

//#define INCLUDE_GUI

//--------------------------------------------------------------
void ofxSurfingOsc::setup(bool standardTemplate) {

	//// colors
	//colorBangs = ofColor::aquamarine;
	//colorToggles = ofColor::aliceBlue;
	//colorValues = ofColor::cyan;
	//colorNumbers = ofColor::deepSkyBlue;

	//// greys
	//int _min = 0;
	//int _max = 128;
	//colorBangs = ofColor((int)ofMap(0, 0, 1, _min, _max));
	//colorToggles = ofColor((int)ofMap(0.2, 0, 1, _min, _max));
	//colorValues = ofColor((int)ofMap(0.7, 0, 1, _min, _max));
	//colorNumbers = ofColor((int)ofMap(1, 0, 1, _min, _max));

	// dark
	int _dark = 24;
	colorBangs = ofColor(_dark);
	colorToggles = ofColor(_dark);
	colorValues = ofColor(_dark);
	colorNumbers = ofColor(_dark);

	//-

	setup();
	setupManager();
	startup();
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupManager() {

	// 1. params

	// create params used to receive from the addon class

	// NOTE:
	// param names goes from 1 to 8, not starting at 0 like vectors

	//-

	// Bangs / triggers

	params_Bangs.setName("BANGS");
	for (int i = 0; i < NUM_BANGS; i++)
	{
		string _name = "BANG_" + ofToString(i + 1);

		//create trigger bools params
		bBangs[i].set(_name, false);
		params_Bangs.add(bBangs[i]);

		//plots
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorBangs, 0);
		plots_TARGETS.push_back(graph);

		//beatCircles
		bangCircles[i].setColor(colorBangs);
	}
	ofAddListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGET_Bangs);

	//-

	// Toggles / state on-off

	params_Toggles.setName("TOGGLES");
	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string _name = "TOGGLE_" + ofToString(i + 1);

		// create toggle bools params
		bToggles[i].set(_name, false);
		params_Toggles.add(bToggles[i]);

		// plots
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorToggles, 0);
		plots_TARGETS.push_back(graph);

		// beatCircles
		togglesCircles[i].setColor(colorToggles);
	}
	ofAddListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Toggles);

	//-

	// Values / floats

	params_Values.setName("VALUES");
	for (int i = 0; i < NUM_VALUES; i++)
	{
		string _name = "VALUE_" + ofToString(i + 1);

		// create values params
		values[i].set(_name, 0.f, 0.f, 1.f);
		params_Values.add(values[i]);

		// plots
		ofxHistoryPlot* graph = addGraph(_name, 1.0f, colorValues, 2, false);// no smooth
		plots_TARGETS.push_back(graph);

		// bars
		barValues[i].setColor(colorValues);
	}
	ofAddListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Values);

	//-

	// Numbers / ints

	params_Numbers.setName("NUMBERS");
	int _max = 1000;
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string _name = "NUMBER_" + ofToString(i + 1);

		// create numbers params
		numbers[i].set(_name, 0, 0, _max);
		params_Numbers.add(numbers[i]);

		// plots
		ofxHistoryPlot* graph = addGraph(_name, _max, colorNumbers, 0);
		plots_TARGETS.push_back(graph);
	}
	ofAddListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Numbers);

	//-

	// customize label and selected plots
	customizePlots_TARGETS();

	//----

	// widgets for all the params they are:
	// 1. updating when receiving OSC/MIDI messages 
	// or 2. sending OSC/MIDI messages when params changes locally
	// that will depends on how params are subcribed to the class: 
	//addReceiver/addSender -> OSC in/out

	params_TARGETS.setName("TARGET PARAMS");
	params_TARGETS.add(params_Bangs);
	params_TARGETS.add(params_Toggles);
	params_TARGETS.add(params_Values);
	params_TARGETS.add(params_Numbers);

	//--

#ifdef INCLUDE_GUI
	// Gui panel
	gui_TARGETS.setup("ofxSurfingOsc SLAVE");

	// Control
	//gui_TARGETS.add(bGui_Control);

	gui_TARGETS.add(params_TARGETS);
	gui_TARGETS.setPosition(100, 100);
#endif

	//--

	// Subscribe all parameters to OSC incomming messages
	setupSubscribersManager();

	//--

#ifdef MODE_SLAVE_RECEIVER_PATCHING
	patchingManager.setup("PATCHING MANAGER");
#endif
}

//--------------------------------------------------------------
void ofxSurfingOsc::customizePlots_TARGETS() {

	// Array with bool of display state of any plot
	plotsSelected_TARGETS.clear();
	plotsSelected_TARGETS.resize(NUM_PLOTS);
	for (int _i = 0; _i < plotsSelected_TARGETS.size(); _i++)
		plotsSelected_TARGETS[_i] = false; // disable all plots by default

	//----

	// Customize special plots

	// template is for my audioanalyzer
	int _i;
	int _start;

	//-

	// bangs
	_start = 0;

	// bang 1 as beat
	_i = _start + 1;
	plots_TARGETS[_i - 1]->setVariableName("BEAT");
	plots_TARGETS[_i - 1]->setColor(ofColor::green);
	plotsSelected_TARGETS[_i - 1] = true;
	bangCircles[_i - 1].setColor(ofColor::green);

	// bang 2 as onset
	_i = _start + 2;
	plots_TARGETS[_i - 1]->setVariableName("ONSET");
	plots_TARGETS[_i - 1]->setColor(ofColor::red);
	plotsSelected_TARGETS[_i - 1] = true;
	bangCircles[_i - 1].setColor(ofColor::red);

	//--

	// Toggles
	_start = NUM_BANGS;

	//toggle 1 as note
	_i = _start + 1;
	plots_TARGETS[_i - 1]->setVariableName("NOTE");
	plots_TARGETS[_i - 1]->setColor(ofColor::orange);
	plotsSelected_TARGETS[_i - 1] = true;
	togglesCircles[_i - 1 - _start].setColor(ofColor::orange);

	//-

	// Values
	_start = NUM_BANGS + NUM_TOGGLES;

	// Value 1 
	_i = _start + 1;
	plots_TARGETS[_i - 1]->setVariableName("AUBIO");
	plots_TARGETS[_i - 1]->setColor(ofColor::red);
	plotsSelected_TARGETS[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::red);

	// Value 2 
	_i = _start + 2;
	plots_TARGETS[_i - 1]->setVariableName("GIST");
	plots_TARGETS[_i - 1]->setColor(ofColor::orange);
	plotsSelected_TARGETS[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::orange);

	//// Value 3 
	//_i = _start + 3;
	//plots_TARGETS[_i - 1]->setVariableName("CIRCLE AUBIO");
	//plots_TARGETS[_i - 1]->setColor(ofColor::green);
	//plotsSelected_TARGETS[_i - 1] = true;

	//// Value 4
	//_i = _start + 4;
	//plots_TARGETS[_i - 1]->setVariableName("CIRCLE GIST");
	//plots_TARGETS[_i - 1]->setColor(ofColor::orange);
	//plotsSelected_TARGETS[_i - 1] = true;

	// Value 8
	_i = _start + 8;//last value
	plots_TARGETS[_i - 1]->setRange(40, 300);
	plots_TARGETS[_i - 1]->setVariableName("BPM");
	plots_TARGETS[_i - 1]->setPrecision(0);
	plots_TARGETS[_i - 1]->setColor(ofColor::lightPink);
	plotsSelected_TARGETS[_i - 1] = true;
	barValues[_i - 1 - _start].setColor(ofColor::lightPink);

	//-

	// Count enabled plots to enable hide the all not used
	numPlostEnabled = 0;
	for (int _i = 0; _i < plotsSelected_TARGETS.size(); _i++)
	{
		if (plotsSelected_TARGETS[_i])
			numPlostEnabled++;
	}

	//cout << "numPlostEnabled:" << numPlostEnabled << endl;
}

//--------------------------------------------------------------
void ofxSurfingOsc::setupSubscribersManager()
{
	setModeFeedback(false);//disabled by default
	// enabled: received OSC/MIDI input messages are auto sended to the output OSC too

	string prefixPage_TARGET = "1/";

	//-

	// add OSC targets to the addon class
	// this will subscribe the OSC addresses with/to the target local parameters

	// NOTE:
	// '/1/' -> it's an address pattern to adapt to pages or layout
	// of some app controllers like TouchOSC

	//--

	// bangs
	for (int i = 0; i < NUM_BANGS; i++)
	{
		string OSC_address = "/bang/" + prefixPage_TARGET + ofToString(i + 1);
		addReceiver_Bool(bBangs[i], OSC_address);
	}

	//-

	// toggles
	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		//add target
		string OSC_address = "/toggle/" + prefixPage_TARGET + ofToString(i + 1);
		addReceiver_Bool(bToggles[i], OSC_address);
	}

	//-

	// values
	for (int i = 0; i < NUM_VALUES; i++)
	{
		//add target
		string OSC_address = "/value/" + prefixPage_TARGET + ofToString(i + 1);
		addReceiver_Float(values[i], OSC_address);
	}

	//-

	// numbers
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		//add target
		string OSC_address = "/number/" + prefixPage_TARGET + ofToString(i + 1);
		addReceiver_Int(numbers[i], OSC_address);
	}

	//--

	// startup

	// call this when all the local/target parameters are already added.
	startup();

	//--
}

//--------------------------------------------------------------
void ofxSurfingOsc::updateManager() {

	// Update plots
	for (int i = 0; i < plots_TARGETS.size(); i++)
	{
		int _start, _end;

		_start = 0;
		_end = NUM_BANGS;
		if (i >= _start && i < _end)
			plots_TARGETS[i]->update(bBangs[i]);//0?

		_start = _end;
		_end = _start + NUM_TOGGLES;
		if (i >= _start && i < _end)
			plots_TARGETS[i]->update(bToggles[i - _start]);

		_start = _end;
		_end = _start + NUM_VALUES;
		if (i >= _start && i < _end)
			plots_TARGETS[i]->update(values[i - _start]);

		_start = _end;
		_end = _start + NUM_NUMBERS;
		if (i >= _start && i < _end)
			plots_TARGETS[i]->update(numbers[i - _start]);
	}

	//--

#ifdef MODE_SLAVE_RECEIVER_PATCHING

	// IMPORTANT:
	// reads OSC messages (to final local receivers) 
	// after being processed by PatchingManager class:

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
		//patchingManager.pipeValues[i].input = plots_TARGETS[i + NUM_BANGS + NUM_TOGGLES];//smoothed
	}
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		patchingManager.pipeNumbers[i].input = numbers[i];
	}

	//--

	// customize some params:
	int _i = 8;
	patchingManager.pipeValues[_i - 1].setClamp(false);
	patchingManager.pipeValues[_i - 1].setRangeOutput(40, 300);

	//--

	// update pipe manager
	patchingManager.update(); // to process filters
#endif
}

//--------------------------------------------------------------
void ofxSurfingOsc::drawPlots(float _x, float _y, float _w, float _h)
{
	if (bGui_Plots)
	{
		float plotHeight;
		float plotMargin = 3;
		float _size = plots_TARGETS.size();

		if (bGui_AllPlots)
			plotHeight = (_h - 2 * plotMargin) / (float)_size; // draw all plots
		else
			plotHeight = (_h - 2 * plotMargin) / (float)numPlostEnabled; // draw only enable plots

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

			if (plotsSelected_TARGETS[i] || bGui_AllPlots)
			{
				plots_TARGETS[i]->draw(x, y + plotMargin, w, h);

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

		//--
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::drawPlots()
{
	if (!bGui_Plots) return;

	// Mini size
	if (bModePlotsMini)
	{
		float wPlotsView;
		float hPlotsView;
		float xPlotsView;
		float yPlotsView;

		xPlotsView = boxPlotsBg.getX();
		yPlotsView = boxPlotsBg.getY();
		wPlotsView = boxPlotsBg.getWidth();
		hPlotsView = boxPlotsBg.getHeight();

		ofPushStyle();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(boxPlotsBg, 5);
		boxPlotsBg.draw();
		ofPopStyle();

		drawPlots(xPlotsView, yPlotsView, wPlotsView, hPlotsView);
	}

	// Full screen
	else
	{
		wPlotsView = ofGetWidth();
		hPlotsView = ofGetHeight();
		xPlotsView = 0;
		yPlotsView = 0;
		drawPlots(xPlotsView, yPlotsView, wPlotsView, hPlotsView);
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::exitManager()
{
	ofRemoveListener(params_Bangs.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGET_Bangs);
	ofRemoveListener(params_Toggles.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Toggles);
	ofRemoveListener(params_Values.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Values);
	ofRemoveListener(params_Numbers.parameterChangedE(), this, &ofxSurfingOsc::Changed_params_TARGETS_Numbers);
}

// Local callbacks
// All the params will send 
// and receive OSC/MIDI changes

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params_TARGET_Bangs(ofAbstractParameter& e) // preset load/trig
{
	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		for (int i = 0; i < NUM_BANGS - 1; i++)
		{
			// only when true (BANG, not note-off)
			if (name == "BANG_" + ofToString(i + 1) && bBangs[i])
			{
				// bang
				ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "BANGS\t[" << ofToString(i + 1) << "] " << "BANG";

				// plot
				//plots_TARGETS[i]->update(1);

				bangCircles[i].bang();
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params_TARGETS_Toggles(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		for (int i = 0; i < NUM_TOGGLES - 1; i++)//?
		{
			if (name == "TOGGLE_" + ofToString(i + 1))
			{
				// state
				bool b = bToggles[i].get();
				ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "TOGGLE\t[" << ofToString(i + 1) << "] " << (b ? "ON" : "OFF");

				// first toggle (eg: gist note)
				if (b) { togglesCircles[i].bang(); }
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params_TARGETS_Values(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		for (int i = 0; i < NUM_VALUES - 1; i++)
		{
			if (name == "VALUE_" + ofToString(i + 1))
			{
				// values
				ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "VALUE\t[" << ofToString(i + 1) << "] " << values[i];
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingOsc::Changed_params_TARGETS_Numbers(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		for (int i = 0; i < NUM_NUMBERS - 1; i++)
		{
			if (name == "NUMBER_" + ofToString(i + 1))
			{
				// numbers
				ofLogVerbose("ofxSurfingOsc") << (__FUNCTION__) << "NUMBER\t[" << ofToString(i + 1) << "] " << numbers[i];
			}
		}
	}
}

//--

// Plots
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