#pragma once 

#include <Arduino.h>

class SerialHandler {
	public:
		void begin(unsigned long baudRate);
		void listen();
		void send(const String& message);
		bool isConnected() const;
		unsigned long getLastPingTime() const { return _lastPingTime; }
		void ping() { _lastPingTime = millis(); }

	private:
		static constexpr unsigned long TIMEOUT_MS = 10000;
		static constexpr unsigned long SLEEP_MS = 60000;

		unsigned long _baudRate;
		unsigned long _lastPingTime{0};

		void handleMessage(const String& message);
};