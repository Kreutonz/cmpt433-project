/* 
[DESCRIPTION]: 
    - general purpose functions used across multiple modules
*/

#ifndef GENERAL_H_
#define GENERAL_H_

enum STATUS {TERMINATE, RUNNING};

void General_readFileContentsToBuffer(char* fileName, char* buff);
int General_readIntegerFromFile(char *fileName);
int General_runCommand(char* command);
void General_sleepForMs(long long delayInMs);

#endif
