#ifndef MOTION_SENSOR_H_
#define MOTION_SENSOR_H_

enum MOTION_SENSOR{MOVEMENT, NO_MOVEMENT};

enum MOTION_SENSOR MotionSensor_getMotionStatus(void);
enum STATUS MotionSensor_getStatus(void);
void MotionSensor_init(void);
void MotionSensor_setMotionStatus(enum MOTION_SENSOR newStatus);
void MotionSensor_setStatus(enum STATUS value);
void MotionSensor_shutDown(void);

#endif