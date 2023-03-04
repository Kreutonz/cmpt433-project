#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>

#include "udp.h"
#include "responses.h"
#include "alarmClock.h"

#define MAX_LEN 1500
#define PORT 12345


// ****************************
//     GLOBAL VARIABLES
// ****************************

static enum STATUS status = RUNNING;
static pthread_t UdpThreadID;
static pthread_mutex_t UdpMutex = PTHREAD_MUTEX_INITIALIZER;
static int socketDescriptor = 0;

//**************************
//   PROTOTYPES (PRIVATE)
//**************************

static void closeSocket(void);
static bool createReply(struct sockaddr_in sinRemote, unsigned int sin_len, char* message);
static void createSocket(void);
static void* receiveData(void* args);
static void sendReply(struct sockaddr_in sinRemote, unsigned int sin_len, char *messageTx);


//**************************
//   FUNCTIONS (PRIVATE)
//**************************

static void closeSocket(void) {
    close(socketDescriptor);
}// closeSocket()


// [INFO]: returns false if program needs to terminate
static bool createReply(struct sockaddr_in sinRemote, unsigned int sin_len, char* message) {
    char messageTx[MAX_LEN];
    int length = 0;
    char* response = Responses_handler(message, &length);
    if(length != 0) {
        int responseIndex = 0;
        int packetIndex = 0;
        
        while(responseIndex < length) {
            if((packetIndex % MAX_LEN) == 0 && packetIndex != 0) {          // go back and find good place to null terminate
                int indexParser = MAX_LEN - 1;                              // last index in packet
                while(indexParser >= 0) {                                   // loop through the indecies until ' ' is found (no data)
                    char valueToInsert = response[indexParser];
                    if(isspace(valueToInsert) == 0) {                       // not whitespace (0), current index is not valid to break on
                        indexParser--;                               
                    } else {                                                // found index to break on (no data)
                        messageTx[indexParser] = atoi("\0");                // NULL terminate at current index
                        sendReply(sinRemote, sin_len, messageTx);           // send the packet
                        responseIndex -= (packetIndex - indexParser);       // set this index to be index to start writing into next packet
                        packetIndex = 0;                                    // reset to beginning of buffer space
                        break;
                    }
                }
            } else {
                messageTx[packetIndex] = response[responseIndex];
                responseIndex++;
                packetIndex++;
            }
        }
        messageTx[packetIndex] = atoi("\0");                                // NULL terminate, end of response
        sendReply(sinRemote, sin_len, messageTx);
    } else {
        snprintf(messageTx, MAX_LEN, "none\n");
        sendReply(sinRemote, sin_len, messageTx);
    }

    bool terminate = strcmp(messageTx, "Program terminating.\n") == 0;      // checks if the program needs to terminate
    free(response);

    if(terminate) {
        return false;
    }
    return true;
}// createReply()


static void createSocket(void) {
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
}// createSocket()


static void* receiveData(void* args) {
    struct sockaddr_in sinRemote;
    unsigned int sin_len = sizeof(sinRemote);
    while(status == RUNNING) {
        pthread_mutex_lock(&UdpMutex);
        
        char* pMessageRx = malloc(sizeof(*pMessageRx) * MAX_LEN);
        int bytesRx = recvfrom(socketDescriptor, pMessageRx, (MAX_LEN - 1), 0, (struct sockaddr *)&sinRemote, &sin_len);
        pMessageRx[bytesRx] = 0;

        bool isActive = createReply(sinRemote, sin_len, pMessageRx);    

        free(pMessageRx);
        pMessageRx = NULL;
        pthread_mutex_unlock(&UdpMutex);

        // check if the response returns a signal to terminate the program
        if(!isActive) {
            printf("Shutting down services...\n");
            AlarmClock_terminateThreads();
        }
    }

    return NULL;
}// receiveData()


static void sendReply(struct sockaddr_in sinRemote, unsigned int sin_len, char *messageTx) {
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}// sendReply()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum STATUS UDP_getStatus(void) {
    pthread_mutex_lock(&UdpMutex);
    enum STATUS stat = status;
    pthread_mutex_unlock(&UdpMutex);

    return stat;
}// UDP_getStatus()


void UDP_init(void) {
    createSocket();
    pthread_create(&UdpThreadID, NULL, &receiveData, NULL);

}// UDP_init()


void UDP_setStatus(enum STATUS value) {
    pthread_mutex_lock(&UdpMutex);
    status = value;
    pthread_mutex_unlock(&UdpMutex);
}// UDP_setStatus()


void UDP_shutdown(void) {
    pthread_join(UdpThreadID, NULL);
    closeSocket();
}// UDP_shutdown()

