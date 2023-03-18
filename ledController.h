#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

enum ALARM_STAUS{ON, OFF};

enum ALARM_STAUS LedController_getAlarmStatus(void);
void LedController_init(void);
void LedController_setAlarmStatus(enum ALARM_STAUS newStatus);
void LedController_shutDown(void);

#endif 