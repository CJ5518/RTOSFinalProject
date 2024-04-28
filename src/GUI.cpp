//Implements GUI.hpp
#include "GUI.hpp"

#include "WiFi.h"
#include "html.hpp"

//Adapted from:

//https://web.archive.org/web/20240213064921/https://gist.githubusercontent.com/evert-arias/
//d0abf2769802e56c88793a4447fe9f7e/raw/b1f582f8cdcaa07f84072ede8687bdbe9045e75a/esp32-wifi-auto-connect.cpp

//Credentials for the Access point
//In the future we'd like to be able to save/change/load these
//But for now the const variables work fine

//I work on the Shear Stress Bioreactor capstone project
const char* ssid     = "ESP32 Bioreactor";
const char* password = "password1234";

IPAddress apIP = IPAddress(192,168,4,42);

AsyncWebServer* GUI::server = 0;
// Wi-Fi events
//Default wifi event definitions are WRONG because everybody hates me I guess

/*
[  5140][V][WiFiGeneric.cpp:392] _arduino_event_cb(): AP Started
[  5141][D][WiFiGeneric.cpp:1035] _eventCallback(): Arduino Event: 10 - AP_START


On user connect (2 events)
[ 27801][V][WiFiGeneric.cpp:407] _arduino_event_cb(): AP Station Connected: MAC: , AID: 1
[ 27802][D][WiFiGeneric.cpp:1035] _eventCallback(): Arduino Event: 12 - AP_STACONNECTED

[ 28327][V][WiFiGeneric.cpp:421] _arduino_event_cb(): AP Station IP Assigned:192.168.4.2
[ 28328][D][WiFiGeneric.cpp:1035] _eventCallback(): Arduino Event: 14 - AP_STAIPASSIGNED

On user disconnect (1 event)
[ 33674][V][WiFiGeneric.cpp:414] _arduino_event_cb(): AP Station Disconnected: MAC: , AID: 1
[ 33675][D][WiFiGeneric.cpp:1035] _eventCallback(): Arduino Event: 13 - AP_STADISCONNECTED
*/




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
server->on("/index.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", index_css);
});
server->on("/index.html", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
});
server->on("/package.json", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/json", package_json);
});
server->on("/style.css", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", style_css);
});
server->on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        request->redirect("/index.html");
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
