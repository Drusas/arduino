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

class LegIKModel {
public:
  static const uint8_t NO_ERR = 0;
  static const uint8_t ERR_NULL_PTR = 1;
  static const uint8_t ERR_PT_OUTSIDE_MODEL = 2;
  static const uint8_t ERR_TARGET_R0 = 3;

  float femur, tibia, zOffset, yOffset;

protected:
  uint8_t checkModelConstraints(Point *target) {
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

  uint8_t checkModelR0(float targetR0) {
    uint8_t result = NO_ERR;
    float modelRadius = femur + tibia;
    if ((targetR0 <= 0) || (targetR0 > modelRadius)) {
      result = ERR_TARGET_R0;
    }

    return result;
  }

public:
  LegIKModel() : 
  femur(0),
  tibia(0),
  zOffset(0),
  yOffset(0)
  {}

  LegIKModel(float femurLength, float tibiaLength, float zOff, float yOff) {
    femur = femurLength;
    tibia = tibiaLength;
    zOffset = zOff;
    yOffset = yOff;
  }

  // todo - need a setContstraints method for;
  // minimum r0...

  // todo - error check (inputs, cos/sin/theta, etc.), return code
  // todo - bounds check points in valid region (can we reach the point?)
  // todo - add zOffset to z input?
  // todo - pass values for transforming coordinate spaces 
  uint8_t getJointAnglesFromVectors(Point* vectors, uint8_t numVectors, JointAngles *joints) {
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
      float hyTheta = alpha0 - alpha5;
      float r0 = h1 * sin(alpha4) / sin(alpha3);
      result = checkModelR0(r0);
      if (result != NO_ERR) {
        break;
      }

      // calculate X-Z plane
      float h = sqrt(r0 * r0 + v.x * v.x);
      float phi = asin(v.x / h);
      float hxCos = (h*h + femur*femur - tibia*tibia) / (2 * h * femur);
      float hxTheta = acos(hxCos) - phi;
      float kTheta = acos((tibia*tibia + femur*femur - h*h) / (2*tibia*femur));

      joints[i].hx = radians(90) + hxTheta; // TRANSLATE TO LOCAL COORDS
      joints[i].hy = radians(90) - hyTheta;
      joints[i].k = kTheta;

      if ((joints[i].hx == NAN) ||
          (joints[i].hy == NAN) ||
          (joints[i].k == NAN)) {
            result = 1;
          }

      TRACE("h1,%0.2f ,h2,%0.2f, a0,%0.2f, a1,%0.2f, a2,%0.2f, a3,%0.2f, a4,%0.2f, a5,%0.2f, r0,%0.2f, h,%0.2f, phi,%0.2f, hxCos,%0.2f\n", h1, h2, alpha0, alpha1, (alpha2), (alpha3), (alpha4), (alpha5), r0, h, phi, hxCos);
      TRACE("%s,%0.2f,%0.2f,%0.2f\n", "Uncorrected angles", degrees(hxTheta), degrees(hyTheta), degrees(kTheta));
      TRACE("%s,%0.2f,%0.2f,%0.2f\n", "Corrected angles", degrees(joints[i].hx), degrees(joints[i].hy), degrees(joints[i].k));
    }
    return result;
  }

  
};

class CurveGenerator {

public:
    static Point* GenerateCircle(float originX, float originY, float radius, uint8_t numPoints) {
        Point* points = new Point[numPoints];
        float angle = (2 * PI) / numPoints;
        for (uint8_t i = 0; i < numPoints; i++) {
            points[i].x = radius * cos(angle * i) + originX;
            points[i].y = 60;
            points[i].z = radius * sin(angle * i) + originY;
        }

        return points;
    }

    static Point* GenerateEllipse(float originX, float originY, float r1, float r2, uint8_t numPoints) {
        Point* points = new Point[numPoints];
        float angle = (2 * PI) / numPoints;
        for (uint8_t i; i < numPoints; i++) {
            points[i].x = r1 * cos(angle * i) + originX;
            points[i].y = r2 * sin(angle * i) + originY;
        }

        return points;
    }
};

class Bone {

    float originX, originY, angle, length;
    Bone *child;

public:
  Bone(float originX, float originY, float angle, float length, Bone* child) {

    this->originX = originX;
    this->originY = originY;
    this->angle = angle;
    this->length = length;
    this->child = child;
  }

  Bone* getChild() {
    return this->child;
  }

