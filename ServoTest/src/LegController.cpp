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
  jointTranslation = 0;
}

void LegController::addPosition(uint8_t c, uint8_t s, uint8_t k) {
  positionBuffer[bufferIdx].hy = s;
  positionBuffer[bufferIdx].hx = c;
  positionBuffer[bufferIdx].k = k;
  bufferIdx = ++bufferIdx % NUM_POSITIONS;
}

void LegController::generateTrajectory() {
  Point* points = CurveGenerator::GenerateCircle(25, 200, 50, NUM_POSITIONS);
  ikModel.getJointAnglesFromVectors(points, NUM_POSITIONS, positionBuffer);
  delete points;
}

void LegController::incrementPosition() {
    posIdx = ++posIdx % NUM_POSITIONS;
  }

void LegController::performUpdate() {
  if (DEBUG_LEGCONTROLLER > 0) {
    Serial.print(hipyMotor->atPosition()); Serial.print(" ,"); Serial.print(hipxMotor->atPosition()); Serial.print(" ,"); Serial.println(kneeMotor->atPosition());
  }
  if (hipyMotor->atPosition() && hipxMotor->atPosition() && kneeMotor->atPosition()) {
    hipyMotor->setPosition(degrees(positionBuffer[posIdx].hy));
    hipxMotor->setPosition(degrees(positionBuffer[posIdx].hx));
    kneeMotor->setPosition(degrees(positionBuffer[posIdx].k));
    if (DEBUG_LEGCONTROLLER > 0) {
      Serial.print(hipyMotor->cmdPosition()); Serial.print(" ,"); Serial.print(hipxMotor->cmdPosition()); Serial.print(" ,"); Serial.println(kneeMotor->cmdPosition());
    }
    incrementPosition();
  }
}

void LegController::moveToXYZ(float x, float y, float z) {
  TRACE("%s %.2f %.2f %.2f\n", "moveToXYZ", x, y, z);
  Point p;
  p.x = x;
  p.y = y;
  p.z = z;
  JointAngles j;
  uint8_t result = ikModel.getJointAnglesFromVectors(&p, 1, &j);
  if (result == 0) {
    TRACE("%s %.2f %.2f %.2f\n", "moveToXYZ set joint angles", degrees(j.hy), degrees(j.hx), degrees(j.k));
    translateJoints(&j, 1);
    TRACE("%s %.2f %.2f %.2f\n", "moveToXYZ TRANSLATED joint angles", degrees(j.hy), degrees(j.hx), degrees(j.k));
    hipxMotor->setPosition(degrees(j.hy));
    hipyMotor->setPosition(degrees(j.hx));
    kneeMotor->setPosition(degrees(j.k));
  }
  else {
    TRACE("%s %.2f %.2f %.2f\n", "ERROR CALCULATING JOINT ANGLES:", degrees(j.hy), degrees(j.hx), degrees(j.k));
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

void LegController::jointTranslationFactor(uint8_t angle) {
  //jointTranslation = radians(angle);
  jointTranslation = angle;
}

void LegController::translateJoints(JointAngles *joints, uint8_t numJoints) {
  if (jointTranslation > 0) {
    for (uint8_t i = 0; i < numJoints; i++) {
      joints[i].hx = radians(jointTranslation - degrees(joints[i].hx));
      joints[i].hy = radians(jointTranslation - degrees(joints[i].hy));
      joints[i].k = radians(jointTranslation - degrees(joints[i].k));
    }
  }
}