#include <Arduino.h>
#include "LegController.h"

LegController::LegController(int interval, IMotor *capsule, IMotor *shoulder, IMotor *knee, IServoController *controller) {
  setEnabled(false);
  servoController = controller;
  updateInterval = interval;
  capsuleController = capsule;
  shoulderController = shoulder;
  kneeController = knee;
  posIdx = bufferIdx = 0;
}

void LegController::addPosition(uint8_t c, uint8_t s, uint8_t k) {
  positionBuffer[bufferIdx].capsule = c;
  positionBuffer[bufferIdx].shoulder = s;
  positionBuffer[bufferIdx].knee = k;
  bufferIdx = ++bufferIdx % NUM_POSITIONS;
}

void LegController::generateTrajectory(Bone* joints) {
  Point* points = CurveGenerator::GenerateCircle(-100, 250, 75, 10);
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 15; j++) {
      joints->updateIK(points[i]);
    }
    float shoulder = Util::toDegrees(joints->getAngle());
    float knee = 180 - Util::toDegrees(joints->getChild()->getAngle());
    uint8_t s = (uint8_t)shoulder;
    uint8_t k = (uint8_t)knee;
    positionBuffer[i].shoulder = s;
    positionBuffer[i].knee = k;
  }
  delete points;
}

void LegController::incrementPosition() {
    posIdx = ++posIdx % NUM_POSITIONS;
  }

void LegController::performUpdate() {
  // Serial.print(capsuleController->atPosition()); Serial.print(" ,"); Serial.print(shoulderController->atPosition()); Serial.print(" ,"); Serial.println(kneeController->atPosition());
    if (capsuleController->atPosition() &&
        shoulderController->atPosition() &&
        kneeController->atPosition()) {
          capsuleController->SetPosition(positionBuffer[posIdx].capsule);
          shoulderController->SetPosition(positionBuffer[posIdx].shoulder);
          kneeController->SetPosition(positionBuffer[posIdx].knee);
          // Serial.print(capsuleController->cmdPosition()); Serial.print(" ,"); Serial.print(shoulderController->cmdPosition()); Serial.print(" ,"); Serial.println(kneeController->cmdPosition());
          incrementPosition();
        }
}

