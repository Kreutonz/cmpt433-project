#define _DEFAULT_SOURCE
/*
[DESCRIPTION]:
- Plays a sound that says the current time when triggered using the terminal command "espeak".
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
    char timeString[20];
    strftime(timeString, sizeof(timeString), "The time is %I %M", localTime);

    // Generate the WAV file using the espeak command
    char command[256];
    snprintf(command, sizeof(command), "espeak '%s' -w temp.wav --samplerate=44100", timeString);
    system(command);

    // Read the WAV file into memory
    wavedata_t soundData;
    AudioMixer_readWaveFileIntoMemory("temp.wav", &soundData);

    // Play the sound using AudioMixer_queueSound
    AudioMixer_queueSound(&soundData);

    // Wait for the sound to finish playing
    int soundDurationMs = (soundData.numSamples * 1000) / 22050;
    usleep(soundDurationMs * 1000);

    // Free the memory and delete the WAV file
    AudioMixer_freeWaveFileData(&soundData);
    remove("temp.wav");
}
