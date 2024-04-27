/*********************************************
 * Id: math1851 (Liam Mathews)
 * Class: CS 452 (Real Time Operating Systems)
 * Date: 4/19/2024
 * Objective: Integrate LCD, Piezobuzzer, and 
 *            an interrupt handler for button 
 *            presses on the Vandaluino Shield.
 * 
 * TODO: Implement switch debouncing, send external
 *       queue for lighting to lightManager.cpp. 
 *********************************************/
#include "lcdManager.hpp"
#include "FreeRTOSConfig.h"
#include "freertos/task.h"

// interrupt is triggered by S1 pushbutton on the
// Vandaluino Shield, toggles piezobuzzer on/off
void IRAM_ATTR ISR(){
  // debounce logic
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  button_Pressed = !button_Pressed;
  if(button_Pressed == false){
    xQueueSendFromISR(lightActions, &sendLights, &xHigherPriorityTaskHasWoken);
    tone(GPIO_INTERRUPT, 0);
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
    returnString = (char *)"Could not get time";
  }
  Serial.print(&ntptime, "%A, %B %d %Y %H:%M:%S");
  Serial.print("\n");

  strftime(returnString, sizeof(returnString), "%A, %B %d", &ntptime);
  strftime(otherReturnString, sizeof(returnString), "%H:%M:%S", &ntptime);

  lcd.begin(16, 2); // initialize LCD 
  lcd.clear(); // clear LCD screen
  lcd.print(returnString); // print first line to LCD
  lcd.setCursor(2,1); // set cursor to second line
  lcd.print(otherReturnString); // print second line to LCD

}

/* Add to main.cpp
void setup(){
  pinMode(GPIO_INTERRUPT, INPUT_PULLDOWN);
  pinMode(ALARM_PIN, OUTPUT);

  attachInterrupt(GPIO_INTERRUPT, ISR, FALLING); // configure isr to run when button released

  xTaskCreate(vLCDTask, "Test", 2048, NULL, 2, NULL); // create vLCDTask

}
*/