//Spencer Butler, CS 452, Final Project
#ifndef alarmScheduler_hpp
#define alarmScheduler_hpp 

#include <time.h>

//Potential actions for an alarm to cause -- bitwise OR these to combine
//Alarm will turn the light on
#define ACTION_LIGHT (0b1)
//Alarm will ring the buzzer
#define ACTION_RING (0b10)
//Alarm will happen only once, rather than every day at that time
#define ACTION_ONCE (0b100)

//Immediate commands for the scheduler to take -- these are exclusive
//Stop all alarm actions
#define COMMAND_OFF (1)
//Stop all alarm actions; resume in SNOOZE_LENGTH minutes
#define COMMAND_SNOOZE (2)
//Send all alarms in internal list to listAlarms
#define COMMAND_LIST (3)

//queue which holds requested changes to the scheduler's list of alarms
extern QueueHandle_t newAlarms;
//queue into which the scheduler will, upon request, dump its list of alarms
extern QueueHandle_t listAlarms;
//queue to hold immediate commands for the scheduler to take
extern QueueHandle_t schedulerCommands;


//struct to represent the definition of an alarm
typedef struct {
    unsigned int time;
    unsigned int actions;
} alarmDefinition;


//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(time_t t);
unsigned int timeToAlarmTime(int hours, int minutes);

//converts from alarm time back to hours and minutes
void timeFromAlarmTime(int &hours, int &minutes, unsigned int alarmTime);

//task which coordinates alarm events
void task_alarmScheduler(void *p);

//wrapper which sends a queue message to turn off the alarm
void offAlarm();

//wrapper which sends a queue message to snooze the alarm
void snoozeAlarm();

//wrapper which sends an alarm-change request to newAlarms
void changeAlarm(unsigned int time, unsigned int actions);

//wrapper which sends an alarm-change request to newAlarms
void changeAlarm(int hours, int minutes, unsigned int actions);

//max number of alarms defined at once
#define MAX_ALARMS (32)

//length of snoozing, in minutes
#define SNOOZE_LENGTH (10)

#endif
