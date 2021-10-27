#ifndef _SERVOCONTROLLER_H
#define _SERVOCONTROLLER_H

#include <Arduino.h>
#include "IServoController.h"

class ServoController : public IServoController {
  
public:
    const static uint8_t MAX_NUM_MOTORS = 12;
    ServoController(); 
    void setEnabled(bool state);
    bool getEnabled();
    void addMotor(IMotor *motor);
    bool getHomed();
    void homeMotors();

private:
    bool enabled;
    IMotor *motors[MAX_NUM_MOTORS];
    uint8_t motorManagementIdx;
};

#endif