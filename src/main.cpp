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

const char* passwordAP = "password"; // password for Wi-Fi network
const char* ssidAP = "ESP32 Alarm"; // name of Wi-Fi network

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


	WiFi.mode(WIFI_AP_STA);

	Serial.println("\n[*] Creating ESP32 AP");
	WiFi.softAP(ssidAP, passwordAP);
	Serial.print("[+] AP Created with IP Gateway ");
	Serial.println(WiFi.softAPIP());

	WiFi.begin(ssidHSPT, passwordHSPT);
	Serial.println("\n[*] Connecting to WiFi Network");

	while(WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(100);
	}

	Serial.print("\n[+] Connected to the WiFi network with local IP : ");
	Serial.println(WiFi.localIP());
}

void loop() {
	delay(1000);
}

