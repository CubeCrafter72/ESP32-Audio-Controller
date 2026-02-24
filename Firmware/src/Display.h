#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

class Display {
	public:
		struct Screen {
			void (*drawFunction)(Display&);
			unsigned long duration{0};
		};

		Display(uint8_t sclPin, uint8_t sdaPin, const u8g2_cb_t* rotation = U8G2_R2);

		void begin();
		void update();
		void setScreen(Screen screen);
		void setBaseScreen(Screen screen);
		void drawScrollingText(const char* text, const uint8_t* font, const int y, float& scrollPos);
		void drawText(const char* text, const uint8_t* font, const int y, int x = -1);

		U8G2_SH1106_128X64_NONAME_1_HW_I2C& u8g2() {
			return _display;
		}

	private:
		U8G2_SH1106_128X64_NONAME_1_HW_I2C _display;
		Screen _currentScreen{};
		Screen _baseScreen{};
		unsigned long _stateExpireTime{0};

		Screen getScreen();
};

extern Display display;