#include "PatchingManager.h"

//--------------------------------------------------------------
PatchingManager::PatchingManager() {
	bGui_InternalAllowed = false;
}

//--------------------------------------------------------------
PatchingManager::~PatchingManager() {
	exit();
}

//--------------------------------------------------------------
void PatchingManager::setup(string name)
{
	ofParameterGroup _gBangs{ "BANGS" };
	ofParameterGroup _gToggles{ "TOGGLES" };
	ofParameterGroup _gValues{ "VALUES" };
	ofParameterGroup _gNumbers{ "NUMBERS" };

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string name = "BANG_" + ofToString(i);
		pipeBangs[i].setup(name);
		_gBangs.add(pipeBangs[i].params);
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string name = "TOGGLE_" + ofToString(i);
		pipeToggles[i].setup(name);
		_gToggles.add(pipeToggles[i].params);
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string name = "VALUE_" + ofToString(i);
		pipeValues[i].setup(name);
		_gValues.add(pipeValues[i].params);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string name = "NUMBER_" + ofToString(i);
		pipeNumbers[i].setup(name, false);//not normalized
		_gNumbers.add(pipeNumbers[i].params);

		// Customize all ranges
		pipeNumbers[i].setRangeInput(0, 1000);
		pipeNumbers[i].setRangeOutput(0, 1000);
	}

	//--

	// Grouped

	params_Targets.setName("TARGETS");

	params_Targets.add(_gBangs);
	params_Targets.add(_gToggles);
	params_Targets.add(_gValues);
	params_Targets.add(_gNumbers);

	//--

	// Preview

	setupPreview();
	refreshPreview();

	//--

	bLock.set("Lock", false); // link preview to gui

	//--

	bGui_PreviewWidgets.makeReferenceTo(boxWidgets.bGui);

	// Gui
	if (bGui_InternalAllowed)
	{
		gui_Internal.setup(name);
		gui_Internal.add(bGui_PreviewWidgets);
		//gui_Internal.add(boxWidgets.bGui);
		gui_Internal.add(bLock);
		gui_Internal.add(params_Targets);

		// Position

		positionGui_Internal.set("Gui Position Internal",
			glm::vec2(550, 10),
			glm::vec2(0, 0),
			glm::vec2(ofGetWidth(), ofGetHeight()));
		gui_Internal.setPosition(positionGui_Internal.get().x, positionGui_Internal.get().y);

		//--

		// Minimize

		auto& g = gui_Internal.getGroup(params_Targets.getName());
		auto& gb = g.getGroup("BANGS");
		gb.minimize();
		auto& gt = g.getGroup("TOGGLES");
		gt.minimize();
		auto& gv = g.getGroup("VALUES");
		gv.minimize();
		auto& gn = g.getGroup("NUMBERS");
		gn.minimize();

		for (int i = 0; i < NUM_BANGS; i++)
		{
			string name = pipeBangs[i].params.getName();
			gb.getGroup(name).minimize();
		}

		for (int i = 0; i < NUM_TOGGLES; i++)
		{
			string name = pipeToggles[i].params.getName();
			gt.getGroup(name).minimize();
		}

		for (int i = 0; i < NUM_VALUES; i++)
		{
			string name = pipeValues[i].params.getName();
			gv.getGroup(name).minimize();
			auto& ge = gv.getGroup(name).getGroup("EXTRA");
			ge.getGroup("MAP").minimize();
			ge.minimize();
		}

		for (int i = 0; i < NUM_NUMBERS; i++)
		{
			string name = pipeNumbers[i].params.getName();
			gn.getGroup(name).minimize();
			auto& ge = gn.getGroup(name).getGroup("EXTRA");
			ge.getGroup("MAP").minimize();
			ge.minimize();
		}
	}

	//----

	//TODO:
	// Must learn handle lambda listeners
	// Workflow
	// Engine to manage all the other channels solo/enable states

	for (int i = 0; i < NUM_BANGS; i++)
	{
		listeners.push(pipeBangs[i].solo.newListener([&](ofParameter<bool> b) {
			ofLogNotice("ofxSurfingOsc") << ("PatchingManager") << "SOLO BANG " << ofToString(b.getName()) << (b ? " ON" : " OFF");
			ofLogNotice("ofxSurfingOsc") << ("PatchingManager") << ofToString(i) << endl;//?a random i?
			}));
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		listeners.push(pipeValues[i].solo.newListener([this](ofParameter<bool> b) {
			ofLogNotice("ofxSurfingOsc") << ("PatchingManager") << "SOLO VALUES " << ofToString(b.getName()) << (b ? " ON" : " OFF");
			}));
	}

	//--

	// Draggable Bg
	{
		boxWidgets.bGui.setName("Preview");
		boxWidgets.bEdit.setName("EDIT PREVIEW MANAGER");
		boxWidgets.setName("PatchingManager");
		boxWidgets.setPathGlobal(path_Global);
		boxWidgets.setup();

		// constraint sizes
		float f = 0.85f;
		glm::vec2 shapeMin(f * 240, f * 320);
		boxWidgets.setRectConstraintMin(shapeMin);
		glm::vec2 shapeMax(ofGetWidth(), ofGetHeight());
		boxWidgets.setRectConstraintMax(shapeMax);
	}

	//--

	// Settings

	params_Settings.setName(name);
	params_Settings.add(positionGui_Internal);
	params_Settings.add(boxWidgets.bGui);
	params_Settings.add(bLock);
	params_Settings.add(params_Targets);

	ofxSurfingHelpers::loadGroup(params_Settings, path_Global + "/" + path_Settings);

	if (bGui_InternalAllowed) {
		gui_Internal.setPosition(positionGui_Internal.get().x, positionGui_Internal.get().y);
	}
}

