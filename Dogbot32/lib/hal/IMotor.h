#ifndef _IMOTOR_H
#define _IMOTOR_H

#include "ITask.h"
#include "stdint.h"

// #include <Arduino.h>

class IMotor {
public:
  virtual void setPosition(int angle) = 0;
  virtual bool atPosition() = 0;
  virtual int cmdPosition() = 0;
  virtual int actPosition() = 0;
  virtual void home() = 0;
  virtual bool getHomed() = 0;
  virtual void setSpeed(uint8_t speed) = 0;
  virtual int getServoIndex() = 0;
  virtual void setEnabled(bool state);
  virtual bool getEnabled();
};

#endif