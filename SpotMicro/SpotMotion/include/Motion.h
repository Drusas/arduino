#ifndef _MOTION_H
#define _MOTION_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "IKModel.h"
#include "IMotor.h"
#include "IServoController.h"

#define DEBUG_MOTION 0

#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

#define SERVO_FREQ 50 
#define DELAY 2000
#define LOOP_DELAY 2000 

struct Joint {
  uint8_t servoIndex;
  uint16_t minPulse;
  uint16_t maxPulse;
  uint8_t minAngle;  
  uint8_t maxAngle;
  uint8_t cmdAngle;
  uint8_t homeAngle;
};


class Sweeper : public IMotor
{
  IServoController *servoController;
  Adafruit_PWMServoDriver driver;
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
  Sweeper(int interval, Joint *servoJoint, IServoController *controller, Adafruit_PWMServoDriver pwmDriver);
  void SetPosition(int angle);
  bool atPosition();
  int cmdPosition();
  int actPosition();
  void home();
  bool getHomed(); 
  void Update();
};

struct LegPosition {
  uint8_t capsule;
  uint8_t shoulder;
  uint8_t knee;
};

class LegController
{
  static const uint8_t NUM_POSITIONS = 10;
  IServoController *servoController;
  IMotor *capsuleController;
  IMotor *shoulderController;
  IMotor *kneeController;
  LegPosition positionBuffer[NUM_POSITIONS];
  uint8_t posIdx, bufferIdx;
  int  updateInterval;
  unsigned long lastUpdate;
  
  void incrementPosition();
    
 public:
  LegController(int interval, IMotor *capsule, IMotor *shoulder, IMotor *knee, IServoController *controller);
  void addPosition(uint8_t c, uint8_t s, uint8_t k);
  void generateTrajectory(Bone* joints);
  void Update();
};

#endif
