#pragma once

#include <Arduino.h>

typedef struct {
	int masterVolume{-1};
	int micVolume{-1};
	int leftKnob{-1};
	int rightKnob{-1};

	bool isPlaying{false};

	char songTitle[64]{};
	char songArtist[64]{};

	float titleScroll{-30.0};
	float artistScroll{-30.0};

	int currentSeconds{0};
	int totalSeconds{0};
	int trackPercentage{-1};
} StateInfo;

extern StateInfo stateInfo;