#ifndef _ISERVOCONTROLLER_H
#define _ISERVOCONTROLLER_H
#include <Arduino.h>
#include "IMotor.h"

class IServoController {
public:
  virtual void setEnabled(bool state) = 0;
  virtual bool getEnabled() = 0;
  virtual void addMotor(IMotor *motor) = 0;
  virtual bool getHomed() = 0;
  virtual void homeMotors() = 0;
};

#endif