#include "ServoMotor.h"

ServoMotor::ServoMotor(int interval, Joint *servoJoint, IServoController *controller, Adafruit_PWMServoDriver pwmDriver)
{
  setEnabled(false);
  servoController = controller;
  driver = pwmDriver;
  updateInterval = interval;
  lastUpdate = 0;
  joint = servoJoint;
  cmdPos = joint->cmdAngle;
  actPos = cmdPos; 
  homePos = joint->homeAngle;
  homed = false;
  increment = 1;
}

void ServoMotor::incrementActualPosition() {
  uint8_t diff;
  diff  = abs(cmdPos - actPos);
  uint8_t i = increment;
  if (diff <= increment) {
    i = diff;
  }
  
  if (actPos < cmdPos) {
    actPos += i;
  } else {
    actPos -= i;
  }
  actPos = clipAngle(actPos);
}

int ServoMotor::clipAngle(int inputAngle) {
  int temp;
  if (inputAngle < joint->minAngle) {
    temp = joint->minAngle;
  } else if (inputAngle > joint->maxAngle) {
    temp = joint->maxAngle;
  } else {
    temp = inputAngle;
  }
  return temp;
}

void ServoMotor::SetPosition(int angle) {
    cmdPos = clipAngle((angle));
}

bool ServoMotor::atPosition() {
  return (cmdPos == actPos);
}

int ServoMotor::cmdPosition() {
  return cmdPos;
}

int ServoMotor::actPosition() {
  return actPos;
}

void ServoMotor::home() {
  if (servoController->getEnabled()) {
    long pulseLength = map(joint->homeAngle, 0, 180, joint->minPulse, joint->maxPulse);
    driver.setPWM(joint->servoIndex, 0, pulseLength);
    cmdPos = actPos = joint->homeAngle;
    homed = true;
  }
}

bool ServoMotor::getHomed() {
  return true;  // TEMP DISABLE
}

void ServoMotor::performUpdate() {
  if (!servoController->getEnabled() || !getHomed()) {
    return;
  }

  if (actPos != cmdPos) {
      incrementActualPosition();
      long pulseLength = map(actPos, 0, 180, joint->minPulse, joint->maxPulse);
      driver.setPWM(joint->servoIndex, 0, pulseLength);
      if (DEBUG_SERVOMOTOR > 0) {
        // Serial.print(cmdPos); Serial.print(" ,"); Serial.print(actPos); Serial.print(" ,"); Serial.println(atPosition()); 
      }
    }
}