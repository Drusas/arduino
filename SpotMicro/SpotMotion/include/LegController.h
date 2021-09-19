#ifndef _LEGCONTROLLER_H
#define _LEGCONTROLLER_H

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <Wire.h>
#include "IKModel.h"
#include "IMotor.h"
#include "ITask.h"
#include "IServoController.h"

#define DEBUG_LEGCONTROLLER 0

struct LegPosition {
  uint8_t capsule;
  uint8_t shoulder;
  uint8_t knee;
};

class LegController : public ITask
{
  static const uint8_t NUM_POSITIONS = 10;
  IServoController *servoController;
  IMotor *capsuleController;
  IMotor *shoulderController;
  IMotor *kneeController;
  LegPosition positionBuffer[NUM_POSITIONS];
  uint8_t posIdx, bufferIdx;
  
  void incrementPosition();

protected:
  void performUpdate();
    
 public:
  LegController(int interval, IMotor *capsule, IMotor *shoulder, IMotor *knee, IServoController *controller);
  void addPosition(uint8_t c, uint8_t s, uint8_t k);
  void generateTrajectory(Bone* joints);
};

#endif
