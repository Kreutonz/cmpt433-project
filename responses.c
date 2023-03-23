// [SOURCE]: Converting strings to lowercase in c (https://stackoverflow.com/questions/2661766/how-do-i-lowercase-a-string-in-c) 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "responses.h"
 #include "audioControl.h"
#include "general.h"
#include "timeController.h"

#define MAX_PACKET_LENGTH_BYTES 1500
#define BUFFER_LENGTH 1024
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE 60

// ****************************
//     GLOBAL VARIABLES
// ****************************

static int alarmMode = DEFAULT1;
static pthread_mutex_t responsesMutex = PTHREAD_MUTEX_INITIALIZER;

//**************************
//   PROTOTYPES (PRIVATE)
//**************************

// static char* convertAlarmModeToString(enum ALARM_MODE alarmMode);
static char* generateResponse(char* request);
static char* getCurrentTime(void);
// static char* setAlarmMode(enum ALARM_MODE alarmMode);
static char* setAlarmTime(time_t alarmTime);
// static char* getAlarmMode(void);
// static char* getAlarmTime(void);
// static char* playAlarmSound(enum ALARM_MODE mode);
static char* invalid(void);
static char* stopProgram(void);
static struct tm alarm;

//**************************
//   FUNCTIONS (PRIVATE)
//**************************

// static char* convertAlarmModeToString(enum ALARM_MODE alarmMode) {
//     if(alarmMode == 0) {
//         return "DEFAULT1"; 
//     } else if (alarmMode == 1) {
//         return "DEFAULT2";
//     } else if (alarmMode == 2) {
//         return "CUSTOM";
//     }
//     return NULL;
// }// convertAlarmModeToString()
static time_t process_alarmTime(char *time_details)
{
	struct tm tm;
    strtok(time_details, ",");
    char *hours = strtok(NULL,",");
    char *mins = strtok(NULL,",");
    char *sec = strtok(NULL,",");
    tm.tm_hour = atoi(hours);
    tm.tm_min = atoi(mins);
    tm.tm_sec = atoi(sec);
    time_t tm_info = mktime(&tm);
    printf("Alarm time set to: %d:%d:%d\n", tm.tm_hour,tm.tm_min,tm.tm_sec);
    alarm = tm;
    TimeController_setNewAlarm(alarm);
    return tm_info;
}
static char* playAlarmSound(enum ALARM_MODE mode) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    SoundHandler_playDefaultSound(mode);
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "play = %d\n", mode);

    return pResponse;
}// playAlarmSound()

static char* generateResponse(char* request) {
    char* pResponse;
    if(strcmp(request, "getCurrentTime\n") == 0) {
        pResponse = getCurrentTime();                     
    } else if(strncmp(request, "setAlarmTime",12) == 0) {
        time_t alarm_time = process_alarmTime(request);
        pResponse = setAlarmTime(alarm_time);
    } else if(strcmp(request, "playDefault1\n") == 0) {             
         pResponse = playAlarmSound(DEFAULT1); 
         alarmMode = DEFAULT1;
    } else if(strcmp(request, "playDefault2\n") == 0) {             
         pResponse = playAlarmSound(DEFAULT2); 
         alarmMode = DEFAULT2;
    } else if(strcmp(request, "playDefault3\n") == 0) {             
         pResponse = playAlarmSound(DEFAULT3); 
         alarmMode = DEFAULT3;
    } else if(strcmp(request, "playCustom1\n") == 0) {             
         pResponse = playAlarmSound(CUSTOM1); 
         alarmMode = CUSTOM1; 
    } else if(strcmp(request, "playCustom2\n") == 0) {             
         pResponse = playAlarmSound(CUSTOM2); 
         alarmMode = CUSTOM2; 
    } 
    else if(strcmp(request, "playRickRoll\n") == 0) {             
         pResponse = playAlarmSound(RICKROLL);
         alarmMode = RICKROLL;
    }
    else if(strcmp(request, "playPunjabi\n") == 0) {
        pResponse = playAlarmSound(PUNJABI);
        alarmMode = PUNJABI;
    }   
    else if(strcmp(request, "playStop\n") == 0) {             
         pResponse = playAlarmSound(STOP);  
    } else if(strcmp(request, "terminate\n") == 0) {
        alarmMode = STOP; 
        pResponse = stopProgram();                   
    } else if(strcmp(request, "check") == 0) {
        char* resp = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
        sprintf(resp, "bbgdata, %d, %d, %d, %d", 123,AudioMixer_getVolume(),11,33);
        pResponse = resp;
    } else { 
        pResponse = invalid();
    }

    return pResponse;
}// generateReponse()


// static char* getAlarmMode(void) {
//     enum ALARM_MODE alarmMode = SoundHandler_getAlarmMode();
//     char* value = convertAlarmModeToString(alarmMode);

//     char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
//     snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "alarmMode = %s\n", value);

//     return pResponse;
// }// getAlarmMode()

static char* getCurrentTime(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    int currentHour = TimeController_getCurrentHours();
    int currentMinute = TimeController_getCurrentMinutes();
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "The current time is %d:%d\n", currentHour, currentMinute);

    return pResponse;
}// getCurrentTime()

static char* getAlarmTime(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    int alarmHour = 1;
    int alarmMinute = 2;
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "The alarm is set for %d:%d\n", alarmHour, alarmMinute);

    return pResponse;
}// getAlarmTime()

static char* setAlarmTime(time_t alarmTime) {
    TimeController_setAlarmTime(alarmTime);
    char* pResponse = getAlarmTime();
    return pResponse;
}// setAlarmTime()

// static char* setAlarmMode(enum ALARM_MODE alarmMode) {
//     SoundHandler_setAlarmMode(alarmMode);
//     char* pResponse = getAlarmMode();

//     return pResponse;
// }// setAlarmMode()

static char* invalid(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
    char message[] = "Invalid request\n";

    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, message);

    return pResponse;
}// invalid()


static char* stopProgram(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
    char message[] = "Program terminating.\n";
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, message);

    return pResponse;
}// stopProgram()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

char* Responses_handler(char* request, int* length) {
    char* pResponse = generateResponse(request);
    *length = strlen(pResponse);

    return pResponse;
}// Responses_Handler()


enum ALARM_MODE Responses_getAlarmMode(void) {
    pthread_mutex_lock(&responsesMutex);
    enum ALARM_MODE mode = alarmMode;
    pthread_mutex_unlock(&responsesMutex);
   
   return mode;
} // enum ALARM_MODE Responses_getAlarmMode


void Responses_setAlarmMode(enum ALARM_MODE newMode) {
   pthread_mutex_lock(&responsesMutex);
   alarmMode = newMode;
   pthread_mutex_unlock(&responsesMutex);
} // void Responses_setAlarmMode()