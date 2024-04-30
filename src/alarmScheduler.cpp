//Spencer Butler, CS 452, Final Project

#include <Arduino.h>
#include <time.h>

#include "alarmScheduler.hpp"
QueueHandle_t newAlarms;
QueueHandle_t listAlarms;
QueueHandle_t schedulerCommands;


#include "lightManager.hpp"

#include "lcdManager.hpp"

//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(time_t t) {
    tm *now = localtime(&t);

    return timeToAlarmTime(now->tm_hour, now->tm_min) + now->tm_sec;
}

//returns the time formatted appropriately for use in defining an alarm
unsigned int timeToAlarmTime(int hours, int minutes) {
    //guarantee the time is within the day
    hours = hours + (minutes / 60);
    minutes = minutes % 60;
    hours = hours % 24;

    //in C, a % b will return negative values, given negative a.
    //this would not be suitable here -- hour -2 means hour 22, not hour -2.
    if(hours < 0) { hours = 24 + hours; }
    if(minutes < 0) { minutes = 60 + minutes; }

    return (hours * 60 * 60) + (minutes * 60);
}

//converts from alarm time back to hours and minutes
void timeFromAlarmTime(int &hours, int &minutes, unsigned int alarmTime) {
    alarmTime /= 60;
    minutes = alarmTime % 60;
    hours = alarmTime / 60;
}

//task which coordinates alarm events
void task_alarmScheduler(void *p) {
    alarmDefinition alarms[MAX_ALARMS];
    alarmDefinition tempAlarm;

    time_t rawTime;
    tm *structuredTime;
    unsigned int processedTime;
    
    //currently ongoing alarm actions
    unsigned int currentActions = 0;

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
        //time out every few ticks to check if the current alarm can go off, or if there are immediate commands
        while(xQueueReceive(newAlarms, &tempAlarm, 5)) {
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
                        i++;
                    }
                    alarmCount--;
                } else {
                    alarms[i].actions = tempAlarm.actions;
                }
            } else {
                //no alarm existed at this time -- add one 
                if(alarmCount >= MAX_ALARMS) {
                    printf("Error! Maximum number of alarms reached.\n");
                } else if(tempAlarm.actions == 0) {
                    printf("Error! Attempted to add an empty alarm, or delete an alarm which was not found.\n");
                } else {
                    //shift all further alarms down
                    j = alarmCount;
                    while(j > i) {
                        alarms[j] = alarms[j - 1];
                        j--;
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
        
        //time-based checks for triggering alarms
        if(alarmCount > 0) {
            rawTime = time(NULL);
            processedTime = timeToAlarmTime(rawTime);

            //set the wraparound flag if it is after 2300
            if(processedTime > timeToAlarmTime(23, 0)) {
                canWraparound = 1;
            }

            //if there are any possible alarms left in the day
            if(nextAlarm < alarmCount) {
                tempAlarm = alarms[nextAlarm];
                if(processedTime > tempAlarm.time) {
                    //the now-current actions are whatever was happening before, along with this alarm
                    currentActions = currentActions | tempAlarm.actions;
                    nextAlarm++;
                    if(tempAlarm.actions & ACTION_LIGHT) {
                        signal = LIGHT_SIGNAL_ON;
                        xQueueSendToBack(lightActions, &signal, 0);
                    }
                    if(tempAlarm.actions & ACTION_RING) {
                        //TODO: standardize frequency?
                        tone(ALARM_PIN, 1700);
                    }
                    if(tempAlarm.actions & ACTION_ONCE) {
                        //delete alarm -- send alarm with same time and actions = 0 to newAlarms
                        changeAlarm(tempAlarm.time, 0);
                    }
                }
                //possible edge case failure:
                //  alarm right before midnight, doesn't ring, since time never less than its
                //  add a daily alarm wraparound check here?
                //  should not happen -- user alarms are only (hour, minute) precision
                //  there is an entire 60 second timeframe for the just-before-midnight alarm to trigger
                //  snooze-alarms are similarly minute-precision
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

        //check for any immediate commands to the scheduler
        while(xQueueReceive(schedulerCommands, &signal, 0)) {
            switch(signal) {
                case COMMAND_SNOOZE:
                    if(currentActions) {
                        //signal the same actions to happen SNOOZE_LENGTH later
                        rawTime = time(NULL);
                        structuredTime = localtime(&rawTime);
                        tempAlarm.actions = currentActions | ACTION_ONCE;
                        //add 1 to number of minutes of snooze length to round, e.g. 12:35:23 up to 12:36
                        //schedule it for 1 second after the minute to avoid overwriting any permanent alarms
                        tempAlarm.time = 1 + timeToAlarmTime(structuredTime->tm_hour, 
                                                             structuredTime->tm_min + SNOOZE_LENGTH + 1);
                        xQueueSendToBack(newAlarms, &tempAlarm, 0);
                    }
                    //no break -- a snooze should do the same things as off, in addition to other stuff
                case COMMAND_OFF:
                    currentActions = 0;
                    //guarantee light is turned off
                    i = LIGHT_SIGNAL_OFF;
                    xQueueSendToBack(lightActions, &i, 0);

                    //guarantee buzzer stops buzzing
                    tone(ALARM_PIN, 0);
                    break;
                case COMMAND_LIST:
                    for(i = 0; i < alarmCount; i++) {
                        xQueueSendToBack(listAlarms, alarms + i, 0);
                    }
                    break;
                default:
                    printf("Error! Scheduler received unknown command: %d\n", signal);
            }
        }
    }
}

//wrapper which sends a queue message to turn off the alarm
void offAlarm() {
    int message = COMMAND_OFF;
    xQueueSendToBack(schedulerCommands, &message, 0);
}

//wrapper which sends a queue message to snooze the alarm
void snoozeAlarm() {
    int message = COMMAND_SNOOZE;
    xQueueSendToBack(schedulerCommands, &message, 0);
}


//wrapper which sends an alarm-change request to newAlarms
void changeAlarm(unsigned int time, unsigned int actions) {
    alarmDefinition message;
    message.time = time;
    message.actions = actions;
    xQueueSendToBack(newAlarms, &message, 0);
}

//wrapper which sends an alarm-change request to newAlarms
void changeAlarm(int hours, int minutes, unsigned int actions) {
    changeAlarm(timeToAlarmTime(hours, minutes), actions);
}


