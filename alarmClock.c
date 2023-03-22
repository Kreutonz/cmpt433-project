#include <stdio.h>
#include <stdlib.h>

#include "alarmClock.h"
#include "joystick.h"
#include "segDisplay.h"
#include "timeController.h"
#include "audioControl.h"
#include "udp.h"
#include "potDriver.h"
#include "ledController.h"

//***************************
//    PRIVATE PROTOTYPES
//***************************

static void init(void);
static void shutdown(void);

//***************************
//          MAIN
//***************************

int main(void) {
    init();
    shutdown();

    return 0;
}// main()


//***************************
//    PRIVATE FUNCTIONS
//***************************

static void init(void) {
    TimeController_init();
    AudioController_init();
    SegDisplay_init();
    Joystick_init();
    potDriver_init();
    UDP_init();
    LedController_init();

}// init()


static void shutdown(void) {
    UDP_shutdown();
    Joystick_shutdown();
    SegDisplay_shutdown();
    TimeController_shutdown();
    AudioController_shutdown();
    potDriver_shutdown();
    LedController_shutDown();
}// shutdown()


//***************************
//    PUBLIC FUNCTIONS
//***************************

void AlarmClock_terminateThreads(void) {
    UDP_setStatus(TERMINATE);
    Joystick_setStatus(TERMINATE);
    SegDisplay_setStatus(TERMINATE);
    TimeController_setStatus(TERMINATE);
    LedController_setStatus(TERMINATE);
}// AlarmClock_terminateThreads()