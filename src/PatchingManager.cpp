#include "PatchingManager.h"

//--------------------------------------------------------------
PatchingManager::PatchingManager() {
}

//--------------------------------------------------------------
PatchingManager::~PatchingManager() {
	saveParams(params_Settings, path_Global + "/" + path_Settings);
}

//--------------------------------------------------------------
void PatchingManager::setup(string name) {
	params_Settings.setName(name);

	ofParameterGroup _gBangs{ "BANGS" };
	ofParameterGroup _gToggles{ "TOGGLES" };
	ofParameterGroup _gValues{ "VALUES" };
	ofParameterGroup _gNumbers{ "NUMBERS" };

	for (int i = 0; i < NUM_BANGS; i++)
	{
		string name = "BANG_" + ofToString(i + 1);
		pipeBangs[i].setup(name);
		_gBangs.add(pipeBangs[i].params);
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		string name = "TOGGLE_" + ofToString(i + 1);
		pipeToggles[i].setup(name);
		_gToggles.add(pipeToggles[i].params);
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		string name = "VALUE_" + ofToString(i + 1);
		pipeValues[i].setup(name);
		_gValues.add(pipeValues[i].params);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string name = "NUMBER_" + ofToString(i + 1);
		pipeNumbers[i].setup(name, false);
		_gNumbers.add(pipeNumbers[i].params);

		// customize all ranges
		pipeNumbers[i].setRangeInput(0, 1000);
		pipeNumbers[i].setRangeOutput(0, 1000);
	}

	// grouped
	params_Settings.add(_gBangs);
	params_Settings.add(_gToggles);
	params_Settings.add(_gValues);
	params_Settings.add(_gNumbers);

	//-

	// preview
	setupPreview();
	refreshPreviewLayout();

	//-

	// gui
	gui.setup(name);
	gui.setPosition(630, 20);

	gui.add(boxPlotsBg.bEditMode);
	gui.add(params_Settings);

	//-

	// minimize
	auto &g = gui.getGroup(name);
	auto &gb = g.getGroup("BANGS");
	gb.minimize();
	auto &gt = g.getGroup("TOGGLES");
	gt.minimize();
	auto &gv = g.getGroup("VALUES");
	gv.minimize();
	auto &gn = g.getGroup("NUMBERS");
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
	}
	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		string name = pipeNumbers[i].params.getName();
		gn.getGroup(name).minimize();
	}

	//-

	//TODO:
	// must learn handle lambda listeners
	// workflow
	// engine to manage all the other channels solo/enable states
	for (int i = 0; i < NUM_BANGS; i++)
	{
		listeners.push(pipeBangs[i].solo.newListener([&](ofParameter<bool> b) {
			ofLogNotice(__FUNCTION__) << "SOLO BANG " << ofToString(b.getName()) << (b ? " ON" : " OFF");
			ofLogNotice(__FUNCTION__) << ofToString(i) << endl;//?a random i?
		}));
	}
	for (int i = 0; i < NUM_VALUES; i++)
	{
		listeners.push(pipeValues[i].solo.newListener([this](ofParameter<bool> b) {
			ofLogNotice(__FUNCTION__) << "SOLO VALUES " << ofToString(b.getName()) << (b ? " ON" : " OFF");
		}));
	}
	//--

	// draggable bg
	{
		boxPlotsBg.bEditMode.setName("EDIT PREVIEW MANAGER");

		//// default plots position
		////ofRectangle r = ofGetCurrentViewport();
		//// init
		//if (0)
		//{
		//	int w = 300;
		//	int pad = 30;
		//	ofRectangle r = ofRectangle(ofGetWidth() - w - 2 * pad, pad, w, ofGetHeight() - 2 * pad);
		//	boxPlotsBg.setRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());
		//}

		// draggable rectangle
		ofColor c0(0, 90);
		//boxPlotsBg.set
		boxPlotsBg.setColorEditingHover(c0);
		boxPlotsBg.setColorEditingMoving(c0);
		boxPlotsBg.enableEdit();

		//boxPlotsBg.setAutoSave(true);
	}

	//-

	// settings
	loadParams(params_Settings, path_Global + "/" + path_Settings);
}

