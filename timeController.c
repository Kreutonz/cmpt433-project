// [SOURCE]: getting time in C: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "timeController.h"
#include "general.h"
#include "ledController.h"
#include "audioControl.h"
#include "responses.h"

#define MS_BETWEEN_ALARM_CHECKS 500
#define MS_BETWEEN_SETTING_TIME 500
#define SECS_TO_HOURS 3600
#define SECS_TO_MINS 60
#define SNOOZE_IN_SEC 10

static pthread_mutex_t timeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t timeThreadID;
static pthread_mutex_t alarmMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t alarmThreadID;

static int status = RUNNING;
static time_t currentTime;
static time_t alarmTime;
static int currentHours;
static int currentMinutes;
static int currentSeconds;
struct tm newAlarmTime;

//**************************
//    PROTOTYPES (PRIVATE)
//**************************

static void* checkAlarm(void* args);
static void* setTimeInfo(void* args);
static bool isAlarmOn = false;

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static void* checkAlarm(void* args) {
    General_sleepForMs(2000);
    while(status == RUNNING) {
        pthread_mutex_lock(&alarmMutex);
        if (newAlarmTime.tm_hour == currentHours && newAlarmTime.tm_min == currentMinutes && newAlarmTime.tm_sec == currentSeconds) {
            isAlarmOn = true;
            printf("TURN ON LIGHTS\n");
            SoundHandler_playDefaultSound(Responses_getAlarmMode());
            LedController_setAlarmStatus(ON);
        }
        pthread_mutex_unlock(&alarmMutex);

        General_sleepForMs(MS_BETWEEN_ALARM_CHECKS);
    }
    return NULL;
}// checkAlarm()


static void* setTimeInfo(void* args) {
    while(status == RUNNING) {
        pthread_mutex_lock(&timeMutex);
        currentTime = time(NULL);
        struct tm * timeinfo;
        
        timeinfo = localtime(&currentTime);

        currentHours = timeinfo->tm_hour;
        currentMinutes = timeinfo->tm_min;
        currentSeconds = timeinfo->tm_sec;

        pthread_mutex_unlock(&timeMutex);

        General_sleepForMs(MS_BETWEEN_SETTING_TIME);
    }

    return NULL;
}// setTimeInfo()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum STATUS TimeController_getStatus(void) {
    pthread_mutex_lock(&timeMutex);
    enum STATUS stat = status;
    pthread_mutex_unlock(&timeMutex);

    return stat;
}// TimeController_getStatus()


time_t TimeController_getCurrentTime(void) {
    pthread_mutex_lock(&timeMutex);
    time_t time = currentTime;
    pthread_mutex_unlock(&timeMutex);
    
    return time;
}// TimeController_getCurrentTime()


time_t TimeController_getAlarmTime(void) {
    pthread_mutex_lock(&alarmMutex);
    time_t time = alarmTime;
    pthread_mutex_unlock(&alarmMutex);
    
    return time;
}// TimeController_getAlarmTime()


int TimeController_getCurrentHours(void) {
    pthread_mutex_lock(&timeMutex);
    int hours = currentHours;
    pthread_mutex_unlock(&timeMutex);
    
    return hours;
}// TimeController_getCurrentHours()


int TimeController_getCurrentMinutes(void) {
    pthread_mutex_lock(&timeMutex);
    int minutes = currentMinutes;
    pthread_mutex_unlock(&timeMutex);
    
    return minutes;
}// TimeController_getCurrentMinutes()


int TimeController_getCurrentSeconds(void) {
    pthread_mutex_lock(&timeMutex);
    int seconds = currentSeconds;
    pthread_mutex_unlock(&timeMutex);
    
    return seconds;
}// TimeController_getCurrentSeconds()


void TimeController_init(void) {
    time(&currentTime);
    newAlarmTime.tm_hour = -1;
    newAlarmTime.tm_min = -1;
    newAlarmTime.tm_hour = -1;
    pthread_create(&timeThreadID, NULL, &setTimeInfo, NULL);
    pthread_create(&alarmThreadID, NULL, &checkAlarm, NULL);

}// TimeController_init()


void TimeController_setStatus(enum STATUS newStatus) {
    pthread_mutex_lock(&timeMutex);
    status = newStatus;
    pthread_mutex_unlock(&timeMutex);
}// TimeController_setStatus()


void TimeController_shutdown(void) {
    pthread_join(timeThreadID, NULL);
    pthread_join(alarmThreadID, NULL);
}// TimeController_shutdown()

 void TimeController_setAlarmTime(time_t alarm_Time)
 {
    alarm_Time = 0;
 }


 void TimeController_setNewAlarm(struct tm alarm) {
    pthread_mutex_lock(&alarmMutex);
    newAlarmTime = alarm;
    pthread_mutex_unlock(&alarmMutex);
 } // void TimeController_setNewAlarm(struct tm alarm) 


struct tm TimeController_getNewAlarm() {
    pthread_mutex_lock(&alarmMutex);
    struct tm time = newAlarmTime;
    pthread_mutex_unlock(&alarmMutex);

    return time;
} // struct tm TimeController_getNewAlarm()


int TimeController_getAlarmInSeconds() {
    struct tm time = newAlarmTime;
    int hoursInSec = time.tm_hour * SECS_TO_HOURS;
    int minInSec = time.tm_min * SECS_TO_MINS;
    int sec = time.tm_sec;
    int total = hoursInSec + minInSec + sec;
    return total;
} // int TimeController_getAlarmInSeconds()


bool TimeController_getAlarmStatus() {
    pthread_mutex_lock(&alarmMutex);
    bool alarmStatus = isAlarmOn;
    pthread_mutex_unlock(&alarmMutex);

    return alarmStatus;
} // TimeController_getAlarmStatus()

void TimeController_setAlarmStatus(bool newAlarmStatus) {
    pthread_mutex_lock(&alarmMutex);
    isAlarmOn = newAlarmStatus;
    pthread_mutex_unlock(&alarmMutex);
} // TimeController_setAlarmStatu()


void TimeController_resetAlarm() {
    struct tm time;
    time.tm_hour = -1;
    time.tm_min = -1;
    time.tm_sec = -1;
    TimeController_setNewAlarm(time);
} // void TimeController_resetAlarm()


void TimeController_snoozeAlarm() {
    pthread_mutex_lock(&alarmMutex);
    struct tm time;
    time.tm_hour = currentHours;
    time.tm_min = currentMinutes;
    time.tm_sec = currentSeconds + SNOOZE_IN_SEC;
    newAlarmTime = time;
    printf("Snooze time: H: %d m: %d s: %d\n",time.tm_hour, time.tm_min , time.tm_sec);
    pthread_mutex_unlock(&alarmMutex);
} // void TimeController_snoozeAlarm()