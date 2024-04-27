//Spencer Butler, CS 452, Final Project
#ifndef alarmScheduler_hpp
#define alarmScheduler_hpp 

#include <time.h>

//Potential actions for an alarm to cause -- bitwise OR these to combine
#define ACTION_LIGHT (0b1)
#define ACTION_RING (0b10)

//struct to represent the definition of an alarm
typedef struct {
    unsigned int time;
    unsigned int actions;
} alarmDefinition;


//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(time_t t);
unsigned int timeToAlarmTime(int hours, int minutes);

//task which coordinates alarm events
void task_alarmScheduler(void *p);

//max number of alarms defined at once
#define MAX_ALARMS (16)

#endif
