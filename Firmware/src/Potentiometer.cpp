#include "Potentiometer.h"

Potentiometer::Potentiometer(uint8_t pin, void (*onChange)(int), const bool inverted, const int activationThreshold, const int maxOutput)
	: _pin(pin), _onChange(onChange), _inverted(inverted), _activationThreshold(activationThreshold), _maxOutput(maxOutput), _lastRaw(0), _mappedValue(0) {
}

bool Potentiometer::update() {
	int currentRaw = analogRead(_pin);

	if (_inverted) {
		currentRaw = 4095 - currentRaw;
	}

	if (abs(currentRaw - _lastRaw) >= _activationThreshold) {
		_lastRaw = currentRaw;
		_mappedValue = map(currentRaw, 0, 4095, 0, _maxOutput);

		if (_onChange != nullptr) {
			_onChange(_mappedValue);
		}
		return true;
	}

	return false;
}

int Potentiometer::getValue() const {
	return _mappedValue;
}