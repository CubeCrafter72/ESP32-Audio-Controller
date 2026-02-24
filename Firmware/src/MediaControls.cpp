#include "MediaControls.h"

static Display* _displayPtr;
static SerialHandler* _serialPtr;
static RGBLed* _ledPtr;

void initMediaControls(Display& display, SerialHandler& serial, RGBLed& led) {
	_displayPtr = &display;
	_serialPtr = &serial;
	_ledPtr = &led;
}

void onMasterVolumeChange(int value) {
	if (stateInfo.masterVolume != -1) {
		_displayPtr->setScreen(Screens::VOLUME);
	}

	stateInfo.masterVolume = value;
	_serialPtr->send("master|" + String(stateInfo.masterVolume));
}

void onMicVolumeChange(int value) {
	if (stateInfo.micVolume != -1) {
		_displayPtr->setScreen(Screens::VOLUME);
	}

	stateInfo.micVolume = value;
	_serialPtr->send("mic|" + String(stateInfo.micVolume));
}

void onLeftKnobChange(int value) {
	if (stateInfo.leftKnob != -1) {
		_displayPtr->setScreen(Screens::APPS);
	}

	stateInfo.leftKnob = value;
	_serialPtr->send("left|" + String(stateInfo.leftKnob));
}

void onRightKnobChange(int value) {
	if (stateInfo.rightKnob != -1) {
		_displayPtr->setScreen(Screens::APPS);
	}

	stateInfo.rightKnob = value;
	_serialPtr->send("right|" + String(stateInfo.rightKnob));
}

void togglePlayPause() {
	_serialPtr->send("play");

	_ledPtr->setMode(RGBLed::Mode::SOLID, 200);
	_ledPtr->setColor(120, 60, 0);
}

void nextTrack() {
	_serialPtr->send("next");

	_ledPtr->setMode(RGBLed::Mode::SOLID, 200);
	_ledPtr->setColor(120, 60, 0);
}

void previousTrack() {
	_serialPtr->send("previous");

	_ledPtr->setMode(RGBLed::Mode::SOLID, 200);
	_ledPtr->setColor(120, 60, 0);
}

void reboot() {
	ESP.restart();
}