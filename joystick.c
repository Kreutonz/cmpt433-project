#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "general.h"
#include "joystick.h"
#include "segDisplay.h"
#include "timeController.h"

#define JOYSTICK_PIN_CONFIGURE_UP "config-pin p8.14 gpio"
#define JOYSTICK_PIN_CONFIGURE_RIGHT "config-pin p8.15 gpio"
#define JOYSTICK_PIN_CONFIGURE_DOWN "config-pin p8.16 gpio"
#define JOYSTICK_PIN_CONFIGURE_LEFT "config-pin p8.18 gpio"
#define JOYSTICK_PIN_CONFIGURE_PUSH "config-pin p8.17 gpio"

#define JOYSTICK_CONFIGURE_DIRECTION_UP "cd /sys/class/gpio/gpio26 && echo in > direction"
#define JOYSTICK_CONFIGURE_DIRECTION_RIGHT "cd /sys/class/gpio/gpio47 && echo in > direction"
#define JOYSTICK_CONFIGURE_DIRECTION_DOWN "cd /sys/class/gpio/gpio46 && echo in > direction"
#define JOYSTICK_CONFIGURE_DIRECTION_LEFT "cd /sys/class/gpio/gpio65 && echo in > direction"
#define JOYSTICK_CONFIGURE_DIRECTION_PUSH "cd /sys/class/gpio/gpio27 && echo in > direction"

#define JOYSTICK_UP_VALUE "/sys/class/gpio/gpio26/value"
#define JOYSTICK_RIGHT_VALUE "/sys/class/gpio/gpio47/value"
#define JOYSTICK_DOWN_VALUE "/sys/class/gpio/gpio46/value"
#define JOYSTICK_LEFT_VALUE "/sys/class/gpio/gpio65/value"
#define JOYSTICK_PUSH_VALUE "/sys/class/gpio/gpio27/value"

#define MS_BETWEEN_READS 150

static pthread_mutex_t joystickMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t joystickThreadID;
static int status = RUNNING;

//**************************
//    PROTOTYPES (PRIVATE)
//**************************

static void configurePins(void);
static void* readJoystick(void* args);

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static void configurePins(void){
    // configure pins to gpio
    General_runCommand(JOYSTICK_PIN_CONFIGURE_UP);    
    General_runCommand(JOYSTICK_PIN_CONFIGURE_RIGHT);   
    General_runCommand(JOYSTICK_PIN_CONFIGURE_DOWN);   
    General_runCommand(JOYSTICK_PIN_CONFIGURE_LEFT);   
    General_runCommand(JOYSTICK_PIN_CONFIGURE_PUSH);   

    // configure pins to input
    General_runCommand(JOYSTICK_CONFIGURE_DIRECTION_UP);  
    General_runCommand(JOYSTICK_CONFIGURE_DIRECTION_RIGHT); 
    General_runCommand(JOYSTICK_CONFIGURE_DIRECTION_DOWN);  
    General_runCommand(JOYSTICK_CONFIGURE_DIRECTION_LEFT);  
    General_runCommand(JOYSTICK_CONFIGURE_DIRECTION_PUSH); 
}// configurePins()

static void* readJoystick(void* args) {
    while(status == RUNNING){
        int upValue = General_readIntegerFromFile(JOYSTICK_UP_VALUE);
        int rightValue = General_readIntegerFromFile(JOYSTICK_RIGHT_VALUE);
        int downValue = General_readIntegerFromFile(JOYSTICK_DOWN_VALUE);
        int leftValue = General_readIntegerFromFile(JOYSTICK_LEFT_VALUE);
        int pushValue = General_readIntegerFromFile(JOYSTICK_PUSH_VALUE);

        if(leftValue == 0) {
            SegDisplay_setDisplayMode(HOURS);
            printf("[TODO]: if alarm activated (playing sound) --> SNOOZE (setter)\n");
        } else if(upValue == 0) {
            SegDisplay_setDisplayMode(MINUTES);
            printf("[TODO]: if alarm activated (playing sound) --> SNOOZE (setter)\n");
        } else if(rightValue == 0) {
            SegDisplay_setDisplayMode(SECONDS);
            printf("[TODO]: if alarm activated (playing sound) --> SNOOZE (setter)\n");
        } else if(downValue == 0) {
            SegDisplay_setDisplayMode(ALL);
            printf("[TODO]: if alarm activated (playing sound) --> SNOOZE (setter)\n");
        } else if(pushValue == 0) {
            printf("[TODO]: if alarm activated (playing sound) --> SHUTOFF ALARM (setter)\n");
        } else {
            SegDisplay_setDisplayMode(ALL);         // idle joystick
        }

        General_sleepForMs(MS_BETWEEN_READS);
    }

    return NULL;
}// readJoystick()

//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum STATUS Joystick_getStatus(void) {
    pthread_mutex_lock(&joystickMutex);
    enum STATUS stat = status;
    pthread_mutex_unlock(&joystickMutex);

    return stat;
}// Joystick_getStatus()


void Joystick_init(void) {
    configurePins();

    pthread_create(&joystickThreadID, NULL, &readJoystick, NULL);
}// Joystick_init()


void Joystick_setStatus(enum STATUS newStatus) {
    pthread_mutex_lock(&joystickMutex);
    status = newStatus;
    pthread_mutex_unlock(&joystickMutex);
}// Joystick_setStatus()


void Joystick_shutdown(void) {
    pthread_join(joystickThreadID, NULL);
}// Joystick_shutdown()
