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
#include <Arduino.h>
#include "lightManager.hpp"

#define GPIO_INTERRUPT 18 // pin for pushbutton 1 on the Vandaluino Shield
#define ALARM_PIN 15 // pin that outputs to piezobuzzer

#define DEBOUNCE 100

extern QueueHandle_t lightActions;

static char returnString[50];
static char otherReturnString[50];

void getTime();
void IRAM_ATTR ISR();
void vLCDTask(void *pvParameters);

#endif