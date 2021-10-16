#ifndef _SERVOMOTOR_H
#define _SERVOMOTOR_H

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <Wire.h>
#include "IMotor.h"
#include "ITask.h"
#include "IServoController.h"



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
  float translate;
  int offset;
  int sign;
};

class ServoMotor : public IMotor, public ITask
{
  IServoController *servoController;
  Adafruit_PWMServoDriver driver;
  
  int actPos;
  int cmdPos;              
  int increment = 2;        // increment to move for each interval - THIS IS HOW WE CONTROL VELOCITY
  bool homed;
  uint8_t homePos;

  void incrementActualPosition();
  int clipAngle(int inputAngle);

public:
  Joint *joint;
  void performUpdate();

public: 
  ServoMotor(int interval, Joint *servoJoint, IServoController *controller, Adafruit_PWMServoDriver pwmDriver);
  void setPosition(int angle);
  bool atPosition();
  int cmdPosition();
  int actPosition();
  void home();
  bool getHomed();
  void setSpeed(uint8_t speed); 
  int getServoIndex() override;
};

#endif