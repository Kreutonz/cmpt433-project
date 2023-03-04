#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "general.h"
#include "time.h"

#define BUFFER_LENGTH 1024
#define NS_PER_MS 1000000
#define NS_PER_SEC 1000000000

//**************************
//    FUNCTIONS (PUBLIC)
//**************************

void General_readFileContentsToBuffer(char* fileName, char* buff) {
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1);
    }

    fgets(buff, BUFFER_LENGTH, pFile);         // Read string (line)
   
    fclose(pFile);                          // Close
}// readFileContentsToBuffer()


/* [INFO] Returns an integer value from a file (used for reading pin values) */
int General_readIntegerFromFile(char *fileName) {
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1);
    }

    const int MAX_LENGTH = BUFFER_LENGTH;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);         // Read string (line)
   
    fclose(pFile);                          // Close
    
    return atoi(buff);
}// readValueFromFile()


int General_runCommand(char* command) {
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it; so we don't get an error when closing the pipe
    char buffer[BUFFER_LENGTH];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("[DEBUG] --> %s", buffer); // Uncomment for debugging
    }

    int exitCode = WEXITSTATUS(pclose(pipe));       // Get the exit code from the pipe; non-zero is an error:
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf("  command:   %s\n", command);
        printf("  exit code: %d\n", exitCode);
    } 

    return exitCode;
}// General_runCommand()


void General_sleepForMs(long long delayInMs) {
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SEC;
    int nanoseconds = delayNs % NS_PER_SEC;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}// LightSensor_sleepForMs()