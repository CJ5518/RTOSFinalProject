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
std::vector<unsigned int> GUI::alarms;

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
server->on("/alarm.js", HTTP_ANY, [](AsyncWebServerRequest *request) {  
    request->send_P(200, "text/javascript", alarm_js);
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
server->on("/index.js", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/javascript", index_js);
});
server->on("/style.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", style_css);
});
server->on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->redirect("/index.html");
});


				//Alarm page functions
				server->on("/deleteAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					int hour = request->getParam("hour", true)->value().toInt();
					int minute = request->getParam("minute", true)->value().toInt();
					unsigned int check = timeToAlarmTime(hour, minute);
					Serial.printf("Trying to delete: %d\n", check);
					for (int q = 0; q < alarms.size(); q++) {

						if (alarms[q] == check) {
							alarms.erase(alarms.begin() + q);
							changeAlarm(check, 0);
							break;
						}
					}
					request->send(200);
				});
				server->on("/createAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					int hour = request->getParam("hour", true)->value().toInt();
					int minute = request->getParam("minute", true)->value().toInt();


					unsigned int check = timeToAlarmTime(hour, minute);
					Serial.printf("Checking if exists: %d\n", check);
					for (int q = 0; q < alarms.size(); q++) {

						if (alarms[q] == check) {
							request->send(200);
							return;
						}
					}

					unsigned int actions = 0;
					if (request->getParam("once", true)) {
						actions |= ACTION_ONCE;
					}
					if (request->getParam("light", true)) {
						actions |= ACTION_LIGHT;
					}
					if (request->getParam("ring", true)) {
						actions |= ACTION_RING;
					}
					Serial.printf("Create: changeAlarm(timeToAlarmTime(%d, %d),  %d)\n", hour, minute, actions);
					changeAlarm(timeToAlarmTime(hour, minute),  actions);
					alarms.push_back(timeToAlarmTime(hour, minute));
					request->send(200);
				});
				server->on("/getAlarms", HTTP_POST, [](AsyncWebServerRequest *request) {
					String ret = "";
					int hours, minutes;
					for (int q = 0; q < alarms.size(); q++) {
						if (q > 0) {
							ret.concat(",");
						}
						timeFromAlarmTime(hours, minutes, alarms[q]);
						ret.concat(hours);
						ret.concat(":");
						if (minutes < 10) {
							ret.concat("0");
						}
						ret.concat(minutes);
					}
					request->send(200, "text/plain", ret);
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
					if (minutes < 10) {
						ret.concat("0");
					}
					ret.concat(minutes);
					request->send(200, "text/plain", ret);
				});

				//Main screen buttons
				server->on("/stopAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					offAlarm();
					Serial.printf("Stoppies!\n");
					request->send(200);
				});
				server->on("/snoozeAlarm", HTTP_POST, [](AsyncWebServerRequest *request) {
					snoozeAlarm();
					Serial.printf("Snoozers!\n");
					request->send(200);
				});

				server->begin();
				Serial.printf("Started server\n");
			}
		} break; 
	}
}


void GUI::init() {
	Serial.printf("In GUI init\n");
	alarms.clear();
	// Wi-Fi events listener
	WiFi.onEvent(onWifiEvent);
}

void GUI::end() {
	delete server;
}