//--------------------------------------------------------------
void PatchingManager::update()
{
	////TODO:
	//for (int i = 0; i < NUM_BANGS; i++)
	//{
	//	pipeBangs[i].update();
	//}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		pipeToggles[i].update();
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		pipeValues[i].update();
	}

	//TODO:
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		pipeNumbers[i].update();
	}

	//--

	//TODO:
	//recalculate();//?
}

//--------------------------------------------------------------
void PatchingManager::doReset() {
	//TODO:
}

//--------------------------------------------------------------
void PatchingManager::draw()
{
	if (bGui_InternalAllowed)
	{
		if (bGui_Internal)
		{
			if (bLock)
			{
				//// Bottom
				//auto p = gui_Internal.getPosition();
				//auto h = gui_Internal.getHeight();

				// Top right
				auto p = gui_Internal.getShape().getTopRight();

				setPositionPreview(glm::vec2(p.x + 0, p.y));
			}

			//--

			gui_Internal.draw();
		}
	}

	//--

	//TODO:
	//widgets
	recalculate();//?

	if (bGui_Preview)
	{
		if (boxWidgets.bGui) drawPreview();
	}
}

//TODO:
//--------------------------------------------------------------
void PatchingManager::recalculate()
{
	for (int i = 0; i < NUM_BANGS; i++)
	{
		if (pipeBangs[i].output)
		{
			previewBangs[i].bang();
		}
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		if (pipeToggles[i].output)
		{
			previewToggles[i].bang();
		}
	}

	////?
	//for (int i = 0; i < NUM_VALUES; i++)
	//{
	//	if (pipeValues[i].output)
	//	{
	//		previewValues[i].bang();
	//	}
	//}
}

//--

// Mini mixer

//--------------------------------------------------------------
void PatchingManager::refreshPreview() {

	float x = boxWidgets.getX();
	float y = boxWidgets.getY();
	float w = boxWidgets.getWidth();
	float h = boxWidgets.getHeight();

	//--

	const float NUM_GAP_UNITS = 2;

	gapx = gapratio * w;
	gapy = gapratio * h;

	y = y + NUM_GAP_UNITS * gapy + szCircle / 2;
	x = x + NUM_GAP_UNITS * gapx;

	w -= NUM_GAP_UNITS * gapx;

	//--

	szCircle = (w / NUM_BANGS) - gapx;
	hSlider = ((h - 4 * gapy - 2 * szCircle) / 2) - (2 * gapy);

	//--

	for (int i = 0; i < NUM_BANGS; i++)
	{
		previewBangs[i].setSize(szCircle / 2);
		previewBangs[i].setPosition(glm::vec2(x + szCircle / 2 + i * (szCircle + gapx), y));
	}
	y = y + szCircle + 2 * gapy;

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		previewToggles[i].setSize(szCircle / 2);
		previewToggles[i].setPosition(glm::vec2(x + szCircle / 2 + i * (szCircle + gapx), y));
	}
	y = y + szCircle / 2 + 2 * gapy;

	for (int i = 0; i < NUM_VALUES; i++)
	{
		previewValues[i].setWidthMax(szCircle);
		previewValues[i].setHeightMax(hSlider);
		previewValues[i].setPosition(glm::vec2(x + i * (szCircle + gapx), y + hSlider));
	}
	y = y + hSlider + 1 * gapy;

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		previewNumbers[i].setWidthMax(szCircle);
		previewNumbers[i].setHeightMax(hSlider);
		previewNumbers[i].setPosition(glm::vec2(x + i * (szCircle + gapx), y + hSlider));
	}
}

