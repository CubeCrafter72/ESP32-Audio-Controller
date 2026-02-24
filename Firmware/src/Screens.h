#pragma once

#include "Display.h"

class Screens {
	public:
		static const Display::Screen VOLUME;
		static const Display::Screen APPS;
		static const Display::Screen TRACK_INFO;
		static const Display::Screen DISCONNECTED;
		static const Display::Screen CONNECTED;
		static const Display::Screen LOGO;

	private:
		static void drawVolumeScreen(Display& display);
		static void drawAppsScreen(Display& display);
		static void drawTrackInfoScreen(Display& display);
		static void drawDisconnectedScreen(Display& display);
		static void drawConnectedScreen(Display& display);
		static void drawLogoScreen(Display& display);
		static void drawVolume(
			Display& display, int leftVolume, int rightVolume,
			const unsigned char* leftGlyph, const unsigned char* rightGlyph
		);
};