/*
[DESCRIPTION]:
    - accepts request string and length variable (which is modified and return for parsing)
    - returns a pointer in dynamic memory to initialed string based on request argument
    - valid requests:
        1. setBeatModeNone
        2. setBeatModeRock1
        3. setBeatModeRock2
        4. getBeatMode
        5. volumeUp
        6. volumeDown
        7. getVolume
        8. tempoUp
        9. tempoDown
        10. getTempo
        11. playHihat
        12. playSnare
        13. playBase
        14. terminate
*/

#ifndef RESPONSES_H_
#define RESPONSES_H_

enum ALARM_MODE {
    DEFAULT1 = 0,
    DEFAULT2,
    DEFAULT3,
    CUSTOM1,
    CUSTOM2,
    STOP
};

char* Responses_handler(char* request, int* length);

#endif 
