//Spencer Butler, CS 452, Final Project

#include <Arduino.h>
#include <time.h>

#include "alarmScheduler.hpp"
QueueHandle_t newAlarms;

#include "lightManager.hpp"
extern QueueHandle_t lightActions;

//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(time_t t) {
    int ret;
    tm *now = localtime(&t);

    return timeToAlarmTime(now->tm_hour, now->tm_min) + now->tm_sec;
}

//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(int hours, int minutes) {
    return (hours * 60 * 60) + (minutes * 60);
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

    //flag for supporting wraparound from end of list to start of list
    //should only wrap around once per day, so this is set when the time is late
    //and cleared when it wraps around
    int canWraparound = 0;

    int i, j;

    while(1) {
        //process any changes to the alarm list
        //time out every 10 ticks to check if the current alarm can go off
        while(xQueueReceive(newAlarms, &tempAlarm, 10)) {
            //find the alarm at this time, if one exists
            while(i < alarmCount && alarms[i].time < tempAlarm.time) {
                i++;
            }
            if(i < alarmCount && alarms[i].time == tempAlarm.time) {
                //there already exists an alarm at this time, update its actions
                //support deletion via actions = 0
                if(tempAlarm.actions == 0) {
                    while(i < (alarmCount - 1)) {
                        alarms[i] = alarms[i + 1];
                    }
                    alarmCount--;
                } else {
                    alarms[i].actions = tempAlarm.actions;
                }
            } else {
                //no alarm existed at this time -- add one 
                if(alarmCount >= MAX_ALARMS) {
                    printf("Error! Maximum number of alarms reached.\n");
                } else {
                    //shift all further alarms down
                    j = alarmCount;
                    while(j > i) {
                        alarms[j] = alarms[j - 1];
                    }
                    alarms[i] = tempAlarm;

                    //check if the new alarm should have already happened today
                    //if so, then increase the index of the next alarm by one
                    rawTime = time(NULL);
                    processedTime = timeToAlarmTime(rawTime);
                    if(processedTime > tempAlarm.time) {
                        nextAlarm++;
                    }
                    alarmCount++;
                }
            }
        }
        
        if(alarmCount > 0) {
            rawTime = time(NULL);
            processedTime = timeToAlarmTime(rawTime);
            tempAlarm = alarms[nextAlarm];

            //set the wraparound flag if it is after 2300
            if(processedTime > timeToAlarmTime(23, 0)) {
                canWraparound = 1;
            }

            //if there are any possible alarms left in the day
            if(nextAlarm < alarmCount) {
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
                //  add a daily alarm wraparound check here?
                //  should not happen -- user alarms are only (hour, minute) precision
                //  there is an entire 60 second timeframe for the just-before-midnight alarm to trigger
            } else {
                //there are no alarms remaining in the day.
                //if the day has just barely begun, wrap around to the start of the list of alarms
                if(processedTime < timeToAlarmTime(0, 1)) {
                    nextAlarm = 0;
                    canWraparound = 0;
                }
                //possible edge case failure:
                //  one single alarm just after midnight, continually triggered, since wrapping around
                //  the canWraparound flag prevents this
            }
        }

    }
}



