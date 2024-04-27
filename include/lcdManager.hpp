/*********************************************
 * Id: math1851 (Liam Mathews)
 * Class: CS 452 (Real Time Operating Systems)
 * Date: 4/19/2024
 * 
 * Declarations of variables and functions used
 * to control LCD, and handle pushbutton response
 * to switch off alarm behavior.  
 *********************************************/
#ifndef LCD_MANAGER_HPP
#define LCD_MANAGER_HPP

#include <LiquidCrystal.h>
#include "lightManager.hpp"

#define GPIO_INTERRUPT 18 // pin for pushbutton 1 on the Vandaluino Shield
#define ALARM_PIN 15 // pin that outputs to piezobuzzer

extern QueueHandle_t lightActions;

int sendLights = LIGHT_SIGNAL_OFF;

static char returnString[50];
static char otherReturnString[50];

struct tm ntptime;

volatile bool button_Pressed = false; // used as a flag for switch debouncing
volatile bool piezo_Beep = false; // used in isr to toggle piezobuzzer on/off 

const long gmtOffset_sec = -28800; // get time for PDT time zone 
const int daylightOffset_sec = 3600; // account for daylight savings
const char* ntpServer = "pool.ntp.org"; // target server to get time from internet

               // RS, E,  D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

void getTime();
void IRAM_ATTR ISR();
void vLCDTask(void *pvParameters);

#endif