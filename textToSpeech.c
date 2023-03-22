#define _DEFAULT_SOURCE
/*
[DESCRIPTION]:
- Plays a sound that says the current time when triggered using espeak.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "audioControl.h"
#include "timeController.h"


void speakTime() {
    // Get the current time
    time_t currentTime = TimeController_getCurrentTime();
    struct tm *localTime = localtime(&currentTime);

    // Format the time as a string
    char hourString[20], minuteString[20], amPmString[20];
    strftime(hourString, sizeof(hourString), "%I", localTime);
    strftime(minuteString, sizeof(minuteString), "%M", localTime);
    strftime(amPmString, sizeof(amPmString), "%p", localTime);

    // Remove the leading zero from the hour string
    if (hourString[0] == '0') {
        memmove(hourString, hourString+1, strlen(hourString));
    }

    // Replace the leading zero with 'o' for the minute string
    if (minuteString[0] == '0') {
        minuteString[0] = 'o';
    }

    // Separate the AM/PM characters
    char amPmSeparated[8];
    snprintf(amPmSeparated, sizeof(amPmSeparated), "%c. %c", amPmString[0], amPmString[1]);

    char timeString[60];
    snprintf(timeString, sizeof(timeString), "The time is %s %s %s", hourString, minuteString, amPmSeparated);

    // Generate the WAV file using the espeak command
    char command[256];
    snprintf(command, sizeof(command), "espeak '%s' -w temp.wav", timeString);
    system(command);

    // Resample the WAV file from 22050 to 44100 using sox
    system("sox temp.wav -r 44100 temp_resampled.wav");

    // Read the resampled WAV file into memory
    wavedata_t soundData;
    AudioMixer_readWaveFileIntoMemory("temp_resampled.wav", &soundData);

    // Play the sound using AudioMixer_queueSound
    AudioMixer_queueSound(&soundData);

    // Wait for the sound to finish playing
    int soundDurationMs = (soundData.numSamples * 1000) / 44100;
    usleep(soundDurationMs * 1000);

    // Free the memory and delete the WAV files
    AudioMixer_freeWaveFileData(&soundData);
    remove("temp.wav");
    remove("temp_resampled.wav");
}
