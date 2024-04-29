//Implements GUI.hpp
#include "GUI.hpp"

#include "WiFi.h"
#include "html.hpp"

#include "hdcWrapper.hpp"
#include "alarmScheduler.hpp"
#include <time.h>

//Adapted from:

//https://web.archive.org/web/20240213064921/https://gist.githubusercontent.com/evert-arias/
//d0abf2769802e56c88793a4447fe9f7e/raw/b1f582f8cdcaa07f84072ede8687bdbe9045e75a/esp32-wifi-auto-connect.cpp

//Credentials for the Access point
const char* ssid     = "Alarm ESP32";
const char* password = "password1234";

IPAddress apIP = IPAddress(192,168,4,42);

AsyncWebServer* GUI::server = 0;

void GUI::onWifiEvent(arduino_event_id_t event, arduino_event_info_t info) {
	Serial.printf("Got event %d - %s\n", event, WiFi.eventName(event));
	switch (event) {
		case 10: {
			Serial.printf("AP Started\n");
			WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

			delay(800);

			WiFi.softAP(ssid, password);
			if (!server) {
				server = new AsyncWebServer(80);

server->on("/alarm.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", alarm_css);
});
server->on("/alarm.html", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", alarm_html);
});
server->on("/common.js", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/javascript", common_js);
});
server->on("/index.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", index_css);
});
server->on("/index.html", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
});
server->on("/style.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", style_css);
});
server->on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->redirect("/index.html");
});


				//Alarm page functions
				server->on("/deleteAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {

				});
				server->on("/createAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {

				});
				server->on("/getAlarms", HTTP_POST, [](AsyncWebServerRequest *request) {

				});


				//Return info for the main screen
				server->on("/getTmp", HTTP_POST, [](AsyncWebServerRequest *request) {
					double temp, humid;
					getTempHumid(temp, humid);
					request->send(200, "text/plain", String(temp));
				});
				server->on("/getHumid", HTTP_POST, [](AsyncWebServerRequest *request) {
					double temp, humid;
					getTempHumid(temp, humid);
					request->send(200, "text/plain", String(humid));
				});
				server->on("/getTime", HTTP_POST, [](AsyncWebServerRequest *request) {
					time_t tim = time(0);
					int hours, minutes;
					timeFromAlarmTime(hours, minutes, timeToAlarmTime(tim));
					String ret = "";
					ret.concat(hours);
					ret.concat(":");
					ret.concat(minutes);
					request->send(200, "text/plain", ret);
				});

				//Main screen buttons
				server->on("/stopAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					offAlarm();
					request->send(200);
				});
				server->on("/snoozeAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					snoozeAlarm();
					request->send(200);
				});

				server->begin();
				Serial.printf("Started server\n");
			}
		} break; 
	}
}

// Wi-Fi connect task
 void GUI::wifi_connect_cb(void*) {
	// Connect to Wi-Fi network
	vTaskDelay(5000);
	bool ret = WiFi.enableAP(true);
	ret = WiFi.softAP(ssid, password);
	Serial.println("After ret=");

	//Silly little hack because tasks can't just end apparently, and I don't want to bother doing it right
	for (;;) {
		vTaskDelay(50000);
	}
}

//Wi-Fi watchdog task
//Will be used in the future to make sure we're connected, but not useful right now
void GUI::wifi_watchdog_cb(void*) {
}

void GUI::init() {
	Serial.printf("In GUI init\n");
	
	xTaskCreate(wifi_connect_cb, "Connection", 8000, NULL, 8, NULL);
	
	// Wi-Fi events listener
	WiFi.onEvent(onWifiEvent);
}

void GUI::end() {
	delete server;
}
