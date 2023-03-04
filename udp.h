/*
[DESCRIPTION]:
    - creates a receiving UDP socket on port 12345
    - listens for requests and passes input to responses module
    - uses the dynamic memory allocated by responses for reply message
        - splits the response message into valid 1500Byte packets
        - sends the response back to the user over UDP
*/

#ifndef UDP_H_
#define UDP_H_

#include "general.h"

enum STATUS UDP_getStatus(void);
void UDP_init(void);
void UDP_setStatus(enum STATUS value);
void UDP_shutdown(void);

#endif