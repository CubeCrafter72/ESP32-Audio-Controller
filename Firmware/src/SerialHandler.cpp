#include "SerialHandler.h"
#include "Display.h"
#include "Screens.h"
#include "StateInfo.h"

void SerialHandler::begin(unsigned long baudRate) {
	Serial.begin(baudRate);
}

void SerialHandler::listen() {
    if (Serial.available()) {
		String message = Serial.readStringUntil('\n');
		message.trim();
		handleMessage(message);
	}
}

bool SerialHandler::isConnected() const {
    return _lastPingTime != 0 && (millis() - _lastPingTime) <= TIMEOUT_MS;
}

void SerialHandler::send(const String& message) {
    Serial.println(message);
}

void SerialHandler::handleMessage(const String& message) {
    if (message.equals("ping")) {
        ping();
        return;
    } else if (message.equals("track-stop")) {
        display.setBaseScreen(Display::Screen{});
        return;
    } else if (message.equals("reboot")) {
        ESP.restart();
        return;
    }

    char buffer[128];
    message.toCharArray(buffer, 128);

    char *token = strtok(buffer, "|");

    if (token == nullptr)
        return;

    if (strcmp(token, "track") != 0)
        return;

    // Title
    if ((token = strtok(nullptr, "|")) == nullptr)
        return;
    
    strncpy(stateInfo.songTitle, token, 63);
    stateInfo.songTitle[63] = '\0';

    // Artist
    if ((token = strtok(nullptr, "|")) == nullptr)
        return;
    
    strncpy(stateInfo.songArtist, token, 63);
    stateInfo.songArtist[63] = '\0';

    // Playback status
    if ((token = strtok(nullptr, "|")) == nullptr)
        return;
    
    stateInfo.isPlaying = strcmp(token, "True") == 0;

    // Current seconds
    if ((token = strtok(nullptr, "|")) == nullptr)
        return;
    
    stateInfo.currentSeconds = atoi(token);

    // Total seconds
    if ((token = strtok(nullptr, "|")) == nullptr)
        return;
    
    stateInfo.totalSeconds = atoi(token);

    // Update track percentage
    if (stateInfo.totalSeconds == 0) {
        stateInfo.trackPercentage = -1;
    } else {
        stateInfo.trackPercentage = (stateInfo.currentSeconds * 100) / stateInfo.totalSeconds;
    }

    display.setBaseScreen(Screens::TRACK_INFO);
}