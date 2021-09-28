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
  static const uint8_t NUM_POSITIONS = 4;
  IServoController *servoController;
  IMotor *hipxMotor; // rotates about the x-axis (y-z plane)
  IMotor *hipyMotor; // rotates about the y-axis (x-z plane)
  IMotor *kneeMotor;
  Point positionBuffer[NUM_POSITIONS];
  uint8_t posIdx, bufferIdx;
  LegIKModel ikModel;
  
  void incrementPosition();

protected:
  void performUpdate();
    
 public:
  LegController(float femurLength, float tibiaLength, float zOffset, float yOffset, int interval, IMotor *hipy, IMotor *hipx, IMotor *knee, IServoController *controller);
  //void addPosition(uint8_t c, uint8_t s, uint8_t k);
  void addPoint(Point p);
  void moveToXYZ(float x, float y, float z);
  void moveToAngles(uint8_t hx, uint8_t hy, uint8_t k);
  IMotor* getJoint(uint8_t idx);
  void setHxTranslationAndOffset(float translate, float offset, int sign);
  void setHyTranslationAndOffset(float translate, float offset, int sign);
  void setKneeTranslationAndOffset(float translate, float offset, int sign);
  void followTrajectory(Point *buffer, uint8_t numPoints);
};

#endif
