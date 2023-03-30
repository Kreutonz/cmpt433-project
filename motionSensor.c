#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "motionSensor.h"
#include "general.h"
#include "timeController.h"
#include "ledController.h"
#include "audioControl.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define MOTION_THRESHOLD 3000

// ********************
// GLOBAL VARIABLES
// ********************

static enum STATUS status = RUNNING;
static enum MOTION_SENSOR motionStatus = NO_MOVEMENT;
static pthread_t motionSensorThreadID;
static pthread_mutex_t motionSensorMutex = PTHREAD_MUTEX_INITIALIZER;

// *********************
// PROTOTYPES (PRIVATE)
// *********************

static int getVoltage1Reading();
static void* takeMotionSensorReadings(void* args);

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static int getVoltage1Reading() {
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
} // static int getVoltage1Reading()


static void* takeMotionSensorReadings(void* args) {
    while (status == RUNNING) {
        int reading = getVoltage1Reading();
        bool alarmStatus = TimeController_getAlarmStatus();
        if (alarmStatus && reading >= MOTION_THRESHOLD) {
            motionStatus = MOVEMENT;
            printf("SHUTOFF ALARM\n");
            TimeController_setAlarmStatus(false);
            LedController_setAlarmStatus(OFF);
            SoundHandler_playDefaultSound(STOP);
            TimeController_resetAlarm();
        } else {
            motionStatus = NO_MOVEMENT;
        }
           
    }
    return NULL;
} // static void* takeMotionSensorReadings()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum MOTION_SENSOR MotionSensor_getMotionStatus(void) {
    pthread_mutex_lock(&motionSensorMutex);
    enum MOTION_SENSOR stat = motionStatus;
    pthread_mutex_unlock(&motionSensorMutex);
   
   return stat;
} // enum MOTION_SENSOR MotionSensor_getMotionStatus()


enum STATUS MotionSensor_getStatus(void) {
    pthread_mutex_lock(&motionSensorMutex);
    enum STATUS stat = status;
    pthread_mutex_unlock(&motionSensorMutex);
   
   return stat;
} // enum STATUS MotionSensor_getStatus()


void MotionSensor_init(void) {
   pthread_create(&motionSensorThreadID, NULL, &takeMotionSensorReadings, NULL);
} // void MotionSensor_init()


void MotionSensor_setMotionStatus(enum MOTION_SENSOR newStatus) {
   pthread_mutex_lock(&motionSensorMutex);
   motionStatus = newStatus;
   pthread_mutex_unlock(&motionSensorMutex);
} // void MotionSensor_setMotionStatus()


void MotionSensor_setStatus(enum STATUS value) {
	pthread_mutex_lock(&motionSensorMutex);
	status = value;
	pthread_mutex_unlock(&motionSensorMutex);
}// void MotionSensor_setStatus()


void MotionSensor_shutDown(void) {
    pthread_join(motionSensorThreadID, NULL);
} // void MotionSensor_shutDown