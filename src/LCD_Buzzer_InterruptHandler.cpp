/*********************************************
 * Id: math1851 (Liam Mathews)
 * Class: CS 452 (Real Time Operating Systems)
 * Date: 4/19/2024
 * Objective: Integrate LCD, Piezobuzzer, and 
 *            an interrupt handler for button 
 *            presses on the Vandaluino Shield.
 * 
 * TODO: Implement switch debouncing, integrate
 *       functionality with other components/files. 
 *********************************************/
#include <Arduino.h>
#include "FreeRTOSConfig.h"
#include "freertos/task.h"
#include <LiquidCrystal.h>
#include <WiFi.h>
#include "string.h"
#include "time.h"

#define GPIO 18 // pin for pushbutton 1 on the Vandaluino Shield
#define TONE 15 // pin that outputs to piezobuzzer

volatile bool button_Pressed = false; // used as a flag for switch debouncing
volatile bool piezo_Beep = false; // used in isr to toggle piezobuzzer on/off 

const char* password = "********"; // password for WiFi network
const char* ssid = "****"; // name of WiFi network

int status = WL_IDLE_STATUS; // used to check status of WiFi connection

const long gmtOffset_sec = -28800; // get time for PDT time zone 
const int daylightOffset_sec = 3600; // account for daylight savings
const char* ntpServer = "pool.ntp.org"; // target server to get time from internet

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
// RS, E, D4, D5, D6, D7

char *getTime(); // funtion called from vLCDTask retrieves time from server. 

// interrupt is triggered by S1 pushbutton on the
// Vandaluino Shield, toggles piezobuzzer on/off
void IRAM_ATTR ISR(){


  button_Pressed = !button_Pressed;
  if(button_Pressed == false){
    tone(TONE, 1770);
  }else{
    tone(TONE, 0);
  }
}

// connects to WiFi and continues to retrieve
// current datetime from target server
void vLCDTask(void *pvParameters){
  // connect to WiFi 
  WiFi.disconnect(true); // disconnect from any previous WiFi networks
  delay(1000);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE); // clear IP configuration settings
  delay(1000);
  WiFi.setHostname("HotspotESP32"); // set a host name for the ESP32
  delay(1000);
  WiFi.mode(WIFI_STA); // set mode of WiFi to Station, connect to a pre-existing network
  delay(1000);

  WiFi.begin(ssid, password); // connect to specified network and enter password
  delay(1000);
  status = WiFi.status(); // get current status of WiFi connection

  while(status != WL_CONNECTED){
    printf("Connecting...\n");
    status = WiFi.begin(ssid, password);
    delay(1000);
  }

  // connect to NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  for(;;){
    // query IOT source for commands
    // display current time
    printf("In the for loop\n");
    printf("%d\n", WiFi.status());
    //retrieveDate = strdup(getTime());
    printf("Printing getTime: %s\n", getTime());
    vTaskDelay(1000);
  }
}

char *getTime(){
  static char returnString[50];
  static char otherReturnString[50];

  // initialize LCD
  lcd.begin(16, 2);
  lcd.clear();

  Serial.print("Entered getTime Function\n");
  struct tm ntptime;

  if(!getLocalTime(&ntptime)){
    Serial.print("Could not get time\n");
    return (char *)"Could not get time\n";
  }
  Serial.print(&ntptime, "%A, %B %d %Y %H:%M:%S");
  Serial.print("\n");

  strftime(returnString, sizeof(returnString), "%A, %B %d", &ntptime);
  strftime(otherReturnString, sizeof(returnString), "%H:%M:%S", &ntptime);

  lcd.clear();
  //lcd.setCursor(1,1);
  lcd.print(returnString);
  lcd.setCursor(2,1);
  lcd.print(otherReturnString);
  //lcd.display();

  return (char *)returnString;
}

void setup() {
  // configure output pins
  pinMode(GPIO, INPUT_PULLDOWN);
  pinMode(TONE, OUTPUT);

  Serial.begin(MONITOR_SPEED); // initialize Serial port 

  attachInterrupt(GPIO, ISR, FALLING); // configure isr to run when button released

  xTaskCreate(vLCDTask, "Test", 2048, NULL, 2, NULL); // create vLCDTask
}

void loop() {}

/*
RECYCLE BIN
*/