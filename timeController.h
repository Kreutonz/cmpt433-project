/* 
[DESCRIPTION]: 
    - handles the current time and alarm control
*/

#ifndef TIME_CONTROLLER_H_
#define TIME_CONTROLLER_H_

#include <time.h>
#include "general.h"
#include <stdbool.h>

enum STATUS TimeController_getStatus(void);
time_t TimeController_getCurrentTime(void);
int TimeController_getCurrentHours(void);
int TimeController_getCurrentMinutes(void);
int TimeController_getCurrentSeconds(void);
void TimeController_init(void);
void TimeController_setStatus(enum STATUS status);
void TimeController_shutdown(void);


void TimeController_setAlarmTime(time_t alarmTime);
void TimeController_setNewAlarm(struct tm alarm);
struct tm TimeController_getNewAlarm();
int TimeController_getAlarmInSeconds();
bool TimeController_getAlarmStatus();
void TimeController_setAlarmStatus(bool newAlarmStatus);
void TimeController_resetAlarm();
void TimeController_snoozeAlarm();
#endif