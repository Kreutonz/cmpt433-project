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
#define MAX_LENGTH_OF_STRING 1024

// *********************
// PROTOTYPES (PRIVATE)
// *********************

void initLeds();


// Set all the triggers to none
void initLeds() {
   General_runCommand("echo none > " trigger1);
   General_runCommand("echo none > " trigger2);
   General_runCommand("echo none > " trigger3);
   General_runCommand("echo none > " trigger4);
}

