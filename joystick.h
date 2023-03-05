/* 
[DESCRIPTION]: 
    - handles setting of pins for joystick input
    - pressing inwards will shut off the alarm if active
    - pressing left/right displays hours
    - pressing up/down displays minutes
    - if alarm is active (playing sound), pressing the joystick in any direction (not pushed) will snooze
*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

enum STATUS Joystick_getStatus(void);
void Joystick_init(void);
void Joystick_setStatus(enum STATUS status);
void Joystick_shutdown(void);

#endif