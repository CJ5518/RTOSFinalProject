//Spencer Butler, CS 452, Final Project
#ifndef lightManager_hpp
#define lightManager_hpp

#define PIXEL_LENGTH (4)
#define PIXEL_PIN (19)
#define PIXEL_MODE (NEO_GRBW + NEO_KHZ800)

//approximate time, in seconds, for the light to go from off to maximum brightness
#define LIGHT_TIMEFRAME (30)

//messages to be sent into the queue
#define LIGHT_SIGNAL_OFF (1)
#define LIGHT_SIGNAL_ON (2)

//Task which controls neoPixel lights in response to a queue
void task_lightManager(void *p);

extern QueueHandle_t lightActions;
#endif

