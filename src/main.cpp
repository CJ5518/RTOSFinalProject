#include <Arduino.h>
#include "GUI.hpp"

GUI gui;



void setup() {
	Serial.begin(MONITOR_SPEED);
	gui.init();
}

void loop() {
	delay(1000);
}

