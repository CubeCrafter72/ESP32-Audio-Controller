#pragma once

#include <Arduino.h>

class Button {
	public:
		Button(uint8_t pin, void (*onPress)(), void (*onLongPress)() = nullptr);

		void update();

	private:
		constexpr static int DEBOUNCE_DELAY_MS = 50;
		constexpr static int LONG_PRESS_DURATION_MS = 2000;

		uint8_t _pin;
		void (*_onPress)();
		void (*_onLongPress)();
		int _lastState{HIGH};
		unsigned long _lastPressedTime{0};
		unsigned long _pressStartTime{0};
		bool _longPressHandled{false};
};