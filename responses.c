// [SOURCE]: Converting strings to lowercase in c (https://stackoverflow.com/questions/2661766/how-do-i-lowercase-a-string-in-c) 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "responses.h"
#include "musicHandler.h"
#include "general.h"

#define MAX_PACKET_LENGTH_BYTES 1500
#define BUFFER_LENGTH 1024
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE 60

// ****************************
//     GLOBAL VARIABLES
// ****************************


//**************************
//   PROTOTYPES (PRIVATE)
//**************************

static char* convertBeatmodeToString(enum BEAT_MODE beatmode);
static char* generateResponse(char* request);
static char* getBeatMode(void);
static char* getTempo(void);
static char* getVolume(void);
static char* invalid(void);
static char* playDefaultSound(enum DEFAULT_SOUNDS sound);
static char* setBeatMode(enum BEAT_MODE beatmode);
static char* setTempo(enum LEVEL tempo);
static char* setVolume(enum LEVEL volume);
static char* stopProgram(void);


//**************************
//   FUNCTIONS (PRIVATE)
//**************************

static char* convertBeatmodeToString(enum BEAT_MODE beatmode) {
    if(beatmode == 0) {
        return "None"; 
    } else if (beatmode == 1) {
        return "Rock #1";
    } else if (beatmode == 2) {
        return "Rock #2";
    }
    return NULL;
}// convertBeatmodeToString()


static char* generateResponse(char* request) {
    char* pResponse;
    if(strcmp(request, "setBeatModeNone\n") == 0) {
        pResponse = setBeatMode(NONE);
    } else if(strcmp(request, "setBeatModeRock1\n") == 0) {             
        pResponse = setBeatMode(ROCK1);                         
    } else if(strcmp(request, "setBeatModeRock2\n") == 0) {             
        pResponse = setBeatMode(ROCK2); 
    } else if(strcmp(request, "getBeatMode\n") == 0) {             
        pResponse = getBeatMode(); 
    } else if(strcmp(request, "volumeUp\n") == 0) {             
        pResponse = setVolume(INCREASE); 
    } else if(strcmp(request, "volumeDown\n") == 0) {             
        pResponse = setVolume(DECREASE); 
    } else if(strcmp(request, "getVolume\n") == 0) {             
        pResponse = getVolume(); 
    } else if(strcmp(request, "tempoUp\n") == 0) {             
        pResponse = setTempo(INCREASE); 
    } else if(strcmp(request, "tempoDown\n") == 0) {             
        pResponse = setTempo(DECREASE); 
    } else if(strcmp(request, "getTempo\n") == 0) {             
        pResponse = getTempo(); 
    } else if(strcmp(request, "playHihat\n") == 0) {             
        pResponse = playDefaultSound(HI_HAT); 
    } else if(strcmp(request, "playSnare\n") == 0) {             
        pResponse = playDefaultSound(SNARE); 
    } else if(strcmp(request, "playBase\n") == 0) {             
        pResponse = playDefaultSound(BASE);  
    } else if(strcmp(request, "terminate\n") == 0) {
        pResponse = stopProgram();                   
    } else { 
        pResponse = invalid();
    }

    return pResponse;
}// generateReponse()


static char* getBeatMode(void) {
    enum BEAT_MODE beatmode = MusicHandler_getBeatMode();
    char* value = convertBeatmodeToString(beatmode);

    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "beatmode = %s\n", value);

    return pResponse;
}// getBeatMode()


static char* getTempo(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    int tempo = MusicHandler_getTempo();
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "tempo = %d\n", tempo);

    return pResponse;
}// getTempo()


static char* getVolume(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    int volume = MusicHandler_getVolume();
    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "volume = %d\n", volume);

    return pResponse;
}// getVolume()


static char* invalid(void) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));
    char message[] = "Invalid request\n";

    snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, message);

    return pResponse;
}// invalid()


static char* playDefaultSound(enum DEFAULT_SOUNDS sound) {
    char* pResponse = malloc(MAX_PACKET_LENGTH_BYTES + sizeof('\n'));

    if(sound == HI_HAT) {
        MusicHandler_playDefaultSound(HI_HAT);
        snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "play = Hi-hat\n");
    } else if(sound == SNARE) {
        MusicHandler_playDefaultSound(SNARE);
        snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "play = Snare\n");
    } else if(sound == BASE) {
        MusicHandler_playDefaultSound(BASE);
        snprintf(pResponse, MAX_PACKET_LENGTH_BYTES, "play = Base\n");
    }

    return pResponse;
}// playDefaultSound()


static char* setBeatMode(enum BEAT_MODE beatmode) {
    MusicHandler_setBeatMode(beatmode);
    char* pResponse = getBeatMode();

    return pResponse;
}// setBeatMode()


static char* setTempo(enum LEVEL tempo) {
    MusicHandler_setTempo(tempo);
    char* pResponse = getTempo();

    return pResponse;
}// setTempo()


static char* setVolume(enum LEVEL volume) {
    MusicHandler_setVolume(volume);
    char* pResponse = getVolume();

    return pResponse;
}// setVolume()


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

