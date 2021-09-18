#include <Arduino.h>
#include "ServoController.h"
#include "Motion.h"

ServoController::ServoController(uint8_t maxNumberMotorsToManage) {
    motors = (Sweeper**)malloc(sizeof(Sweeper*)*maxNumberMotorsToManage);
    motorManagementIdx = 0;
}

void ServoController::setEnabled(bool state) {
  this->enabled = state;
}

bool ServoController::getEnabled() {
  return this->enabled;
}

void ServoController::addMotor(Sweeper *motor) {
  if (motorManagementIdx < numMotors) {
    motors[motorManagementIdx++] = motor;
  }
}

bool ServoController::getHomed() {
  for (uint8_t i = 0; i < motorManagementIdx; i++) {
    Sweeper *s = motors[i];
    if (!s->getHomed()) {
      return false;
    }
  }
  return true;
}