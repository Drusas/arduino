#ifndef _IKMODEL_H
#define _IKMODE_H

#include <Arduino.h>

struct Point {
  float x;
  float y;
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

  float getAngle() {
    return this->angle;
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
      child->rotate(rotationAngle);
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
      return getEndPointWorld();
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