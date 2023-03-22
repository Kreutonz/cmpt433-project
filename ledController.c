#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ledController.h"
#include "general.h"

#define trigger1 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define brightness1 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define trigger2 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define brightness2 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define trigger3 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define brightness3 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define trigger4 "/sys/class/leds/beaglebone:green:usr3/trigger"
#define brightness4 "/sys/class/leds/beaglebone:green:usr3/brightness"
#define triggerForWiredLED1 "/sys/class/leds/leds:P9.23/trigger"
#define brightnessForWiredLED1 "/sys/class/leds/leds:P9.23/brightness"
#define GPIO_EXPORT_115 "echo 115 > /sys/class/gpio/export"
#define GPIO_EXPORT_SLEEP_MS 500
#define GPIO_DIRECTION_115 "echo out > /sys/class/gpio/gpio115/direction"
#define GPIO_VALUE_115 "/sys/class/gpio/gpio115/value"
#define MAX_ATTEMPTS 5

// ********************
// GLOBAL VARIABLES
// ********************

static enum ALARM_STAUS alarmStatus = OFF;
static enum STATUS status = RUNNING;
static pthread_t ledThreadID;
static pthread_mutex_t ledControllerMutex = PTHREAD_MUTEX_INITIALIZER;

// *********************
// PROTOTYPES (PRIVATE)
// *********************
static void bruteForceConfig(char* command);
static void config_pins(void);
static void initLeds(void);
static void turnAllBBLightsOff(void);
static void turnAllBBLightsOn(void);
static void* turnAlarmLightsOnAndOff(void* args);

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static void bruteForceConfig(char* command) {
	int exitCode = -1;
	int attempts = 0;

	while(exitCode != 0 && attempts < MAX_ATTEMPTS) {
		exitCode = General_runCommand(command);
		attempts++;
		if(exitCode != 0) {
			printf("[FAILED] '%s'....", command); 
			if(attempts < MAX_ATTEMPTS) {
				printf(" Trying again");
			} else {
				perror("Cannot execute successfully: ");
				printf("%s\n", command);
				exit(exitCode);
			}
			printf("\n");
		}
	}
	printf("[SUCCESS] %s with %d attempts\n", command, attempts);
}// bruteForceConfig()


static void config_pins(void) {
	
	// export pins for gpio
    bruteForceConfig(GPIO_EXPORT_115);

    // sleep until pins are exported
    General_sleepForMs(GPIO_EXPORT_SLEEP_MS);

	 // set direction for pins
    General_runCommand(GPIO_DIRECTION_115);

    // turn off led after its exported
    General_runCommand("echo 1 > " GPIO_VALUE_115);
    General_sleepForMs(GPIO_EXPORT_SLEEP_MS);

	printf("[SYSTEM] Pins exported successully...\n");
}// config_pins()


static void initLeds(void) {
   config_pins();
   General_sleepForMs(GPIO_EXPORT_SLEEP_MS);
   General_runCommand("echo none > " trigger1);
   General_runCommand("echo none > " trigger2);
   General_runCommand("echo none > " trigger3);
   General_runCommand("echo none > " trigger4);
   General_runCommand("echo none > " triggerForWiredLED1);
} // initLeds()


static void turnAllBBLightsOff(void) {
   General_runCommand("echo 0 > " brightness1);
   General_runCommand("echo 0 > " brightness2);
   General_runCommand("echo 0 > " brightness3);
   General_runCommand("echo 0 > " brightness4);
} // LedController_turnAllLightsOff()


static void turnAllBBLightsOn(void) {
   General_runCommand("echo 1 > " brightness1);
   General_runCommand("echo 1 > " brightness2);
   General_runCommand("echo 1 > " brightness3);
   General_runCommand("echo 1 > " brightness4);
} // LedController_turnAllLightsOn()


static void* turnAlarmLightsOnAndOff(void* args) {
   while (status == RUNNING) {
      if (alarmStatus == ON) {
         // Turn one led on one off
         turnAllBBLightsOn();
         General_runCommand("echo 1 > " brightnessForWiredLED1);
         General_runCommand("echo 1 > " GPIO_VALUE_115);
         General_sleepForMs(250);
         General_runCommand("echo 0 > " brightnessForWiredLED1);
         General_runCommand("echo 0 > " GPIO_VALUE_115);
         General_sleepForMs(250);
      } else if (alarmStatus == OFF) {
         // Turn both leds off
         General_runCommand("echo 0 > " brightnessForWiredLED1);
         General_runCommand("echo 1 > " GPIO_VALUE_115);
         turnAllBBLightsOff();
      }
   }
   // Turn both leds off
   General_runCommand("echo 0 > " brightnessForWiredLED1);
   General_runCommand("echo 1 > " GPIO_VALUE_115);
   turnAllBBLightsOff();
   return NULL;
} // LedController_turnAlarmLightsOn()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum ALARM_STAUS LedController_getAlarmStatus(void) {
   pthread_mutex_lock(&ledControllerMutex);
   enum ALARM_STAUS stat = alarmStatus;
   pthread_mutex_unlock(&ledControllerMutex);
   
   return stat;
} // LedController_getAlarmStatus()

enum STATUS LedController_getStatus(void) {
   pthread_mutex_lock(&ledControllerMutex);
   enum STATUS stat = status;
   pthread_mutex_unlock(&ledControllerMutex);
   
   return stat;
} // enum STATUS LedController_getStatus()


void LedController_init(void) {
   initLeds();
   pthread_create(&ledThreadID, NULL, &turnAlarmLightsOnAndOff, NULL);
} // LedController_init()


void LedController_setAlarmStatus(enum ALARM_STAUS newStatus) {
   pthread_mutex_lock(&ledControllerMutex);
   alarmStatus = newStatus;
   pthread_mutex_unlock(&ledControllerMutex);
} // LedController_setAlarmStatus()


void LedController_setStatus(enum STATUS value) {
	pthread_mutex_lock(&ledControllerMutex);
	status = value;
	pthread_mutex_unlock(&ledControllerMutex);
}//SegDisplay_setStatus()


void LedController_shutDown(void) {
   turnAllBBLightsOff();
   General_runCommand("echo 0 > " brightnessForWiredLED1);
   General_runCommand("echo 1 > " GPIO_VALUE_115);
   pthread_join(ledThreadID, NULL);
} // LedController_shutDown()

