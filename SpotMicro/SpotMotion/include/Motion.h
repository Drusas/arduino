#ifndef _MOTION_H
#define _MOTION_H

#define DEBUG 2
// #include "ServoController.h"
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
// #define DEBUG 0

uint8_t servonum = 0;
uint16_t minPulse = SERVOMIN;
uint16_t maxPulse = SERVOMAX;

struct Joint {
  uint8_t servoIndex;
  uint16_t minPulse;
  uint16_t maxPulse;
  uint8_t minAngle;  
  uint8_t maxAngle;
  uint8_t cmdAngle;
  uint8_t homeAngle;
} knee, shoulder, capsule;

class ServoController;

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
  bool homed;
  uint8_t homePos;

  void incrementActualPosition();
  int clipAngle(int inputAngle);

public: 
  Sweeper(int interval, Joint *servoJoint, ServoController *controller);
  void SetPosition(int angle);
  bool atPosition();
  int cmdPosition();
  int actPosition();
  void home();
  bool getHomed(); 
  void Update();
};

Joint* joints[3];

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
  
  void incrementPosition();
    
 public:
  LegController(int interval, Sweeper *capsule, Sweeper *shoulder, Sweeper *knee, ServoController *controller);
  void addPosition(uint8_t c, uint8_t s, uint8_t k);
  void generateTrajectory(Bone* joints);
  void Update();
};

#endif
