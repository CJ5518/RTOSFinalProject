#include <Arduino.h>
#include <Wire.h>
#include "GUI.hpp"

#include "alarmScheduler.hpp"
#include "lightManager.hpp"
#include "hdcWrapper.hpp"
#include "lcdManager.hpp"

GUI gui;

int status = WL_IDLE_STATUS;

const char* passwordHSPT = "04xjji7sa764i"; // password for Wi-Fi network
const char* ssidHSPT = "iPhone"; // name of Wi-Fi network

void setup() {
	Serial.begin(MONITOR_SPEED);
	gui.init();
				
	pinMode(GPIO_INTERRUPT, INPUT_PULLDOWN);
	pinMode(ALARM_PIN, OUTPUT);

	attachInterrupt(digitalPinToInterrupt(GPIO_INTERRUPT), ISR, FALLING);
	attachInterrupt(digitalPinToInterrupt(GPIO_SNOOZE), ISR_SNOOZE, FALLING);

	xTaskCreate(vLCDTask, "LCD", 2048, NULL, 5, NULL);

	lightActions = xQueueCreate(16, sizeof(int));
	xTaskCreate(task_lightManager, "Light Manager", 2048, NULL, 5, NULL);
	
	Wire.begin();
	//first call to the function initializes the sensor
	double a, b;
	getTempHumid(a, b);

	newAlarms = xQueueCreate(16, sizeof(alarmDefinition));
    listAlarms = xQueueCreate((MAX_ALARMS * 2), sizeof(alarmDefinition));
	schedulerCommands = xQueueCreate(16, sizeof(int));
	xTaskCreate(task_alarmScheduler, "Alarm Scheduler", 4096, NULL, 5, NULL);

	WiFi.setHostname("AVG_ESP32"); // set a host name for the ESP32
	delay(1000);
	WiFi.mode(WIFI_AP_STA); // set mode of WiFi to Station, connect to a pre-existing network
	delay(1000);

	WiFi.begin(ssidHSPT, passwordHSPT); // connect to specified network and enter password
	delay(1000);
	status = WiFi.status(); // get current status of WiFi connection

	while(status != WL_CONNECTED){
		printf("Connecting...\n");
		status = WiFi.begin(ssidHSPT, passwordHSPT);
		delay(1000);
	}

}

void loop() {
	delay(1000);
}

