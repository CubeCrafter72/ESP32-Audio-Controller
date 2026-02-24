#pragma once

#include <Arduino.h>

class Potentiometer {
    private:
        uint8_t _pin;
        void (*_onChange)(int);
        bool _inverted;
        int _activationThreshold;
        int _maxOutput;
        int _lastRaw;
        int _mappedValue;

    public:
        Potentiometer(uint8_t pin, void (*onChange)(int), bool inverted = false, int activationThreshold = 40, int maxOutput = 100);

        bool update();
        int getValue() const;
};