//--------------------------------------------------------------
void PatchingManager::update() {
	for (int i = 0; i < NUM_VALUES; i++)
	{
		pipeValues[i].update();
	}

	for (int i = 0; i < NUM_BANGS; i++)
	{
		pipeBangs[i].update();
	}
}

//--------------------------------------------------------------
void PatchingManager::doReset() {

}

//--------------------------------------------------------------
void PatchingManager::draw() {

	////TODO:
	////auto p = gui_Internal.getPosition();
	////auto h = gui_Internal.getHeight();
	//auto p = gui.getPosition();
	//auto h = gui.getHeight();
	//setPositionPreview(glm::vec2(p.x + 5, p.y + h + 5));

	//--

	if (bVisiblePreview) 
	{
		gui.draw();

		//-

		//TODO:
		recalculate();

		drawPreview();
	}
}

//--------------------------------------------------------------
void PatchingManager::exit() {
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

//--------------------------------------------------------------
void PatchingManager::loadParams(ofParameterGroup& g, string path)
{
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";
	ofLogNotice(__FUNCTION__) << path;

	ofXml settings;
	settings.load(path);
	ofLogNotice(__FUNCTION__) << "params: \n" << settings.toString();
	ofDeserialize(settings, g);
}

//--------------------------------------------------------------
void PatchingManager::saveParams(ofParameterGroup& g, string path)
{
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";
	ofLogNotice(__FUNCTION__) << path;

	ofXml settings;
	ofSerialize(settings, g);
	ofLogNotice(__FUNCTION__) << "params: \n" << settings.toString();
	settings.save(path);
}

//--

// mini mixer

//--------------------------------------------------------------
void PatchingManager::refreshPreviewLayout() {

	float x = boxPlotsBg.getX();
	float y = boxPlotsBg.getY();
	float w = boxPlotsBg.getWidth();
	float h = boxPlotsBg.getHeight();

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
		previewBangs[i].setRadius(szCircle / 2);
		previewBangs[i].setPosition(glm::vec2(x + szCircle / 2 + i * (szCircle + gapx), y));
	}
	y = y + szCircle + 2 * gapy;

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		previewToggles[i].setRadius(szCircle / 2);
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

	//--

	for (int i = 0; i < NUM_BANGS; i++)
	{
		previewBangs[i].setColor(ofColor::green);
	}

	for (int i = 0; i < NUM_TOGGLES; i++)
	{
		previewToggles[i].setColor(ofColor::orange);
	}

	for (int i = 0; i < NUM_VALUES; i++)
	{
		previewValues[i].setColor(ofColor::red);
	}

	for (int i = 0; i < NUM_NUMBERS; i++)
	{
		previewNumbers[i].setColor(ofColor::yellow);
	}

	//--

	// customize
	previewBangs[0].setColor(ofColor::green);
	previewBangs[1].setColor(ofColor::red);
	previewBangs[2].setColor(ofColor::orange);
	previewValues[0].setColor(ofColor::red);
	previewValues[1].setColor(ofColor::orange);
	previewValues[7].setColor(ofColor::lightPink);

	//--
}

//TODO:
//--------------------------------------------------------------
void PatchingManager::updatePreview() {

	//TODO:
	// default customize to params min/max
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
	// customize
	// bpm
	previewValues[7].setValueMin(40);
	previewValues[7].setValueMax(300);
}

//--------------------------------------------------------------
void PatchingManager::drawPreview() {

	//TODO:
	updatePreview();

	//TODO:
	refreshPreviewLayout();

	//-

	ofPushStyle();
	{
		// bg box
		ofFill();
		ofPushStyle();
		ofSetColor(OF_COLOR_BG_PANELS);
		ofDrawRectRounded(boxPlotsBg, 5);
		boxPlotsBg.draw();
		ofPopStyle();

		//-

		// widgets

		for (int i = 0; i < NUM_BANGS; i++)
		{
			previewBangs[i].update();
			previewBangs[i].draw();
		}

		for (int i = 0; i < NUM_TOGGLES; i++)
		{
			previewToggles[i].update();
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