#include <Arduino.h>
#include "LegController.h"

LegController::LegController(float femurLength, float tibiaLength, float zOffset, float yOffset, 
                             int interval, IMotor *capsule, IMotor *shoulder, IMotor *knee, IServoController *controller) {
  setEnabled(false);
  servoController = controller;
  updateInterval = interval;
  hipyMotor = capsule;
  hipxMotor = shoulder;
  kneeMotor = knee;
  posIdx = bufferIdx = 0;
  ikModel = LegIKModel(femurLength, tibiaLength, zOffset, yOffset);
}

void LegController::addPosition(uint8_t c, uint8_t s, uint8_t k) {
  positionBuffer[bufferIdx].hy = c;
  positionBuffer[bufferIdx].hx = s;
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
    hipyMotor->SetPosition(degrees(positionBuffer[posIdx].hy));
    hipxMotor->SetPosition(degrees(positionBuffer[posIdx].hx));
    kneeMotor->SetPosition(degrees(positionBuffer[posIdx].k));
    if (DEBUG_LEGCONTROLLER > 0) {
      Serial.print(hipyMotor->cmdPosition()); Serial.print(" ,"); Serial.print(hipxMotor->cmdPosition()); Serial.print(" ,"); Serial.println(kneeMotor->cmdPosition());
    }
    incrementPosition();
  }
}

void LegController::moveToXYZ(float x, float y, float z) {
  Point p;
  p.x = x;
  p.y = y;
  p.z = z;
  JointAngles j;
  ikModel.getJointAnglesFromVectors(&p, 1, &j);
  hipxMotor->SetPosition(degrees(j.hx));
  hipyMotor->SetPosition(degrees(j.hy));
  kneeMotor->SetPosition(degrees(j.k));
}

