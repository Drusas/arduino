// Inverse kinematic leg model for SpotMicroAI - https://gitlab.com/public-open-source/spotmicroai
// Algorithm is from Adham Elarabawy's paper: www.adham-e.dev/papers

#ifndef _IKMODEL_H
#define _IKMODEL_H

#include <Arduino.h>
#include "Utils.h"

struct Point {
  float x;
  float y;
  float z;
};

struct JointAngles {
  float hx;
  float hy;
  float k;
};

 struct JointConstraints {
  volatile float translate;
  volatile float offset;
  volatile int sign;
};

class LegIKModel {
public:
  static const uint8_t NO_ERR = 0;
  static const uint8_t ERR_NULL_PTR = 1;
  static const uint8_t ERR_PT_OUTSIDE_MODEL = 2;
  static const uint8_t ERR_TARGET_R0 = 3;

  float femur, tibia, zOffset, yOffset;
  JointConstraints hxConstraints, hyConstraints, kneeConstraints;

protected:
  uint8_t checkModelConstraints(Point *target);
  uint8_t checkModelR0(float targetR0);
  uint8_t applyJointTranslationAndOffset(float *hx, float *hy, float *knee);
public:
  LegIKModel();
  LegIKModel(float femurLength, float tibiaLength, float zOff, float yOff);
  uint8_t getJointAnglesFromVectors(Point* vectors, uint8_t numVectors, JointAngles *joints);
  void setHxTranslationAndOffset(float translate, float offset, int sign);
  void setHyTranslationAndOffset(float translate, float offset, int sign);
  void setKneeTranslationAndOffset(float translate, float offset, int sign);
};
#endif
