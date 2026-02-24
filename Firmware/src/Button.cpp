#include "Button.h"

Button::Button(uint8_t pin, void (*onPress)(), void (*onLongPress)())
	: _pin(pin), _onPress(onPress), _onLongPress(onLongPress) {

	pinMode(_pin, INPUT_PULLUP);
}

void Button::update() {
	int currentState = digitalRead(_pin);
	unsigned long currentTime = millis();

	if (currentState == LOW && _lastState == HIGH) {

		if (currentTime - _lastPressedTime > DEBOUNCE_DELAY_MS) {
			_lastPressedTime = currentTime;
			_pressStartTime = currentTime;
			_longPressHandled = false; // reset flag
		}
	}

	if (currentState == LOW && !_longPressHandled && _onLongPress != nullptr) {
		if (currentTime - _pressStartTime >= LONG_PRESS_DURATION_MS) {
			_onLongPress();
			_longPressHandled = true;
		}
	}

	if (currentState == HIGH && _lastState == LOW) {
		if (currentTime - _lastPressedTime > DEBOUNCE_DELAY_MS) {
			_lastPressedTime = currentTime;

			if (!_longPressHandled && _onPress != nullptr) {
				_onPress();
			}
		}
	}

	_lastState = currentState;
}