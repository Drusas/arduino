// Inverse kinematic leg model for SpotMicroAI - https://gitlab.com/public-open-source/spotmicroai
// Algorithm is from Adham Elarabawy's paper: www.adham-e.dev/papers

#ifndef _IKMODEL_H
#define _IKMODEL_H

#include <Arduino.h>

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
  float femur, tibia, zOffset, yOffset;

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

  // todo - error check (inputs, cos/sin/theta, etc.), return code
  // todo - bounds check points in valid region (can we reach the point?)
  // todo - add zOffset to z input?
  // todo - pass values for transforming coordinate spaces 
  void getJointAnglesFromVectors(Point* vectors, uint8_t numVectors, JointAngles *joints) {
    for (uint8_t i = 0; i < numVectors; i++) {
      Point v  = vectors[i];
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
      // calculate X-Z plane
      float h = sqrt(r0 * r0 + v.x * v.x);
      float phi = asin(v.x / h);
      float hxCos = (h*h + femur*femur - tibia*tibia) / (2 * h * femur);
      float hxTheta = acos(hxCos) - phi;
      float kTheta = acos((tibia*tibia + femur*femur - h*h) / (2*tibia*femur));
      joints[i].hx = radians(90) + hxTheta; // TRANSLATE TO LOCAL COORDS
      joints[i].hy = radians(90) - hyTheta;
      joints[i].k = kTheta;
    }
  }
};

class CurveGenerator {

public:
    static Point* GenerateCircle(float originX, float originY, float radius, uint8_t numPoints) {
        Point* points = new Point[numPoints];
        float angle = (2 * PI) / numPoints;
        for (int i; i < numPoints; i++) {
            points[i].x = radius * cos(angle * i) + originX;
            points[i].y = 60;
            points[i].z = radius * sin(angle * i) + originY;
        }

        return points;
    }

    static Point* GenerateEllipse(float originX, float originY, float r1, float r2, uint8_t numPoints) {
        Point* points = new Point[numPoints];
        float angle = (2 * PI) / numPoints;
        for (int i; i < numPoints; i++) {
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