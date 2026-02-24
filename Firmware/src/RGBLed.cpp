#include "RGBLed.h"

RGBLed::RGBLed(uint8_t pinR, uint8_t pinG, uint8_t pinB)
	: _pinR(pinR), _pinG(pinG), _pinB(pinB) {

	ledcAttach(_pinR, PWM_FREQ, PWM_RESOLUTION);
	ledcAttach(_pinG, PWM_FREQ, PWM_RESOLUTION);
	ledcAttach(_pinB, PWM_FREQ, PWM_RESOLUTION);
}

void RGBLed::setColor(int r, int g, int b) {
	_r = r;
	_g = g;
	_b = b;

	if (_currentMode == Mode::SOLID) {
		writeColor(_r, _g, _b);
	}
}

void RGBLed::setMode(Mode mode, unsigned long duration) {
	_currentMode = mode;
	_modeExpireTime = duration > 0 ? millis() + duration : 0;

	if (_currentMode == Mode::OFF) {
		writeColor(0, 0, 0);
	} else if (_currentMode == Mode::SOLID) {
		writeColor(_r, _g, _b);
	}
}

void RGBLed::setBreathPeriod(int period) {
	_breathPeriod = period;
}

void RGBLed::setBlinkInterval(int interval) {
	_blinkInterval = interval;
}

void RGBLed::update() {
	if (_modeExpireTime != 0 && millis() >= _modeExpireTime) {
		setMode(Mode::OFF);
		return;
	}

	switch (_currentMode) {
		case Mode::OFF:
		case Mode::SOLID:
			return;
		case Mode::BLINK: {
			unsigned long currentTime = millis();

			if (currentTime - _lastBlinkTime >= _blinkInterval) {
				_blinkState = !_blinkState;
				_lastBlinkTime = currentTime;

				if (_blinkState) {
					writeColor(_r, _g, _b);
				} else {
					writeColor(0, 0, 0);
				}
			}
			break;
		}
		case Mode::BREATH:
			float intensity = calculateBreathIntensity(millis());
			
			writeColor(
				static_cast<int>(_r * intensity),
				static_cast<int>(_g * intensity),
				static_cast<int>(_b * intensity)
			);
			break;
	}
}

void RGBLed::writeColor(int r, int g, int b) {
	ledcWrite(_pinR, r);
	ledcWrite(_pinG, g);
	ledcWrite(_pinB, b);
}

/*
 * Returns a value in range [0, 1]
 */
float RGBLed::calculateBreathIntensity(unsigned long time) {
	float phase = (time % _breathPeriod) * 2.0f * PI / _breathPeriod;
	return (exp(sin(phase)) - 0.36787944) * 0.42545906;
}