#ifndef _SERVOCONTROLLER_H
#define _SERVOCONTROLLER_H

#include <Arduino.h>
#include "IServoController.h"
// #include "IMotor.h"

class ServoController : public IServoController {
  bool enabled;
  IMotor **motors;
  uint8_t numMotors, motorManagementIdx;

public:
  ServoController(uint8_t maxNumberMotorsToManage); 
  void setEnabled(bool state);
  bool getEnabled();
  void addMotor(IMotor *motor);
  bool getHomed();
  void homeMotors();
};

#endif