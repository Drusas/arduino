#include "IKModel.h"
#include "Utils.h"

// #define DEBUG_IKMODEL

// todo - need a setContstraints method for;
// minimum r0...

// todo - error check (inputs, cos/sin/theta, etc.), return code
// todo - bounds check points in valid region (can we reach the point?)
// todo - add zOffset to z input?
// todo - pass values for transforming coordinate spaces 
uint8_t LegIKModel::getJointAnglesFromVectors(Point* vectors, uint8_t numVectors, JointAngles *joints) {
  if ((vectors == 0) || (joints == 0)) {
    return ERR_NULL_PTR;
  }
  
  uint8_t result = NO_ERR;
  for (uint8_t i = 0; i < numVectors; i++) {
    Point v  = vectors[i];
  #ifdef DEBUG_IKMODEL
    TRACE("%s, %.2f, %.2f, %.2f\n", "Point: ", v.x, v.y, v.z);
  #endif
    // todo need an array of error numbers
    result = checkModelConstraints(&v);
    if (result != NO_ERR) {
      break;
    }
    // calculate Y-Z plane
    float h1 = sqrt(zOffset*zOffset + yOffset*yOffset);
    float h2 = sqrt(v.z*v.z + v.y*v.y);
    float alpha0 = atan(v.y / v.z);
    float alpha1 = atan(yOffset / zOffset);
    float alpha2 = atan(zOffset / yOffset);
    float alpha3 = asin(h1 * sin(alpha2 + radians(90)) / h2);
    float alpha4 = radians(180) - (alpha3 + alpha2 + radians(90));
    float alpha5 = alpha1 - alpha4;
    float hxTheta = alpha0 - alpha5;
    float r0 = h1 * sin(alpha4) / sin(alpha3);
    result = checkModelR0(r0);
    if (result != NO_ERR) {
      break;
    }

    // calculate X-Z plane
    float h = sqrt(r0 * r0 + v.x * v.x);
    float phi = asin(v.x / h);
    float hxCos = (h*h + femur*femur - tibia*tibia) / (2 * h * femur);
    float hyTheta = acos(hxCos) - phi;
    float kTheta = acos((tibia*tibia + femur*femur - h*h) / (2*tibia*femur));
#ifdef DEBUG_IKMODEL
    TRACE("h1,%0.2f ,h2,%0.2f, a0,%0.2f, a1,%0.2f, a2,%0.2f, a3,%0.2f, a4,%0.2f, a5,%0.2f, r0,%0.2f, h,%0.2f, phi,%0.2f, hxCos,%0.2f\n", h1, h2, alpha0, alpha1, (alpha2), (alpha3), (alpha4), (alpha5), r0, h, phi, hxCos);
    TRACE("%s %0.2f,%0.2f,%0.2f\n", "Uncorrected angles: ", degrees(hxTheta), degrees(hyTheta), degrees(kTheta));
#endif
    result = applyJointTranslationAndOffset(&hxTheta, &hyTheta, &kTheta);
    if (result != NO_ERR) {
      break;
    }
    result = checkThetaConstraints(hxTheta, hyTheta, kTheta);
    if (result != NO_ERR) {
      break;
    }

    joints[i].hx = hxTheta;
    joints[i].hy = hyTheta;
    joints[i].k = kTheta;
#ifdef DEBUG_IKMODEL
    TRACE("%s %0.2f,%0.2f,%0.2f\n", "Translated angles: ", degrees(hxTheta), degrees(hyTheta), degrees(kTheta));
#endif
    if ((joints[i].hx == NAN) ||
        (joints[i].hy == NAN) ||
        (joints[i].k == NAN)) {
          result = ERR_UNKNOWN;
        }
  }
  return result;
}

uint8_t LegIKModel::checkModelConstraints(Point *target) {
  uint8_t result = NO_ERR;
  if (target == 0)
  {
    result = ERR_NULL_PTR;
  }

  float modelRadius = femur + tibia;
  float targetRadius = sqrt(target->x * target->x + target->z * target->z);
  if (targetRadius >= modelRadius) {
    result = ERR_PT_OUTSIDE_MODEL;
  }

  return result;
}

uint8_t LegIKModel::checkModelR0(float targetR0) {
  uint8_t result = NO_ERR;
  float modelRadius = femur + tibia;
  if ((targetR0 <= 0) || (targetR0 > modelRadius)) {
    result = ERR_TARGET_R0;
  }

  return result;
}

