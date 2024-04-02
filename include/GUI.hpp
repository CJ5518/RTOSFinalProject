#pragma once

#include "ESPAsyncWebServer.h"


//Class encapsulating the GUI of the program
//Contains code and data for setting up the web server and interacting with the routine manager
//from the html interface
class GUI {
public:
    //Inits the server
    static void init();
    //Closes all server connections and deletes the server,
    //recommend to call this to make sure the connections close cleanly
    static void end();

    //Only called by GUI.cpp
    static void initWebServer();


//private:
    //Pointer to internal web server
    static AsyncWebServer* server;
    //Task callbacks
    static void wifi_connect_cb(void*);
    static void wifi_watchdog_cb(void*);

    static void onWifiEvent(arduino_event_id_t event, arduino_event_info_t info);
};
