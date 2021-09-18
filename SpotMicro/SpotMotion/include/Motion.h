#ifndef _MOTION_H
#define _MOTION_H

#define DEBUG 2

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "IKModel.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

#define SERVO_FREQ 50 
#define DELAY 2000
#define LOOP_DELAY 2000 
#define DEBUG 0

uint8_t servonum = 0;
uint16_t minPulse = SERVOMIN;
uint16_t maxPulse = SERVOMAX;

class ServoController {
  bool enabled;

public:
  void setEnabled(bool state) {
    this->enabled = state;
  }

  bool getEnabled() {
    return this->enabled;
  }

};

struct Joint {
  uint8_t servoIndex;
  uint16_t minPulse;
  uint16_t maxPulse;
  uint8_t minAngle;  
  uint8_t maxAngle;
  uint8_t cmdAngle;
} knee, shoulder, capsule;

Joint* joints[3];

class Sweeper
{
  ServoController *servoController;
  Adafruit_PWMServoDriver controller;
  Joint *joint;
  int actPos;
  int cmdPos;              
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position

  void incrementActualPosition() {
    uint8_t diff = abs(cmdPos - actPos);
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

  int clipAngle(int inputAngle) {
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

public: 
  Sweeper(int interval, Joint *servoJoint, ServoController *controller)
  {
    servoController = controller;
    updateInterval = interval;
    joint = servoJoint;
    cmdPos = joint->cmdAngle;
    actPos = cmdPos; // we don't want to move initially
    increment = 1;
  }

  void SetPosition(int angle) {
      cmdPos = clipAngle(angle);
  }

  bool atPosition() {
    return (cmdPos == actPos);
  }

  int cmdPosition() {
    return cmdPos;
  }

  int actPosition() {
    return actPos;
  }
  
  void Update()
  {
    if (!servoController->getEnabled()) {
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
};

struct LegPosition {
  uint8_t capsule;
  uint8_t shoulder;
  uint8_t knee;
};

class LegController
{
  static const uint8_t NUM_POSITIONS = 10;
  ServoController *servoController;
  Sweeper *capsuleController;
  Sweeper *shoulderController;
  Sweeper *kneeController;
  LegPosition positionBuffer[NUM_POSITIONS];
  uint8_t posIdx, bufferIdx;
  int  updateInterval;
  unsigned long lastUpdate;
  
  void incrementPosition() {
    posIdx = ++posIdx % NUM_POSITIONS;
  }
    
 public:
  LegController(int interval, Sweeper *capsule, Sweeper *shoulder, Sweeper *knee, ServoController *controller) {
    servoController = controller;
    updateInterval = interval;
    capsuleController = capsule;
    shoulderController = shoulder;
    kneeController = knee;
    posIdx = bufferIdx = 0;
  }

  void addPosition(uint8_t c, uint8_t s, uint8_t k) {
    positionBuffer[bufferIdx].capsule = c;
    positionBuffer[bufferIdx].shoulder = s;
    positionBuffer[bufferIdx].knee = k;
    bufferIdx = ++bufferIdx % NUM_POSITIONS;
  }

  void generateTrajectory(Bone* joints) {
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

  void Update() {
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
};


#endif