//--------------------------------------------------------------
void PatchingManager::setupPreview() {

	//default styles
	for (int i = 0; i < NUM_BANGS; i++)
	{
		//previewBangs[i].setColor(ofColor::green);
		previewBangs[i].setColor(OF_COLOR_WIDGETS);
		previewBangs[i].setColorBackground(ofColor(0, 128));
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		//previewToggles[i].setColor(ofColor::orange);
		previewToggles[i].setColor(OF_COLOR_WIDGETS);
		previewToggles[i].setColorBackground(ofColor(0, 128));
		//previewToggles[i].setToggleMode(true);//?
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		//previewValues[i].setColor(ofColor::red);
		previewValues[i].setColor(OF_COLOR_WIDGETS);
		previewValues[i].setColorBackground(ofColor(0, 128));
		//previewValues[i].setRounded(_rounded);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		//previewNumbers[i].setColor(ofColor::yellow);
		previewNumbers[i].setColor(OF_COLOR_WIDGETS);
		previewNumbers[i].setColorBackground(ofColor(0, 128));
		//previewNumbers[i].setRounded(_rounded);
	}

	//--

	/*
	//TODO:
	// Customize
	// Template is for my ofxSoundAnalyzer add-on
	// Add method to set and correlate to parent styles..
	if (bCustomTemplate)
	{
		//previewBangs[0].setColor(ofColor::green);
		//previewBangs[1].setColor(ofColor::red);
		//previewBangs[2].setColor(ofColor::orange);
		//previewValues[0].setColor(ofColor::red);
		//previewValues[1].setColor(ofColor::orange);
		//previewValues[7].setColor(ofColor::lightPink);
		previewBangs[0].setColor(OF_COLOR_WIDGETS);
		previewBangs[1].setColor(OF_COLOR_WIDGETS);
		previewBangs[2].setColor(OF_COLOR_WIDGETS);
		previewValues[0].setColor(OF_COLOR_WIDGETS);
		previewValues[1].setColor(OF_COLOR_WIDGETS);
		previewValues[7].setColor(OF_COLOR_WIDGETS);
	}
	*/
}

//--------------------------------------------------------------
void PatchingManager::setupStyleWidget(int i, ofColor color) {

	if (i < NUM_BANGS)
	{
		previewBangs[i].setColor(color);
	}
	else if (i < NUM_BANGS + NUM_TOGGLES)
	{
		int start = NUM_BANGS;
		previewToggles[i - start].setColor(color);
	}
	else if (i < NUM_BANGS + NUM_TOGGLES + NUM_VALUES)
	{
		int start = NUM_BANGS + NUM_TOGGLES;
		previewValues[i - start].setColor(color);
	}
	else if (i < NUM_BANGS + NUM_TOGGLES + NUM_VALUES + NUM_NUMBERS)
	{
		int start = NUM_BANGS + NUM_TOGGLES + NUM_VALUES;
		previewNumbers[i - start].setColor(color);
	}
}

//TODO:
//--------------------------------------------------------------
void PatchingManager::updatePreview()
{
	//TODO:
	// Default customize to params min/max.

	for (int i = 0; i < NUM_VALUES; i++)
	{
		previewValues[i].setValueMin(pipeValues[i].minOutput);
		previewValues[i].setValueMax(pipeValues[i].maxOutput);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		previewNumbers[i].setValueMin(pipeNumbers[i].minOutput);
		previewNumbers[i].setValueMax(pipeNumbers[i].maxOutput);
	}

	//TODO:
	// Customize
	// Bpm
	if (bCustomTemplate)
	{
		previewValues[7].setValueMin(40);
		previewValues[7].setValueMax(300);
	}
}

//--------------------------------------------------------------
void PatchingManager::setPositionPreview(glm::vec2 position)
{
	boxWidgets.setPosition(position.x, position.y);
}

//--------------------------------------------------------------
void PatchingManager::drawPreview() {

	//TODO:
	updatePreview();

	//TODO:
	refreshPreview();

	//-

	ofPushStyle();
	{
		// Bg box
		ofFill();
		ofPushStyle();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(boxWidgets.getRectangle(), 5);
		boxWidgets.draw();
		ofPopStyle();

		//-

		// Widgets

		for (int i = 0; i < NUM_BANGS; i++)
		{
			//previewBangs[i].update();
			previewBangs[i].draw();
		}

		for (int i = 0; i < NUM_TOGGLES; i++)
		{
			//previewToggles[i].update();
			previewToggles[i].draw();
		}

		for (int i = 0; i < NUM_VALUES; i++)
		{
			previewValues[i].draw(pipeValues[i].output);
		}

		for (int i = 0; i < NUM_NUMBERS; i++)
		{
			previewNumbers[i].draw(pipeNumbers[i].output);
		}
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void PatchingManager::exit()
{
	if (bGui_InternalAllowed)
		positionGui_Internal = gui_Internal.getPosition();

	ofxSurfingHelpers::saveGroup(params_Settings, path_Global + "/" + path_Settings);
}