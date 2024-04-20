#include <Arduino.h>
#include <Wire.h>
#include "GUI.hpp"

#include "alarmScheduler.hpp"
#include "lightManager.hpp"
#include "hdcWrapper.hpp"

GUI gui;

//queue for sending on/off actions to the light
//declared in lightManager.cpp
extern QueueHandle_t lightActions;

//queue for sending new alarm (time, actions) to the alarm scheduler
//declared in alarmScheduler.cpp
extern QueueHandle_t newAlarms;

void setup() {
	Serial.begin(MONITOR_SPEED);
	gui.init();
        
        lightActions = xQueueCreate(16, sizeof(int));
        xTaskCreate(task_lightManager, "Light Manager", 2048, NULL, 5, NULL);
        
        Wire.begin();
        //first call to the function initializes the sensor
        double a, b;
        getTempHumid(a, b);

        newAlarms = xQueueCreate(16, sizeof(alarmDefinition));
        xTaskCreate(task_alarmScheduler, "Alarm Scheduler", 4096, NULL, 5, NULL);
}

void loop() {
	delay(1000);
}

