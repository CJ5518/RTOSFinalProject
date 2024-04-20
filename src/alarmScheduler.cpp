//Spencer Butler, CS 452, Final Project

#include <Arduino.h>
#include <time.h>

#include "alarmScheduler.hpp"
QueueHandle_t newAlarms;

#include "lightManager.hpp"
extern QueueHandle_t lightActions;

//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(time_t t) {
    //TODO: alarm time standardization
    //unpack t to formatted day/hour/minute/etc
    //preserve only time within the day, e.g. hour/minute/seconds
    //repack to seconds since midnight, or other simple daily-time integer
    return 5;
}

//task which coordinates alarm events
void task_alarmScheduler(void *p) {
    alarmDefinition alarms[MAX_ALARMS];
    alarmDefinition tempAlarm;

    time_t rawTime;
    unsigned int processedTime;

    int signal;
    
    //total alarms defined
    int alarmCount = 0;

    //index of the next alarm
    int nextAlarm = 0;

    while(1) {
        //process any changes to the alarm list
        //time out every 10 ticks to check if the current alarm can go off
        while(xQueueReceive(newAlarms, &tempAlarm, 10)) {
            //TODO: alarm list insertion
            //search through the list of alarms, insert the new alarm to maintain list's order
            alarmCount++;
        }
        
        if(alarmCount > 0) {
            //if there are any possible alarms left in the day
            if(nextAlarm < alarmCount) {
                rawTime = time(NULL);
                processedTime = timeToAlarmTime(rawTime);
                tempAlarm = alarms[nextAlarm];
                if(processedTime > tempAlarm.time) {
                    nextAlarm++;
                    if(tempAlarm.actions & ACTION_LIGHT) {
                        signal = LIGHT_SIGNAL_ON;
                        xQueueSendToBack(lightActions, &signal, 0);
                    }
                    if(tempAlarm.actions & ACTION_RING) {
                        //TODO: signal the buzzer to ring
                    }
                }
                //possible edge case failure:
                //  alarm right before midnight, doesn't ring, since time never less than its
                //  also add a daily alarm wraparound check here
                
            } else {
                //TODO: daily alarm wraparound
                //if it's midnight, wrap next alarm back to start of day
                //one possible way:
                //  check alarms[0]
                //  if the current time is less than that, then wrap around
                //  this may fail with an alarm very shortly after midnight
                //  could instead check alarms[next - 1]
                //
                //another:
                //  if the current time is just less than a small constant, wrap around
                //  may also fail with alarm very shortly after midnight, triggering it multiple times
                //  could reduce size of constant
                //  could have a flag to prevent resetting until time has gotten big again
                //
                //edge cases, should ring once only right about the wraparound time:
                //  alarm exactly at time (0 - 1)
                //  alarm exactly at time (0 + 1)
            }
        }

    }
}



