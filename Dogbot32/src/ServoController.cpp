#include <Arduino.h>
#include "ServoController.h"

ServoController::ServoController() : 
  enabled(false),
  motorManagementIdx(0) 
{
}

void ServoController::setEnabled(bool state) {
  this->enabled = state;
  for (uint8_t i = 0; i < motorManagementIdx; i++) {
    motors[i]->setEnabled(state);
  }
}

bool ServoController::getEnabled() {
  return this->enabled;
}

void ServoController::addMotor(IMotor *motor) {
  if (motorManagementIdx < MAX_NUM_MOTORS) {
    motors[motorManagementIdx++] = motor;
  }
}

bool ServoController::getHomed() {
  for (uint8_t i = 0; i < motorManagementIdx; i++) {
    IMotor *s = motors[i];
    if (!s->getHomed()) {
      return false;
    }
  }
  return true;
}

void ServoController::homeMotors() {
  for (uint8_t i = 0; i < motorManagementIdx; i++) {
    motors[i]->home();
  }
}