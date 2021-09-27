#include "IKModel.h"
#include "Utils.h"

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
    TRACE("%s, %.2f, %.2f, %.2f\n", "Point: ", v.x, v.y, v.z);
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

    TRACE("h1,%0.2f ,h2,%0.2f, a0,%0.2f, a1,%0.2f, a2,%0.2f, a3,%0.2f, a4,%0.2f, a5,%0.2f, r0,%0.2f, h,%0.2f, phi,%0.2f, hxCos,%0.2f\n", h1, h2, alpha0, alpha1, (alpha2), (alpha3), (alpha4), (alpha5), r0, h, phi, hxCos);
    TRACE("%s %0.2f,%0.2f,%0.2f\n", "Uncorrected angles: ", degrees(hxTheta), degrees(hyTheta), degrees(kTheta));

    result = applyJointTranslationAndOffset(&hxTheta, &hyTheta, &kTheta);

    joints[i].hx = hxTheta;
    joints[i].hy = hyTheta;
    joints[i].k = kTheta;

    TRACE("%s %0.2f,%0.2f,%0.2f\n", "Translated angles: ", degrees(hxTheta), degrees(hyTheta), degrees(kTheta));

    if ((joints[i].hx == NAN) ||
        (joints[i].hy == NAN) ||
        (joints[i].k == NAN)) {
          result = 1;
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

  TRACE("%s %0.2f,%0.2f,%d\n", "hx offsets: ", degrees(round_up(hxConstraints.translate, 2)), degrees(round_up(hxConstraints.offset, 2)), hxConstraints.sign);
  TRACE("%s %0.2f,%0.2f,%d\n", "hy offsets: ", degrees(round_up(hyConstraints.translate, 2)), degrees(round_up(hyConstraints.offset, 2)), hyConstraints.sign);
  TRACE("%s %0.2f,%0.2f,%d\n", "knee offsets: ", degrees(kneeConstraints.translate), degrees(kneeConstraints.offset), kneeConstraints.sign);

  float x = *hx;
  float y = *hy;
  float k = *knee;
  if (hxConstraints.translate > 0) {
    x = hxConstraints.translate + (hxConstraints.sign * x);
  }
  if (hyConstraints.translate > 0) {
    y = hyConstraints.translate + (hyConstraints.sign * y);
  }
  if (kneeConstraints.translate > 0) {
    k = kneeConstraints.translate + (kneeConstraints.sign * k);
  }

  x += hxConstraints.offset;
  y += hyConstraints.offset;
  k += kneeConstraints.offset;

  *hx = x;
  *hy = y;
  *knee = k;
  return result;
}

void LegIKModel::setHxTranslationAndOffset(float translate, float offset, int sign) {
  hxConstraints.translate = translate;
  hxConstraints.offset = offset;
  hxConstraints.sign = sign;
}

void LegIKModel::setHyTranslationAndOffset(float translate, float offset, int sign) {
  hyConstraints.translate = translate;
  hyConstraints.offset = offset;
  hyConstraints.sign = sign;
}

void LegIKModel::setKneeTranslationAndOffset(float translate, float offset, int sign) {
  kneeConstraints.translate = translate;
  kneeConstraints.offset = offset;
  kneeConstraints.sign = sign;
}