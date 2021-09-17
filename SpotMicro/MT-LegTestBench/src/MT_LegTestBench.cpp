#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Drusas.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

#define SERVO_FREQ 50 
#define DELAY 2000
#define LOOP_DELAY 2000 
#define DEBUG 0

uint8_t servoEnable = 0;
uint8_t servonum = 0;
uint16_t minPulse = SERVOMIN;
uint16_t maxPulse = SERVOMAX;

char keyServoEnable[] = "enable", keyPose[] = "pose";
char keyMin[] = "min", keyMax[] = "max", keyIdx[] = "idx", keyMode[] = "mode";
char keyCapsule[] = "capsule", keyShoulder[] = "shoulder", keyKnee[] = "knee";

int calibrationMode = -1; // -1 = off, 0 = manual, 1 = pose, 2 = leg controller
int poseMode = -1; // 0 = sit, 1 = stand, 2 = loop sit/stand

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
  Sweeper(int interval, Joint *servoJoint)
  {
    updateInterval = interval;
    joint = servoJoint;
    cmdPos = joint->cmdAngle;
    actPos = cmdPos; // we don't want to move initially
    increment = 2;
  }

  void SetPosition(int angle) {
    //if (atPosition()) {
      cmdPos = clipAngle(angle);
    //}
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
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();
      if (actPos != cmdPos) {
        incrementActualPosition();
        long pulseLength = map(actPos, 0, 180, joint->minPulse, joint->maxPulse);
        pwm.setPWM(joint->servoIndex, 0, pulseLength);
        if (DEBUG > 0) {
          Serial.print(cmdPos); Serial.print(" ,"); Serial.print(actPos); Serial.print(" ,"); Serial.println(atPosition()); 
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
  bool enabled = false;
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
  LegController(int interval, Sweeper *capsule, Sweeper *shoulder, Sweeper *knee) {
    updateInterval = interval;
    capsuleController = capsule;
    shoulderController = shoulder;
    kneeController = knee;
    posIdx = bufferIdx = 0;
  }

  void setEnabled(bool state) {
    enabled = state;
  }

  void addPosition(uint8_t c, uint8_t s, uint8_t k) {
    positionBuffer[bufferIdx].capsule = c;
    positionBuffer[bufferIdx].shoulder = s;
    positionBuffer[bufferIdx].knee = k;
    bufferIdx = ++bufferIdx % NUM_POSITIONS;
  }

  void Update() {
    if (!enabled) {
      return;
    }
    
    if((millis() - lastUpdate) > updateInterval)
    {
      Serial.println("Leg Update");
      lastUpdate = millis();
      Serial.print(capsuleController->atPosition()); Serial.print(" ,"); Serial.print(shoulderController->atPosition()); Serial.print(" ,"); Serial.println(kneeController->atPosition());
      if (capsuleController->atPosition() &&
          shoulderController->atPosition() &&
          kneeController->atPosition()) {
            capsuleController->SetPosition(positionBuffer[posIdx].capsule);
            shoulderController->SetPosition(positionBuffer[posIdx].shoulder);
            kneeController->SetPosition(positionBuffer[posIdx].knee);
            Serial.print(capsuleController->cmdPosition()); Serial.print(" ,"); Serial.print(shoulderController->cmdPosition()); Serial.print(" ,"); Serial.println(kneeController->cmdPosition());
            incrementPosition();
          }
    }
  }
};

Sweeper* hipyController;
Sweeper* hipxController;
Sweeper* kneeController;
LegController* leg;

void setup() {
  Serial.begin(9600);
  Serial.println("Begin Leg Test Bench");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  capsule.servoIndex = 0;
  joints[0] = &capsule;
  shoulder.servoIndex = 1;
  joints[1] = &shoulder;
  knee.servoIndex = 2;
  joints[2] = &knee;

  capsule.minPulse = shoulder.minPulse = knee.minPulse = SERVOMIN;
  capsule.maxPulse = shoulder.maxPulse = knee.maxPulse = SERVOMAX;

  capsule.minAngle = 60;
  capsule.maxAngle = 120;
  capsule.cmdAngle = 90;

  shoulder.minAngle = 0;
  shoulder.maxAngle = 180;
  shoulder.cmdAngle = 180;

  knee.minAngle = 5;
  knee.maxAngle = 170;
  knee.cmdAngle = 0;

  hipyController = new Sweeper(20, &capsule);
  hipxController = new Sweeper(20, &shoulder);
  kneeController = new Sweeper(20, &knee);
  leg = new LegController(1000, hipyController, hipxController, kneeController);
  leg->addPosition(90,180,0);
  leg->addPosition(90,130,140);
  leg->addPosition(90,180,0);
  leg->addPosition(90,130,140);
  leg->addPosition(90,180,0);
  leg->addPosition(90,130,140);
  leg->addPosition(90,180,0);
  leg->addPosition(90,130,140);
  leg->addPosition(90,180,0);
  leg->addPosition(90,130,140);
}


void printJoints() {
  if (DEBUG > 0) {
    // Enable,Mode,Pose,Capsule,Shoulder,Knee
    Serial.print(servoEnable); Serial.print(", ");
    Serial.print(calibrationMode); Serial.print(", ");
    Serial.print(poseMode); Serial.print(", ");
    Serial.print(capsule.cmdAngle); Serial.print(", ");
    Serial.print(shoulder.cmdAngle); Serial.print(", ");
    Serial.println(knee.cmdAngle); 
//    Serial.print("Capsule, "); Serial.print(capsule.cmdAngle); Serial.print(", ");
//    Serial.print("Shoulder, "); Serial.print(shoulder.cmdAngle); Serial.print(", ");
//    Serial.print("Knee, "); Serial.println(knee.cmdAngle);
  }
}

void moveJointToCommandedAngle(int index) {
  Joint* j = joints[index];
  if (j->cmdAngle < j->minAngle) {
    j->cmdAngle = j->minAngle;
  }
  if (j->cmdAngle > j->maxAngle) {
    j->cmdAngle = j->maxAngle;
  }
  long pulseLength = map(j->cmdAngle, 0, 180, j->minPulse, j->maxPulse);
  if (DEBUG == 2) {
    Serial.print("cmd servo idx, "); Serial.print(j->servoIndex); Serial.print(", ");
    Serial.print("min, "); Serial.print(j->minPulse); Serial.print(", ");
    Serial.print("max, "); Serial.print(j->maxPulse); Serial.print(", ");
    Serial.print("angle, "); Serial.print(j->cmdAngle); Serial.print(", ");
    Serial.print("pulse, "); Serial.println(pulseLength); 
  }
  pwm.setPWM(j->servoIndex, 0, pulseLength);
}

void moveJoints() {
  if (servoEnable > 0) {
    for (int i = 0; i < 3; i++) {
      moveJointToCommandedAngle(i);
    }
  }
}

void sit() {
  capsule.cmdAngle = 90;
  shoulder.cmdAngle = 180;
  knee.cmdAngle = 0;
  moveJoints();
}

void stand() {
  capsule.cmdAngle = 90;
  shoulder.cmdAngle = 130;
  knee.cmdAngle = 140;
  moveJoints();
}

void loopSitStand() {
  for (int i = 0; i < 3; i++) {
    sit();
    delay(LOOP_DELAY);
    stand();
    delay(LOOP_DELAY);
  }
}

void moveMid() {
  uint16_t midPulse = minPulse + ((maxPulse - minPulse) / 2);
  Serial.print("Move Mid: ");
  Serial.println(midPulse);
  delay(DELAY);
  pwm.setPWM(servonum, 0, midPulse);
  Serial.println("At Mid");
  delay(DELAY);
}

void moveMin() {
  Serial.print("Move Min: ");
  Serial.println(minPulse);
  delay(DELAY);
  pwm.setPWM(servonum, 0, minPulse);
  Serial.println("At Min");
  delay(DELAY);
}

void moveMax() {
  delay(DELAY);
  Serial.print("Move Max: ");
  Serial.println(maxPulse);
  pwm.setPWM(servonum, 0, maxPulse);
  Serial.println("At Max");
  delay(DELAY);
}

void sweep() {
  moveMin();
  moveMax();
}

void updateServoInputs() {
  if (servoEnable > 0) {
    if (strcmp(keyMode, messageFromPC) == 0) {
      calibrationMode = integerFromPC;
      if (calibrationMode == 2) {
        leg->setEnabled(true); 
      } else {
        leg->setEnabled(false); 
      }
    }
    
    if (strcmp(keyCapsule,messageFromPC) == 0) {
      capsule.cmdAngle = integerFromPC; 
    }
  
    if (strcmp(keyShoulder,messageFromPC) == 0) {
      Serial.print("set shoulder: ");
      Serial.println(integerFromPC);
      shoulder.cmdAngle = integerFromPC; 
//      knee.cmdAngle = 140; 
//      shoulder.cmdAngle = 130;
    }

    if (strcmp(keyKnee,messageFromPC) == 0) {
      Serial.print("set knee: ");
      Serial.print(integerFromPC);
      knee.cmdAngle = integerFromPC; 
      long pulseLength = map(knee.cmdAngle, 0, 180, knee.minPulse, knee.maxPulse);
      Serial.print(" ,");
      Serial.println(pulseLength);
//      knee.cmdAngle = 0; 
//      shoulder.cmdAngle = 180;
    }

    if (strcmp(keyPose,messageFromPC) == 0) {
      poseMode = integerFromPC; 
    }
  }
}

void updateInputs() {
  if (newData) {
    if (strcmp(keyServoEnable, messageFromPC) == 0) {
      Serial.println("enable servos");
      servoEnable = integerFromPC; 
    }
    updateServoInputs();
  }
}

// todo: change this to use millis
void updateVariables() {
  static long updateCounter = 0;
  if (++updateCounter % 500 == 0)
  {
//    Serial.println("Check input");
    recvWithEndMarker();
    parseData();
    updateInputs();
    newData = false;
  }
}

void updateMode() {
  if (newData) {
    if (strcmp(keyMode,messageFromPC) == 0) {
      calibrationMode = integerFromPC;
      Serial.print("Mode: ");
      Serial.print(calibrationMode);
      Serial.println(" 0=sweep, 1=min, 2=max");
    }
  }
}

void loop() {
  updateVariables();
  leg->Update();
  // kneeController->SetPosition(knee.cmdAngle);
  // kneeController->Update();
  // shoulderController->SetPosition(shoulder.cmdAngle);
  // shoulderController->Update();
  // capsuleController->SetPosition(capsule.cmdAngle);
  // capsuleController->Update();

  if (DEBUG) {
  static long debugCounter = 0;
    if (++debugCounter % 10000 == 0) {
      Serial.print("Knee, ");
      Serial.print(kneeController->atPosition()); Serial.print(" ,");
      Serial.print(kneeController->cmdPosition()); Serial.print(" ,");
      Serial.println(kneeController->actPosition());
      Serial.print("shoulder, ");
      Serial.print(hipxController->atPosition()); Serial.print(" ,");
      Serial.print(hipxController->cmdPosition()); Serial.print(" ,");
      Serial.println(hipxController->actPosition());
      Serial.print("capsule, ");
      Serial.print(hipyController->atPosition()); Serial.print(" ,");
      Serial.print(hipyController->cmdPosition()); Serial.print(" ,");
      Serial.println(hipyController->actPosition());
    }
  }
}

void moveToPose() {
  switch (poseMode) {
    case 0:
      sit();
      break;
    case 1:
      stand();
      break;
    case 2:
      loopSitStand();
      break;
    default:
      break;
  }
}

void executeMode() {
  switch (calibrationMode) {
    case 0:
      moveJoints();
      break;
    case 1:
      moveToPose();
      break;
    default:
      break;
  }
}

void loop2() {
  updateVariables();
  executeMode();
  printJoints();
  delay(1500);
}






