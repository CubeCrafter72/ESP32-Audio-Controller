#include <Arduino.h>
#include <U8g2lib.h>
#include "Display.h"
#include "StateInfo.h"

Display::Display(uint8_t sclPin, uint8_t sdaPin, const u8g2_cb_t* rotation)
	: _display(rotation, U8X8_PIN_NONE, sclPin, sdaPin) {
}

void Display::begin() {
	_display.begin();
}

void Display::setScreen(Screen screen) {
	_stateExpireTime = millis() + screen.duration;
	_currentScreen = screen;
}

void Display::setBaseScreen(Screen screen) {
	_baseScreen = screen;
}

void Display::update() {
	Screen state = getScreen();

	_display.setPowerSave(state.drawFunction == nullptr);

	if (!state.drawFunction) {
		return;
	}

	_display.firstPage();
	do {
		state.drawFunction(*this);
	} while (_display.nextPage());
}

Display::Screen Display::getScreen() {
	if (_currentScreen.drawFunction != nullptr && _stateExpireTime != 0 && millis() < _stateExpireTime) {
		return _currentScreen;
	} else if (_baseScreen.drawFunction != nullptr) {
		return _baseScreen;
	}
	
	return Screen{};
}

void Display::drawScrollingText(const char* text, const uint8_t* font, const int y, float& scrollPos) {
    _display.setFont(font);
    int textWidth = _display.getUTF8Width(text);

    if (textWidth <= 128) {
        int x = (128 - textWidth) / 2;
        _display.drawUTF8(x, y, text);
        return;
    }

    if (scrollPos < 0) {
        // Start pause
        scrollPos += 0.15;
        _display.drawUTF8(0, y, text);
    } else if (scrollPos > textWidth - 128) {
        // End pause
        scrollPos += 0.15;
        int x = 128 - textWidth;
        _display.drawUTF8(x, y, text);

        if (scrollPos > textWidth - 98) {  // 30 units * 0.25/frame wait
            scrollPos = -30.0;
        }
    } else {
        // Scrolling
        int x = 0 - (int) scrollPos;
        _display.drawUTF8(x, y, text);
        scrollPos += 0.15;
    }
}

void Display::drawText(const char* text, const uint8_t* font, const int y, int x) {
    _display.setFont(font);

	int textWidth = _display.getUTF8Width(text);
	if (x == -1) {
		x = (128 - textWidth) / 2;
	}
	_display.drawUTF8(x, y, text);
}