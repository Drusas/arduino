#ifndef _SERVOCONTROLLER_H
#define _SERVOCONTROLLER_H

#include <Arduino.h>
// #include "Motion.h"

class Sweeper;

class ServoController {
  bool enabled;
  Sweeper **motors;
  uint8_t numMotors, motorManagementIdx;

public:
  ServoController(uint8_t maxNumberMotorsToManage); 
  void setEnabled(bool state);
  bool getEnabled();
  void addMotor(Sweeper *motor);
  bool getHomed();
};

#endif