uint8_t LegIKModel::checkThetaConstraints(float hx, float hy, float knee) {
  uint8_t result = NO_ERR;

  hx = round_up(hx, 4);
  hy = round_up(hy, 4);
  knee = round_up(knee, 4);

  if ((hx < hxConstraints.min) || (hx > hxConstraints.max)) {
    result = ERR_HX_OUT_OF_RANGE;
  }
  else if ((hy < hyConstraints.min) || (hy > hyConstraints.max)) {
    result = ERR_HY_OUT_OF_RANGE;
  }
  else if ((knee < kneeConstraints.min) || (knee > kneeConstraints.max)) {
    result = ERR_KNEE_OUT_OF_RANGE;
  }

  return result;
}

LegIKModel::LegIKModel() :
femur(0),
tibia(0),
zOffset(0),
yOffset(0)
{
}

LegIKModel::LegIKModel(float femurLength, float tibiaLength, float zOff, float yOff) {
  femur = femurLength;
  tibia = tibiaLength;
  zOffset = zOff;
  yOffset = yOff;
  setHxTranslationAndOffset(0, 0, 1);
  setHyTranslationAndOffset(0, 0, 1);
  setKneeTranslationAndOffset(0, 0, 1);
  setHxConstraints(0, radians(180));
  setHyConstraints(0, radians(180));
  setKneeConstraints(0, radians(180));
}

uint8_t LegIKModel::applyJointTranslationAndOffset(float *hx, float *hy, float *knee) {
  uint8_t result = NO_ERR;
  if (hx == 0) {
    result = ERR_NULL_PTR;
  }
  else if (hy == 0) {
    result = ERR_NULL_PTR;
  }
  else if (knee == 0) {
    result = ERR_NULL_PTR;
  }
  if (result != NO_ERR) {
    return result;
  }
#ifdef DEBUG_IKMODEL
  TRACE("%s %0.2f,%0.2f,%d\n", "hx offsets: ", degrees(round_up(hxOffsets.translate, 2)), degrees(round_up(hxOffsets.offset, 2)), hxOffsets.sign);
  TRACE("%s %0.2f,%0.2f,%d\n", "hy offsets: ", degrees(round_up(hyOffsets.translate, 2)), degrees(round_up(hyOffsets.offset, 2)), hyOffsets.sign);
  TRACE("%s %0.2f,%0.2f,%d\n", "knee offsets: ", degrees(kneeOffsets.translate), degrees(kneeOffsets.offset), kneeOffsets.sign);
#endif
  float x = *hx;
  float y = *hy;
  float k = *knee;
  if (hxOffsets.translate > 0) {
    x = hxOffsets.translate + (hxOffsets.sign * x);
  }
  if (hyOffsets.translate > 0) {
    y = hyOffsets.translate + (hyOffsets.sign * y);
  }
  if (kneeOffsets.translate > 0) {
    k = kneeOffsets.translate + (kneeOffsets.sign * k);
  }

  x += hxOffsets.offset;
  y += hyOffsets.offset;
  k += kneeOffsets.offset;

  *hx = x;
  *hy = y;
  *knee = k;
  return result;
}

void LegIKModel::setHxTranslationAndOffset(float translate, float offset, int sign) {
  hxOffsets.translate = translate;
  hxOffsets.offset = offset;
  hxOffsets.sign = sign;
}

void LegIKModel::setHyTranslationAndOffset(float translate, float offset, int sign) {
  hyOffsets.translate = translate;
  hyOffsets.offset = offset;
  hyOffsets.sign = sign;
}

void LegIKModel::setKneeTranslationAndOffset(float translate, float offset, int sign) {
  kneeOffsets.translate = translate;
  kneeOffsets.offset = offset;
  kneeOffsets.sign = sign;
}

void LegIKModel::setHxConstraints(float min, float max) {
  hxConstraints.min = min;
  hxConstraints.max = max;
}

void LegIKModel::setHyConstraints(float min, float max) {
  hyConstraints.min = min;
  hyConstraints.max = max;
}

void LegIKModel::setKneeConstraints(float min, float max) {
  kneeConstraints.min = min;
  kneeConstraints.max = max;
}