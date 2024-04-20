//Spencer Butler, CS 452, Final Project
//Defines a task which turns lights on and off in response to messages in a queue
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "lightManager.hpp"
QueueHandle_t lightActions;

//Task which controls neoPixel lights in response to a queue
void task_lightManager(void *p) {
  int i, brightness, delta, delayTime, signal;
  Adafruit_NeoPixel pixelStrip = Adafruit_NeoPixel(PIXEL_LENGTH, PIXEL_PIN, PIXEL_MODE);
  uint32_t displayColor = pixelStrip.Color(255, 255, 255);
  //total time, into milliseconds, divide by 255 steps, divide by length of each tick
  delayTime = (LIGHT_TIMEFRAME * 1000.0 / 255.0 / portTICK_PERIOD_MS);

  pixelStrip.begin();
  for(i = 0; i < PIXEL_LENGTH; i++) {
    pixelStrip.setPixelColor(i, displayColor);
  }
  brightness = 0;
  pixelStrip.setBrightness(brightness);
  pixelStrip.show();

  while(1) {
    //empty the queue each time around, perform the latest action
    while(xQueueReceive(lightActions, &signal, 0)) {
      if(signal == LIGHT_SIGNAL_OFF) {
        delta = -1;
        printf("Light received off-signal.\n");
      } else if(signal == LIGHT_SIGNAL_ON) {
        delta = 1;
        printf("Light received on-signal.\n");
      }
    }

    if(delta) {
      brightness += delta;
      if(brightness < 0) {
        brightness = 0;
        delta = 0;
        printf("Light at minimum brightness. Stopping.\n");
      } else if(brightness > 255) {
        brightness = 255;
        delta = 0;
        printf("Light at maximum brightness. Stopping.\n");
      } else {
        //setBrightness is lossy -- refresh the pixel colors
        for(i = 0; i < PIXEL_LENGTH; i++) {
          pixelStrip.setPixelColor(i, displayColor);
        }
        pixelStrip.setBrightness(brightness);
        pixelStrip.show();
      }
    }

    vTaskDelay(delayTime);
  }
}

