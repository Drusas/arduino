#include "ServoMotor.h"
#include "Utils.h"

// #define DEBUG_SERVOMOTOR 1

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
//   increment = 5;
}

void ServoMotor::incrementActualPosition() {
  uint8_t diff;
  diff  = abs(cmdPos - actPos);
  uint8_t i = increment;
  if (joint->servoIndex == 1) {
      //printf("ServoMotor::incrementActualPosition() cmd: %d act: %d i: %d diff: %d inc: %d\n", cmdPos, actPos, i, diff, increment);
    }
  if (diff <= increment) {
    i = diff;
    if (joint->servoIndex == 1) {
      //printf("ServoMotor::incrementActualPosition() 1 diff <= increment diff: %d inc: %d i = %d\n", diff, increment, i);
    }
  }
  
  if (joint->servoIndex == 1) {
      //printf("ServoMotor::incrementActualPosition() 1 act: %d inc: %d\n", actPos, i);
    }
  if (actPos < cmdPos) {
    actPos += i;
  } else {
    actPos -= i;
  }
  if (joint->servoIndex == 1) {
      //printf("ServoMotor::incrementActualPosition() 1 act: %d\n", actPos);
    }
  actPos = clipAngle(actPos);
  if (joint->servoIndex == 1) {
      //printf("ServoMotor::incrementActualPosition() 1 act: %d\n", actPos);
    }
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

void ServoMotor::setPosition(int angle) {
    cmdPos = clipAngle((angle));
#ifdef DEBUG_SERVOMOTOR
    TRACE("%s, %d, %d, %d\n", "ServoMotor::setPosition:", this->joint->servoIndex, angle, cmdPos);
#endif
  //printf("ServoMotor::setPosition() %d, ang: %d, cmdPos: %d\n", this->joint->servoIndex, angle, cmdPos);
}

bool ServoMotor::atPosition() {
  // return true;
  bool result = false;
  result = (cmdPos == actPos); 
  //printf("ServoMotor::atPosition() %d result: %d cmd: %d act: %d\n",this->joint->servoIndex, result, cmdPos, actPos);
  return result;
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
  return true;  // temporarily disable while adding support for all legs
}

void ServoMotor::setSpeed(uint8_t speed) {
  // increment = speed;
}

int ServoMotor::getServoIndex() {
  return joint->servoIndex;
}

void ServoMotor::performUpdate() {
  if (joint->servoIndex == 1) {
    //printf("ServoMotor::performUpdate() enter %d\n", joint->servoIndex);
  }
  if (!servoController->getEnabled() || !getHomed()) {
    TRACE("%s%d%s%d\n","NOT PERFORMING UPDATE, ENABLED: ", servoController->getEnabled(), getHomed());
    return;
  }

  //TRACE("%s\n","PERFORMING UPDATE");

  if (actPos != cmdPos) {
    if (joint->servoIndex == 1) {
    //printf("ServoMotor::performUpdate() %d, %d, %d, %d\n",joint->servoIndex, cmdPos, actPos, (actPos == cmdPos));
    }
    incrementActualPosition();
    long pulseLength = map(actPos, 0, 180, joint->minPulse, joint->maxPulse);
    driver.setPWM(joint->servoIndex, 0, pulseLength);
#ifdef DEBUG_SERVOMOTOR
      TRACE("%d,%d,%d, %d,%d\n",joint->servoIndex, pulseLength, cmdPos, actPos, atPosition());
#endif
    if (joint->servoIndex == 1) {
    //printf("ServoMotor::performUpdate() %d,  %d, %d, %d, %d\n",joint->servoIndex, pulseLength, cmdPos, actPos, (actPos == cmdPos));
    }
  }
}