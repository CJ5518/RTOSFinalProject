/*********************************************
 * Id: math1851 (Liam Mathews)
 * Class: CS 452 (Real Time Operating Systems)
 * Date: 4/19/2024
 * 
 * Integrate LCD, Piezobuzzer, and 
 * an interrupt handler for button 
 * presses on the Vandaluino Shield.
 *********************************************/
#include "lcdManager.hpp"
#include "alarmScheduler.hpp"
#include <Arduino.h>

               // RS, E,  D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
// declaration of LCD object and specified pinouts

struct tm ntptime; // tm object used to get time

int sendLights = LIGHT_SIGNAL_OFF; // signal to turn off NeoPixels declared in lightManager.hpp

const long gmtOffset_sec = -28800; // get time for PDT time zone 
const int daylightOffset_sec = 3600; // account for daylight savings
const char* ntpServer = "pool.ntp.org"; // target server to get time from internet

unsigned long timeSincePressed = -1; // used with delay to debounce switch presses

// interrupt is triggered by S1 pushbutton on the Vandaluino Shield
void IRAM_ATTR ISR(){
  if((millis() - timeSincePressed) >= DEBOUNCE){
    //BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    offAlarm();
    // xQueueSendFromISR(lightActions, &sendLights, &xHigherPriorityTaskWoken); // send light shutdown signal
    // tone(ALARM_PIN, 0); // turn off piezobuzzer
    timeSincePressed = millis(); // reset debounce time
  }
}

void IRAM_ATTR ISR_SNOOZE(){
  if((millis() - timeSincePressed) >= DEBOUNCE){
    snoozeAlarm();
    timeSincePressed = millis();
  }
}

// connects to ntp server and calls  
// getTime function
void vLCDTask(void *pvParameters){
  // connect to NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  for(;;){
    printf("LCDTask\n");
    getTime();
    vTaskDelay(1000);
  }
}

// call getLocalTime, print time to LCD if successful
// otherwise print error message
void getTime(){

  if(!getLocalTime(&ntptime)){
    Serial.print("Could not get time\n");
  }
  Serial.print(&ntptime, "%A, %B %d %Y %H:%M:%S");
  Serial.print("\n");

  strftime(returnString, sizeof(returnString), "%B %d, %Y", &ntptime); // send time to returnString var
  strftime(otherReturnString, sizeof(returnString), "%H:%M:%S", &ntptime); // second string for better spacing

  lcd.begin(16, 2); // initialize LCD 
  lcd.clear(); // clear LCD screen
  lcd.print(returnString); // print first line to LCD
  lcd.setCursor(2,1); // set cursor to second line
  lcd.print(otherReturnString); // print second line to LCD

}
