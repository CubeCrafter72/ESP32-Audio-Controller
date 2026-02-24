#pragma once

#include <Arduino.h>

class RGBLed {
	public:
		enum class Mode {
			OFF,
			SOLID,
			BLINK,
			BREATH
		};

		RGBLed(uint8_t pinR, uint8_t pinG, uint8_t pinB);

		void setColor(int r, int g, int b);
		void setMode(Mode mode, unsigned long duration = 0);
		void setBreathPeriod(int period);
		void setBlinkInterval(int interval);
		void update();

	private:
		static constexpr int PWM_FREQ = 5000;
		static constexpr int PWM_RESOLUTION = 8; // [0-255]

		uint8_t _pinR, _pinG, _pinB;
		
		Mode _currentMode{Mode::OFF};
		unsigned long _modeExpireTime{0};
		int _r{0}, _g{0}, _b{0};
		
		int _breathPeriod{2500};
		int _blinkInterval{500};
		unsigned long _lastBlinkTime{0};
		bool _blinkState{false};

		void writeColor(int r, int g, int b);
		float calculateBreathIntensity(unsigned long time);
};