// [SOURCE]: getting time in C: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "timeController.h"
#include "general.h"
#include "ledController.h"

#define MS_BETWEEN_ALARM_CHECKS 500
#define MS_BETWEEN_SETTING_TIME 500

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

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static void* checkAlarm(void* args) {
    while(status == RUNNING) {
        int alarmInSec = TimeController_getAlarmInSeconds();
        printf("start alarmInSec %d\n", alarmInSec);
        int currentTimeInSec = (currentHours * 3600) + (currentMinutes * 60) + currentSeconds;
        pthread_mutex_lock(&alarmMutex);
        if(currentTimeInSec >= alarmInSec && alarmInSec != 0) {
            printf("start alarmInSec %d\n", alarmInSec);
            LedController_setAlarmStatus(ON);
            General_sleepForMs(1000);
            TimeController_resetAlarm();
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
 }

struct tm TimeController_getNewAlarm() {
    pthread_mutex_lock(&alarmMutex);
    struct tm time = newAlarmTime;
    pthread_mutex_unlock(&alarmMutex);

    return time;
}

int TimeController_getAlarmInSeconds() {
    struct tm time = TimeController_getNewAlarm();
    int hoursInSec = time.tm_hour * 3600;
    int minInSec = time.tm_min * 60;
    int sec = time.tm_sec;
    int total = hoursInSec + minInSec + sec;
    return total;
}

void TimeController_resetAlarm() {
    struct tm time;
    time.tm_hour = 0;
    time.tm_min = 0;
    time.tm_sec = 0;
    TimeController_setNewAlarm(time);
} 