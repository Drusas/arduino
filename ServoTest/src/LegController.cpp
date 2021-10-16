#include <Arduino.h>
#include "LegController.h"

LegController::LegController(float femurLength, float tibiaLength, float zOffset, float yOffset, 
                             int interval, IMotor *capsule, IMotor *shoulder, IMotor *knee, IServoController *controller) {
  setEnabled(false);
  servoController = controller;
  updateInterval = interval;
  hipxMotor = capsule;
  hipyMotor = shoulder;
  kneeMotor = knee;
  posIdx = bufferIdx = 0;
  ikModel = LegIKModel(femurLength, tibiaLength, zOffset, yOffset);
}

void LegController::addPoint(float x, float y, float z) {
  Point p = { x, y, z };
  addPoint(p);
}

void LegController::addPoint(Point p) {
  printf("LegController::addPoint 1\n");
  if (getEnabled()) {
    printf("LegController::addPoint 2\n");
    TRACE("%s { %0.2f, %0.2f, %0.2f }\n", "Point", p.x, p.y, p.z);
    if (positionBuffer.isNotFull()) {
      positionBuffer.addElement(p);
    }
    else {
      printf("LegController::addPoint 3\n");
      TRACE("%s\n", "Buffer full");
      throw std::exception();
    }
  }
}

void LegController::performUpdate() {
  if (DEBUG_LEGCONTROLLER > 0) {
    Serial.print(hipyMotor->atPosition()); Serial.print(" ,"); Serial.print(hipxMotor->atPosition()); Serial.print(" ,"); Serial.println(kneeMotor->atPosition());
  }
  printf("LegController::performUpdate() wait motors 2\n");
  if (areAllMotorsAtPosition()) { //  && isNextPositionInBufferValid()) {
    if (positionBuffer.isFull()) {
      printf("LegController::performUpdate() wait motors BUFFER FULL!\n");
      // throw exception;
      TRACE("%s\n", "buffer full");
      throw std::exception();
    }
    else if (positionBuffer.isNotEmpty()) {
      printf("LegController::performUpdate() wait motors 4\n");
      Point p = positionBuffer.getElement();
      moveToXYZ(p.x, p.y, p.z);
    }
    else {
      printf("LegController::performUpdate() wait motors buffer empty\n");
      TRACE("%s\n", "buffer empty");
      // empty no point to move to.
    }
  }
}

bool LegController::areAllMotorsAtPosition() {
  printf("LegController::areAllMotorsAtPosition() wait X joint: %d cmd: %d act: %d\n", hipxMotor->getServoIndex(), hipxMotor->cmdPosition(), hipxMotor->actPosition());
  printf("LegController::areAllMotorsAtPosition() wait Y joint: %d cmd: %d act: %d\n", hipyMotor->getServoIndex(), hipyMotor->cmdPosition(), hipyMotor->actPosition());
  printf("LegController::areAllMotorsAtPosition() wait K joint: %d cmd: %d act: %d\n", kneeMotor->getServoIndex(), kneeMotor->cmdPosition(), kneeMotor->actPosition());
  return hipyMotor->atPosition() && hipxMotor->atPosition() && kneeMotor->atPosition();
}

void LegController::followTrajectory(Point *buffer, uint8_t numPoints) {
  for (int i = 0; i < numPoints; i++) {
    do {
      moveToXYZ(buffer[i].x, buffer[i].y, buffer[i].z);
    } while (!hipxMotor->atPosition() || !hipyMotor->atPosition() || !kneeMotor->atPosition());
    
  }
}

void LegController::moveToXYZ(float x, float y, float z) {
  TRACE("%s %.2f %.2f %.2f\n", "moveToXYZ", x, y, z);
  printf("%s %.2f %.2f %.2f\n", "moveToXYZ", x, y, z);
  Point p;
  p.x = x;
  p.y = y;
  p.z = z;
  JointAngles j;
  uint8_t result = ikModel.getJointAnglesFromVectors(&p, 1, &j);
  if (result == LegIKModel::NO_ERR) {
    
    TRACE("%s %.2f %.2f %.2f\n", "moveToXYZ set joint angles", degrees(j.hx), degrees(j.hy), degrees(j.k));
    printf("%s %.2f %.2f %.2f\n", "moveToXYZ set joint angles", degrees(j.hx), degrees(j.hy), degrees(j.k));

    hipxMotor->setPosition(degrees(j.hx));
    hipyMotor->setPosition(degrees(j.hy));
    kneeMotor->setPosition(degrees(j.k));
  }
  else {
    TRACE("%s, %d\n", "Model error", result);
    TRACE("%s %.2f %.2f %.2f\n", "ERROR CALCULATING JOINT ANGLES:", degrees(j.hy), degrees(j.hx), degrees(j.k));
    printf("%s, %d\n", "Model error", result);
  }
}

void LegController::moveToAngles(uint8_t hx, uint8_t hy, uint8_t k) {
  hipxMotor->setPosition(hx);
  hipyMotor->setPosition(hy);
  kneeMotor->setPosition(k);
}

IMotor* LegController::getJoint(uint8_t idx) {
  Serial.print("LegController::getJoint: ");
  Serial.println(idx);
  IMotor* motor = 0;
  switch (idx)
  {
  case 0:
    motor = hipxMotor;
    break;
  case 1:
    motor = hipyMotor;
    break;
  case 2:
    motor = kneeMotor;
    break;
  default:
    break;
  }
  return motor;
}

void LegController::setHxTranslationAndOffset(float translate, float offset, int sign) {
  ikModel.setHxTranslationAndOffset(translate, offset, sign);
}

void LegController::setHyTranslationAndOffset(float translate, float offset, int sign) {
  ikModel.setHyTranslationAndOffset(translate, offset, sign);
}

void LegController::setKneeTranslationAndOffset(float translate, float offset, int sign) {
  ikModel.setKneeTranslationAndOffset(translate, offset, sign);
}

void LegController::setHxConstraints(float min, float max) {
  ikModel.setHxConstraints(min, max);
}

void LegController::setHyConstraints(float min, float max) {
  ikModel.setHyConstraints(min, max);
}

void LegController::setKneeConstraints(float min, float max) {
  ikModel.setKneeConstraints(min, max);
}