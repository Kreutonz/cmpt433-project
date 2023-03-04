/*
[DESCRIPTION]: 
    - initializes and sets the zencape pins for the seg display
    - uses two threads:
        - 1. fetches the time value 
        - 2. displays time value to the zencape seg display
*/

#ifndef SEGDISPLAY_H_
#define SEGDISPLAY_H_

#include "general.h"

enum DISPLAY_MODE{HOURS, MINUTES, SECONDS, ALL};


enum STATUS SegDisplay_getStatus(void);
void SegDisplay_init(void);
void SegDisplay_setDisplayMode(enum DISPLAY_MODE value);
void SegDisplay_setStatus(enum STATUS value);
void SegDisplay_shutdown(void);

#endif