#include <time.h>
#include <unistd.h>
#include <stdio.h>

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


// *********************
// PROTOTYPES (PRIVATE)
// *********************

void initLeds();

//**************************
//    FUNCTIONS (PRIVATE)
//**************************

void initLeds() {
   General_runCommand("echo none > " trigger1);
   General_runCommand("echo none > " trigger2);
   General_runCommand("echo none > " trigger3);
   General_runCommand("echo none > " trigger4);
   General_runCommand("echo none > " triggerForWiredLED1);
}


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

void LedController_init(void) {
   initLeds();
} // LedController_init()

void LedController_shutDown(void) {
   LedController_turnAllBBLightsOff();
   initLeds();
} // LedController_shutDown()


void LedController_turnAllBBLightsOff(void) {
   General_runCommand("echo 0 > " brightness1);
   General_runCommand("echo 0 > " brightness2);
   General_runCommand("echo 0 > " brightness3);
   General_runCommand("echo 0 > " brightness4);
   General_runCommand("echo 0 > " brightnessForWiredLED1);
} // LedController_turnAllLightsOff()


void LedController_turnAllBBLightsOn(void) {
   General_runCommand("echo 1 > " brightness1);
   General_runCommand("echo 1 > " brightness2);
   General_runCommand("echo 1 > " brightness3);
   General_runCommand("echo 1 > " brightness4);
   General_runCommand("echo 1 > " brightnessForWiredLED1);
} // LedController_turnAllLightsOn()

