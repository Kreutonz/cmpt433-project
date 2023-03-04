/* 
[DESCRIPTION]: 
    - handles setting of pins for joystick input
    - pressing inwards will cycle through the beats
    - pressing left decreases tempo
    - pressing right increases tempo
    - pressing up increases volume
    - pressing down decreases volume
*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

enum STATUS Joystick_getStatus(void);
void Joystick_init(void);
void Joystick_setStatus(enum STATUS status);
void Joystick_shutdown(void);

#endif