  float getAngle() {
    return simplifyAngle(this->angle);
  }

  void setOrigin(Point pt) {
    originX = pt.x;
    originY = pt.y;
  }

  Point* getEndPoint() {
    Point* endPts = new Point[2];
    Point myEndPt = getEndPointWorld();
    endPts[0].x = originX;
    endPts[0].y = originY;
    endPts[1].x = myEndPt.x;
    endPts[1].y = myEndPt.y;
    return endPts;
  }

  Point getEndPointWorld() {
    Point endPt;
    endPt.x = length * cos(angle) + originX;
    endPt.y = length * sin(angle) + originY;
    return endPt;
  }

  void rotatePoint(Point* pt, float rotationAngle) {
    pt->x = pt->x * cos(rotationAngle) - pt->y * sin(rotationAngle);
    pt->y = pt->y * cos(rotationAngle) + pt->x * sin(rotationAngle);
  }

  void translatePoint(Point* pt, float offsetX, float offsetY) {
    pt->x = pt->x + offsetX;
    pt->y = pt->y + offsetY;
  }

  float calcualteAngleToPoint(Point pt) {
    return atan2(pt.y, pt.x);
  }

  void rotate(float rotationAngle) {
    angle += rotationAngle;
    if (child) {
      Point pt = getEndPointWorld();
      child->setOrigin(pt);
      // child->rotate(rotationAngle);
      child->setAngle(child->getAngle() - this->angle);
    }
  }

  void setAngle(float angle) {
    this->angle = angle;
    if (child) {
      Point pt = getEndPointWorld();
      child->setOrigin(pt);
    }
  }

  float simplifyAngle(float someAngle) {
    float a = fmod(someAngle, (PI * 2));
    if (a < -PI) {
      a += (PI * 2);
    }
    else if (a > PI) {
      a -= (PI * 2);
    }
    return a;
  }
  
  Point getEndOfChain() {
    if (child != 0) {
      return child->getEndOfChain();
    }
    else {
      return getEndPointWorld();
    }
  }

  Point updateIK(Point targetPoint) {
    if (child) {
      child->updateIK(targetPoint);
    }

    if (child == 0) {
      Point worldTarget = targetPoint;
      translatePoint(&worldTarget, -originX, -originY);
      float targetDistance = sqrt(worldTarget.x * worldTarget.x + worldTarget.y * worldTarget.y);
      float targetCosine = worldTarget.x / targetDistance;
      float targetSine = worldTarget.y / targetDistance;
      float targetCosAngle = acos(targetCosine);
      float targetSinAngle = asin(targetSine);
      
      if (targetSinAngle < 0) {
        targetCosAngle = -targetCosAngle;
      }

      float targetRotationAngle = simplifyAngle(targetCosAngle);
      float shiftAngle = targetRotationAngle - angle;
      angle += shiftAngle;
      Point myEndPt = getEndPointWorld();
      return myEndPt;
    } 
    else {
      // Get angle of end of the chain
      Point chainEndPt = child->getEndOfChain();
      translatePoint(&chainEndPt, -originX, -originY);
      float chainDistance = sqrt(chainEndPt.x * chainEndPt.x + chainEndPt.y * chainEndPt.y);
      float chainCosine = chainEndPt.x / chainDistance;
      float chainSine = chainEndPt.y / chainDistance;
      float chainCosAngle = acos(chainCosine);
      float chainSinAngle = asin(chainSine);
      
      if (chainSinAngle < 0) {
        chainCosAngle = -chainCosAngle;
      }
      float chainRotationAngle = simplifyAngle(chainCosAngle);

      // Get the angle of the target
      Point worldTarget = targetPoint;
      translatePoint(&worldTarget, -originX, -originY);
      float targetDistance = sqrt(worldTarget.x * worldTarget.x + worldTarget.y * worldTarget.y);
      float targetCosine = worldTarget.x / targetDistance;
      float targetSine = worldTarget.y / targetDistance;
      float targetCosAngle = acos(targetCosine);
      float targetSinAngle = asin(targetSine);
      
      if (targetSinAngle < 0) {
        targetCosAngle = -targetCosAngle;
      }

      float targetRotationAngle = simplifyAngle(targetCosAngle);

      float shiftAngle = targetRotationAngle - chainRotationAngle;
      
      // update the whole chain
      rotate(shiftAngle);
      return getEndPointWorld();
    }
  }


  
};

#endif