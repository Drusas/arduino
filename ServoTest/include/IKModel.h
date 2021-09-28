// Inverse kinematic leg model for SpotMicroAI - https://gitlab.com/public-open-source/spotmicroai
// Algorithm is from Adham Elarabawy's paper: www.adham-e.dev/papers

#ifndef _IKMODEL_H
#define _IKMODEL_H

#include <Arduino.h>
#include "Utils.h"


struct JointAngles {
  float hx;
  float hy;
  float k;
};

 struct JointOffsets {
  volatile float translate;
  volatile float offset;
  volatile int sign;
};

struct JointConstraints {
  float min;
  float max;
};

class LegIKModel {
public:
  static const uint8_t NO_ERR = 0;
  static const uint8_t ERR_NULL_PTR = 1;
  static const uint8_t ERR_PT_OUTSIDE_MODEL = 2;
  static const uint8_t ERR_TARGET_R0 = 3;
  static const uint8_t ERR_HX_OUT_OF_RANGE = 4;
  static const uint8_t ERR_HY_OUT_OF_RANGE = 5;
  static const uint8_t ERR_KNEE_OUT_OF_RANGE = 6;
  static const uint8_t ERR_UNKNOWN = 7;

  float femur, tibia, zOffset, yOffset;
  JointOffsets hxOffsets, hyOffsets, kneeOffsets;
  JointConstraints hxConstraints, hyConstraints, kneeConstraints;

protected:
  uint8_t checkModelConstraints(Point *target);
  uint8_t checkModelR0(float targetR0);
  uint8_t checkThetaConstraints(float hx, float hy, float knee);
  uint8_t applyJointTranslationAndOffset(float *hx, float *hy, float *knee);
public:
  LegIKModel();
  LegIKModel(float femurLength, float tibiaLength, float zOff, float yOff);
  uint8_t getJointAnglesFromVectors(Point* vectors, uint8_t numVectors, JointAngles *joints);
  void setHxTranslationAndOffset(float translate, float offset, int sign);
  void setHyTranslationAndOffset(float translate, float offset, int sign);
  void setKneeTranslationAndOffset(float translate, float offset, int sign);
  void setHxConstraints(float min, float max);
  void setHyConstraints(float min, float max);
  void setKneeConstraints(float min, float max);
};
#endif
