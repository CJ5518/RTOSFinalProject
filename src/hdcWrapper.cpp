//Spencer Butler, CS 452, Final Project
#include <Arduino.h>
#include <Wire.h>
#include "hdcWrapper.hpp"

//Stores temperature in t and humidity in h
//Temperature is degrees Celsius
//Humidity is between 0 and 1
//First function call initializes the sensor rather than returning values
void getTempHumid(double &t, double &h) {
  static SemaphoreHandle_t sensorBusy = NULL;
  static char dataBytes[4];
  static uint16_t dataWords[2];
  static unsigned int delayPeriod;

  //first call initializes the sensor rather than taking a measurement
  if(sensorBusy == NULL) {
    sensorBusy = xSemaphoreCreateMutex();
    printf("Initializing HDC1080 sensor.\n");

    //configuring to read both temp and humidity, in 14 bit resolution
    dataWords[0] = 0x1000;
    Wire.beginTransmission(HDC1080ADDRESS);
    //config register at 0x02
    Wire.print(0x02);
    Wire.print(dataWords[0]);
    Wire.endTransmission();

    //ensures the sensor has at least 20 milliseconds to perform the measurement
    delayPeriod = (20 / portTICK_PERIOD_MS);
    if(!delayPeriod) { delayPeriod = 1; }

    xSemaphoreGive(sensorBusy);
    return;
  }

  if(xSemaphoreTake(sensorBusy, portMAX_DELAY)) {
    Wire.beginTransmission(HDC1080ADDRESS);
    //reading from register 0x00
    Wire.write(0x00);
    Wire.endTransmission();

    vTaskDelay(delayPeriod);
    Wire.requestFrom(HDC1080ADDRESS, 4);
    Wire.readBytes(dataBytes, 4);
    dataWords[0] = dataBytes[0] << 8 | dataBytes[1];
    dataWords[1] = dataBytes[2] << 8 | dataBytes[3];
    
    //datasheet: divide by 2^16, multiply by 165, subtract 40
    t = ((double) dataWords[0] / (0xffff + 1.0)) * 165 - 40;

    //datasheet: divide by 2^16
    h = ((double) dataWords[1] / (0xffff));

    xSemaphoreGive(sensorBusy);
  }

}

//Converts degrees Celsius to Fahrenheit
double degCtoF(double degC) { return ((degC * 9.0 / 5.0) + 32); }
