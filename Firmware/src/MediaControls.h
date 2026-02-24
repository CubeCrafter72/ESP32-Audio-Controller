#pragma once

#include "Display.h"
#include "Screens.h"
#include "SerialHandler.h"
#include "StateInfo.h"
#include "RGBLed.h"

void initMediaControls(Display& display, SerialHandler& serial, RGBLed& led);

void onMasterVolumeChange(int value);
void onMicVolumeChange(int value);
void onLeftKnobChange(int value);
void onRightKnobChange(int value);

void togglePlayPause();
void nextTrack();
void previousTrack();

void reboot();