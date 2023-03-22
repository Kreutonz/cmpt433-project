#define _DEFAULT_SOURCE
/* 
[DESCRIPTION]: 
    - Plays mp3 audio that is stored in the sounds directory using the mpg123 command.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "audioControl.h"
#include "timeController.h"


void play_mp3(char *mp3_file) {
    char command[256];
    snprintf(command, sizeof(command), "mpg123 -o alsa sounds/%s.mp3", mp3_file);
    system(command);
}
