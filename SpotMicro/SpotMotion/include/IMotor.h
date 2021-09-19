#ifndef _IMOTOR_H
#define _IMOTOR_H
#include <Arduino.h>

class Util {
public:
  static float toDegrees(float radians) {
    return radians / 2 / PI * 360;
  }

  static float toRadians(float degrees) {
    return degrees / 360 * 2 * PI;
  }
};

class IMotor {
public:
  virtual void SetPosition(int angle) = 0;
  virtual bool atPosition() = 0;
  virtual int cmdPosition() = 0;
  virtual int actPosition() = 0;
  virtual void home() = 0;
  virtual bool getHomed() = 0;
};

#endif