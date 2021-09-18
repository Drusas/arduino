#include <Arduino.h>
#include "Motion.h"
#include "ServoController.h"

Sweeper::Sweeper(int interval, Joint *servoJoint, ServoController *controller)
{
  servoController = controller;
  updateInterval = interval;
  joint = servoJoint;
  cmdPos = joint->cmdAngle;
  actPos = cmdPos; 
  homePos = joint->homeAngle;
  homed = false;
  increment = 1;
}

void Sweeper::incrementActualPosition() {
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

int Sweeper::clipAngle(int inputAngle) {
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

void Sweeper::SetPosition(int angle) {
    cmdPos = clipAngle(angle);
}

bool Sweeper::atPosition() {
  return (cmdPos == actPos);
}

int Sweeper::cmdPosition() {
  return cmdPos;
}

int Sweeper::actPosition() {
  return actPos;
}

void Sweeper::home() {
  if (servoController->getEnabled()) {
    long pulseLength = map(joint->homeAngle, 0, 180, joint->minPulse, joint->maxPulse);
    pwm.setPWM(joint->servoIndex, 0, pulseLength);
    homed = true;
  }
}

bool Sweeper::getHomed() {
  return homed;
}

void Sweeper::Update()
{
  if (!servoController->getEnabled() || !getHomed()) {
    return;
  }

  if((millis() - lastUpdate) > updateInterval)  // time to update
  {
    lastUpdate = millis();
    if (actPos != cmdPos) {
        incrementActualPosition();
        long pulseLength = map(actPos, 0, 180, joint->minPulse, joint->maxPulse);
        pwm.setPWM(joint->servoIndex, 0, pulseLength);
        if (DEBUG > 0) {
          // Serial.print(cmdPos); Serial.print(" ,"); Serial.print(actPos); Serial.print(" ,"); Serial.println(atPosition()); 
        }
      }
    
  }
}

LegController::LegController(int interval, Sweeper *capsule, Sweeper *shoulder, Sweeper *knee, ServoController *controller) {
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
    float shoulder = toDegrees(joints->getAngle());
    float knee = 180 - toDegrees(joints->getChild()->getAngle());
    uint8_t s = (uint8_t)shoulder;
    uint8_t k = (uint8_t)knee;
    positionBuffer[i].shoulder = s;
    positionBuffer[i].knee = k;
  }
  delete points;
}

void LegController::Update() {
  if (!servoController->getEnabled()) {
    return;
  }
  
  if((millis() - lastUpdate) > updateInterval)
  {
    // Serial.println("Leg Update");
    lastUpdate = millis();
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
}

