#include <Arduino.h>
#include "Display.h"
#include "Screens.h"
#include "Potentiometer.h"
#include "RGBLed.h"
#include "StateInfo.h"
#include "SerialHandler.h"
#include "Button.h"
#include "MediaControls.h"

void setConnected(bool state);

const int UPDATE_FREQ_HZ = 25;
const int POT_COUNT = 4;
const int BUTTON_COUNT = 3;

Potentiometer pots[POT_COUNT] = {
    Potentiometer(26, onMasterVolumeChange), // L fader
    Potentiometer(27, onMicVolumeChange, true), // R fader
    Potentiometer(33, onLeftKnobChange), // L knob
    Potentiometer(25, onRightKnobChange) // R knob
};

Button buttons[BUTTON_COUNT] = {
    Button(5, togglePlayPause, reboot),
    Button(18, previousTrack),
    Button(19, nextTrack)
};

RGBLed led(4, 16, 17);
Display display(22, 21);
SerialHandler serial;
StateInfo stateInfo;

bool connected = true;

void setup() {
    serial.begin(115200);

    display.begin();
    display.setScreen(Screens::LOGO);
    display.update();

    led.setColor(3, 140, 252);
    led.setMode(RGBLed::Mode::SOLID);

    initMediaControls(display, serial, led);

    delay(2000);
}

void loop() {
    serial.listen();
    setConnected(serial.isConnected());

    if ((millis() - serial.getLastPingTime()) > 60000) {
        led.setMode(RGBLed::Mode::OFF);

        display.setBaseScreen(Display::Screen{});
        display.update();

        Serial.flush();

        esp_sleep_enable_uart_wakeup(0);
        esp_light_sleep_start();
        // After waking up
        serial.ping();
    }
    
    if (connected) {
        for (int i = 0; i < POT_COUNT; i++) {
            pots[i].update();
        }
    }

    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].update();
    }

    led.update();
    display.update();

    delay(1000 / UPDATE_FREQ_HZ);
}

void setConnected(bool state) {
    if (connected == state) {
        return;
    }

    connected = state;

    if (connected) {
        led.setMode(RGBLed::Mode::OFF);

        display.setBaseScreen(Display::Screen{});
        display.setScreen(Screens::CONNECTED);
    } else {
        led.setMode(RGBLed::Mode::BLINK);
        led.setBlinkInterval(500);
        led.setColor(128, 0, 0);

        display.setBaseScreen(Screens::DISCONNECTED);
    }
}