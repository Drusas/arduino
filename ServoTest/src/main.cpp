#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "GaitTask.h"
#include "LegController.h"
#include "SerialCommands.h"
#include "ServoController.h"
#include "ServoMotor.h"
#include "TaskManager.h"
#include "TrajectoryGenerator.h"
#include "Utils.h"
#include "Controller.h"

Controller ctlr;
GaitTask *gaitTask;
State state;
Command cmd;

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
IServoController* servoController;
TaskManager* taskManager;

enum JointIdx {
  HX = 0,
  HY = 1,
  KNEE = 2,
};

Joint jointsLF[3];
Joint jointsLR[3];
Joint jointsRF[3];
Joint jointsRR[3];

ServoMotor* motorsLF[3];
ServoMotor* motorsLR[3];
ServoMotor* motorsRF[3];
ServoMotor* motorsRR[3];

LegController *legLF, *legLR, *legRF, *legRR;

bool enabled = 0;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
  TRACESC("Unrecognized command [%s]\n", cmd);
}

void cmd_servo_enable(SerialCommands* sender) {
  enabled = !enabled;
  servoController->setEnabled(enabled);
  // taskManager->setTasksEnabled(enabled);
  for (uint8_t i = 0; i < 3; i++) {
    motorsLF[i]->setEnabled(enabled);
    motorsLR[i]->setEnabled(enabled);
    motorsRF[i]->setEnabled(enabled);
    motorsRR[i]->setEnabled(enabled);
  }
  
  TRACESC("SERVO ENABLE: %d\n", enabled);
}

void cmd_hipx(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HX [ANGLE] ");
		return;
	}

	uint8_t angle = atoi(angle_str);
  motorsLF[HX]->setPosition(angle);
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HY [ANGLE] ");
		return;
	}
  
	uint8_t angle = atoi(angle_str);
  motorsLF[HY]->setPosition(angle);
}

void cmd_knee(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}
  
	uint8_t angle = atoi(angle_str);
  motorsLF[KNEE]->setPosition(angle);
}

void cmd_leg(SerialCommands* sender) {
  char* idx_str = sender->Next();
  if (idx_str == NULL) {
		sender->GetSerial()->println("ERROR IK IDX LEG [X, Y, Z]");
		return;
  }
  char* hx_str = sender->Next();
	if (hx_str == NULL) {
		sender->GetSerial()->println("ERROR LEG [HX, HY, KNEE]");
		return;
	}
  char* hy_str = sender->Next();
  if (hx_str == NULL) {
		sender->GetSerial()->println("ERROR LEG [HX, HY, KNEE]");
		return;
	}

  char* knee_str = sender->Next();
  if (hx_str == NULL) {
		sender->GetSerial()->println("ERROR LEG [HX, HY, KNEE]");
		return;
	}
  TRACESC("%s%s%s%s\n", "LEG", hx_str, hy_str, knee_str);
  
  int idx = atoi(idx_str);
	uint8_t angleHx = atoi(hx_str);
  uint8_t angleHy = atoi(hy_str);
  uint8_t angleK = atoi(knee_str);
  
  switch (idx)
  {
  case 0:
    legLF->moveToAngles(angleHx, angleHy, angleK);
    break;
  case 1:
    legLR->moveToAngles(angleHx, angleHy, angleK);
    break;
  case 2:
    legRF->moveToAngles(angleHx, angleHy, angleK);
    break;
  case 3:
    legRR->moveToAngles(angleHx, angleHy, angleK);
    break;
  default:
    TRACESC("%s, %d", "Invalid index", idx);
    break;
  }
}

// IK idx x y z
void cmd_ik(SerialCommands* sender) {
  TRACESC("%s\n", "Start IK command");
  char* idx_str = sender->Next();
	if (idx_str == NULL) {
		sender->GetSerial()->println("ERROR IK IDX LEG [X, Y, Z]");
		return;
  }
  TRACESC("%s%s\n", "IDX, ", idx_str);
  char* hx_str = sender->Next();
	if (hx_str == NULL) {
		sender->GetSerial()->println("ERROR IK IDX LEG [X, Y, Z]");
		return;
	}
  TRACESC("%s%s\n", "HX, ", hx_str);

  char* hy_str = sender->Next();
  if (hx_str == NULL) {
		sender->GetSerial()->println("ERROR IK IDX LEG [X, Y, Z]");
		return;
	}
  TRACESC("%s%s\n", "HY, ", hy_str);

  char* knee_str = sender->Next();
  if (knee_str == NULL) {
		sender->GetSerial()->println("ERROR IK LEG [X, Y, Z]");
		return;
	}
  TRACESC("%s%s\n", "KNEE, ", knee_str);
  TRACESC("%s %s %s %s %s\n", "IK",idx_str, hx_str, hy_str, knee_str);
  
  int idx = atoi(idx_str);
	int x = atoi(hx_str);
  int y = atoi(hy_str);
  int z = atoi(knee_str);
  switch (idx)
  {
  case 0:
    legLF->moveToXYZ(x, y, z);
    break;
  case 1:
    legLR->moveToXYZ(x, y, z);
    break;
  case 2:
    legRF->moveToXYZ(x, y, z);
    break;
  case 3:
    legRR->moveToXYZ(x, y, z);
    break;
  default:
    TRACESC("%s, %d", "Invalid index", idx);
    break;
  }
  
}

void cmd_stand(SerialCommands* sender) {
  TRACE("%s\n", "STAND");
  legLF->moveToXYZ(0, 60, 220);
  legLR->moveToXYZ(-50, 60, 220);
  legRF->moveToXYZ(0, 60, 220);
  legRR->moveToXYZ(-50, 60, 220);
}

void cmd_lay(SerialCommands* sender) {
  TRACE("%s\n", "LAY");
  legLF->moveToXYZ(0, 60, 100);
  legLR->moveToXYZ(-50, 60, 100);
  legRF->moveToXYZ(0, 60, 100);
  legRR->moveToXYZ(-50, 60, 100);
}

void cmd_sit(SerialCommands* sender) {
  legLF->setEnabled(false);
  TRACE("%s\n", "SIT");
  // legLF->moveToXYZ(0, 60, 220);
  // legLR->moveToXYZ(-50, 60, 100);
  // legRF->moveToXYZ(0, 60, 220);
  // legRR->moveToXYZ(-50, 60, 100);
}

void cmd_circle(SerialCommands* sender) {
  TRACE("%s\n", "CIRC");
  int numPoints = 4;
  Point *buffer;
  //buffer = TrajectoryGenerator::GenerateCircle(25, 180, 50, numPoints);
  buffer = TrajectoryGenerator::GenerateRectangle(25, 180, 50, 150, numPoints);
  for (int i = 0; i < numPoints; i++) {
      TRACE("%d: %0.2f, %0.2f, %0.2f\n", i, buffer[i].x, buffer[i].y, buffer[i].z);
      legLF->addPoint(buffer[i]);
  }

  legLF->setEnabled(true);
  // legLF->followTrajectory(buffer, numPoints);

  free (buffer);
}

void cmd_walk(SerialCommands* sender) {
  TRACE("%s\n", "WALK");
  char* enable_str = sender->Next();
	if (enable_str == NULL) {
		sender->GetSerial()->println("ERROR WALK ENABLE");
		return;
  }
  TRACESC("%s%s\n", "ENABLE, ", enable_str);
  int enable = atoi(enable_str);
  if (enable < 1) {
    cmd.horizontalVelocity[0] = 0.0;
    gaitTask->setEnabled(false);
    legRF->setEnabled(false);
    legLF->setEnabled(false);
    legRR->setEnabled(false);
    legLR->setEnabled(false);
  }
  else {
    legRF->setEnabled(true);
    // legLF->setEnabled(true);
    // legRR->setEnabled(true);
    // legLR->setEnabled(true);
    cmd.horizontalVelocity[0] = 100.5;
    gaitTask->setEnabled(true);
  }
}

void configureJoints()
{
  jointsLF[HX].minPulse = jointsLF[HY].minPulse = jointsLF[KNEE].minPulse = SERVOMIN;
  jointsLF[HX].maxPulse = jointsLF[HY].maxPulse = jointsLF[KNEE].maxPulse = SERVOMAX;

  jointsLF[HX].servoIndex = 4; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsLF[HX].minAngle = 60;
  jointsLF[HX].maxAngle = 90;
  jointsLF[HX].cmdAngle = 80; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLF[HX].homeAngle = 80; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLF[HX].translate = 90;
  jointsLF[HX].offset = -10;
  jointsLF[HX].sign = -1;

  jointsLF[HY].servoIndex = 2; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsLF[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsLF[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsLF[HY].cmdAngle = 175; // RR 50; // RF 50 // LF/LR 130;
  jointsLF[HY].homeAngle = 175; // RF/RR 50; // LF/LR 130;
  jointsLF[HY].translate = 90;
  jointsLF[HY].offset = 0;
  jointsLF[HY].sign = 1;

  jointsLF[KNEE].servoIndex = 0; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsLF[KNEE].minAngle = 30; // RF/RR 170; // LF/LR 30;
  jointsLF[KNEE].maxAngle = 180;
  jointsLF[KNEE].cmdAngle = 39; // RF/RR 170; // LF/LR 50;
  jointsLF[KNEE].homeAngle = 39; // RF/RR 170; // LF/LR 50;
  jointsLF[KNEE].translate = 0;
  jointsLF[KNEE].offset = 0;
  jointsLF[KNEE].sign = 1;

  jointsLR[HX].minPulse = jointsLR[HY].minPulse = jointsLR[KNEE].minPulse = SERVOMIN;
  jointsLR[HX].maxPulse = jointsLR[HY].maxPulse = jointsLR[KNEE].maxPulse = SERVOMAX;

  jointsLR[HX].servoIndex = 11; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsLR[HX].minAngle = 45;
  jointsLR[HX].maxAngle = 75;
  jointsLR[HX].cmdAngle = 60; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLR[HX].homeAngle = 60; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLR[HX].translate = 90;
  jointsLR[HX].offset = -30;
  jointsLR[HX].sign = -1;

  jointsLR[HY].servoIndex = 13; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsLR[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsLR[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsLR[HY].cmdAngle = 180; // RR 50; // RF 50 // LF/LR 130;
  jointsLR[HY].homeAngle = 180; // RF/RR 50; // LF/LR 130;
  jointsLR[HY].translate = 90;
  jointsLR[HY].offset = 0;
  jointsLR[HY].sign = 1;

  jointsLR[KNEE].servoIndex = 15; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsLR[KNEE].minAngle = 30; // RF/RR 170; // LF/LR 30;
  jointsLR[KNEE].maxAngle = 180;
  jointsLR[KNEE].cmdAngle = 47; // RF/RR 170; // LF/LR 50;
  jointsLR[KNEE].homeAngle = 47; // RF/RR 170; // LF/LR 50;
  jointsLR[KNEE].translate = 0;
  jointsLR[KNEE].offset = 0;
  jointsLR[KNEE].sign = 1;

  jointsRF[HX].minPulse = jointsRF[HY].minPulse = jointsRF[KNEE].minPulse = SERVOMIN;
  jointsRF[HX].maxPulse = jointsRF[HY].maxPulse = jointsRF[KNEE].maxPulse = SERVOMAX;

  jointsRF[HX].servoIndex = 5; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsRF[HX].minAngle = 90;
  jointsRF[HX].maxAngle = 120;
  jointsRF[HX].cmdAngle = 100; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRF[HX].homeAngle = 100; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRF[HX].translate = 90;
  jointsRF[HX].offset = 10;
  jointsRF[HX].sign = -1;

  jointsRF[HY].servoIndex = 3; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsRF[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsRF[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsRF[HY].cmdAngle = 4; // RR 50; // RF 50 // LF/LR 130;
  jointsRF[HY].homeAngle = 4; // RF/RR 50; // LF/LR 130;
  jointsRF[HY].translate = 90;
  jointsRF[HY].offset = 0;
  jointsRF[HY].sign = -1;

  jointsRF[KNEE].servoIndex = 1; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsRF[KNEE].minAngle = 0; // RF/RR 170; // LF/LR 30;
  jointsRF[KNEE].maxAngle = 170;
  jointsRF[KNEE].cmdAngle = 170; // RF/RR 170; // LF/LR 50;
  jointsRF[KNEE].homeAngle = 170; // RF/RR 170; // LF/LR 50;
  jointsRF[KNEE].translate = 180;
  jointsRF[KNEE].offset = 32;
  jointsRF[KNEE].sign = -1;

  jointsRR[HX].minPulse = jointsRR[HY].minPulse = jointsRR[KNEE].minPulse = SERVOMIN;
  jointsRR[HX].maxPulse = jointsRR[HY].maxPulse = jointsRR[KNEE].maxPulse = SERVOMAX;

  jointsRR[HX].servoIndex = 10; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsRR[HX].minAngle = 105;
  jointsRR[HX].maxAngle = 135;
  jointsRR[HX].cmdAngle = 120; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRR[HX].homeAngle = 120; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRR[HX].translate = 90;
  jointsRR[HX].offset = 30;
  jointsRR[HX].sign = -1;

  jointsRR[HY].servoIndex = 12; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsRR[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsRR[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsRR[HY].cmdAngle = 0; // RR 50; // RF 50 // LF/LR 130;
  jointsRR[HY].homeAngle = 0; // RF/RR 50; // LF/LR 130;
  jointsRR[HY].translate = 90;
  jointsRR[HY].offset = 0;
  jointsRR[HY].sign = -1;

  jointsRR[KNEE].servoIndex = 14; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsRR[KNEE].minAngle = 0; // RF/RR 170; // LF/LR 30;
  jointsRR[KNEE].maxAngle = 170;
  jointsRR[KNEE].cmdAngle = 157; // RF/RR 170; // LF/LR 50;
  jointsRR[KNEE].homeAngle = 157; // RF/RR 170; // LF/LR 50;
  jointsRR[KNEE].translate = 180;
  jointsRR[KNEE].offset = 25;
  jointsRR[KNEE].sign = -1;

  TRACE("%s\n", "configureJoints COMPLETE");
}

void configureMotors() {
  servoController = new ServoController(1);

  motorsLF[HX] = new ServoMotor(20, &jointsLF[HX], servoController, pwm);
  motorsLF[HY] = new ServoMotor(20, &jointsLF[HY], servoController, pwm);
  motorsLF[KNEE] = new ServoMotor(20, &jointsLF[KNEE], servoController, pwm);
  motorsLF[KNEE]->setSpeed(2);

  motorsLR[HX] = new ServoMotor(20, &jointsLR[HX], servoController, pwm);
  motorsLR[HY] = new ServoMotor(20, &jointsLR[HY], servoController, pwm);
  motorsLR[KNEE] = new ServoMotor(20, &jointsLR[KNEE], servoController, pwm);
  motorsLR[KNEE]->setSpeed(2);

  motorsRF[HX] = new ServoMotor(20, &jointsRF[HX], servoController, pwm);
  motorsRF[HY] = new ServoMotor(20, &jointsRF[HY], servoController, pwm);
  motorsRF[KNEE] = new ServoMotor(20, &jointsRF[KNEE], servoController, pwm);
  motorsRF[KNEE]->setSpeed(2);

  motorsRR[HX] = new ServoMotor(20, &jointsRR[HX], servoController, pwm);
  motorsRR[HY] = new ServoMotor(20, &jointsRR[HY], servoController, pwm);
  motorsRR[KNEE] = new ServoMotor(20, &jointsRR[KNEE], servoController, pwm);
  motorsRR[KNEE]->setSpeed(2);

  TRACE("%s\n", "configureMotors COMPLETE");
}

void configureTasks() {
  taskManager = new TaskManager(32);
  for (uint8_t i = 0; i < 3; i++) {
    taskManager->addTask(motorsLF[i]);
    taskManager->addTask(motorsLR[i]);
    taskManager->addTask(motorsRF[i]);
    taskManager->addTask(motorsRR[i]);
  }

  // legRF->setEnabled(false);
  taskManager->addTask(legRF);
  taskManager->addTask(legLF);
  taskManager->addTask(legRR);
  taskManager->addTask(legLR);

  taskManager->addTask(gaitTask);
  
}

void configureController() {
  state.setAllFootLocations(ctlr.spotConfig->defaultStance);
  gaitTask = new GaitTask(20, &ctlr, &state, &cmd);
  gaitTask->setLegs(legRF, legLF, legRR, legLR);
}

void configureLegs() {
  legLF = new LegController(108, 132, 15, 60, 20, motorsLF[HX], motorsLF[HY], motorsLF[KNEE], servoController);
  legLF->setHxTranslationAndOffset(radians(jointsLF[HX].translate), radians(jointsLF[HX].offset), jointsLF[HX].sign);
  legLF->setHyTranslationAndOffset(radians(jointsLF[HY].translate), radians(jointsLF[HY].offset), jointsLF[HY].sign);
  legLF->setKneeTranslationAndOffset(radians(jointsLF[KNEE].translate), radians(jointsLF[KNEE].offset), jointsLF[KNEE].sign);
  legLF->setHxConstraints(radians(jointsLF[HX].minAngle), radians(jointsLF[HX].maxAngle));
  legLF->setHyConstraints(radians(jointsLF[HY].minAngle), radians(jointsLF[HY].maxAngle));
  legLF->setKneeConstraints(radians(jointsLF[KNEE].minAngle), radians(jointsLF[KNEE].maxAngle));

  legLR = new LegController(108, 132, 15, 60, 20, motorsLR[HX], motorsLR[HY], motorsLR[KNEE], servoController);
  legLR->setHxTranslationAndOffset(radians(jointsLR[HX].translate), radians(jointsLR[HX].offset), jointsLR[HX].sign);
  legLR->setHyTranslationAndOffset(radians(jointsLR[HY].translate), radians(jointsLR[HY].offset), jointsLR[HY].sign);
  legLR->setKneeTranslationAndOffset(radians(jointsLR[KNEE].translate), radians(jointsLR[KNEE].offset), jointsLR[KNEE].sign);
  legLR->setHxConstraints(radians(jointsLR[HX].minAngle), radians(jointsLR[HX].maxAngle));
  legLR->setHyConstraints(radians(jointsLR[HY].minAngle), radians(jointsLR[HY].maxAngle));
  legLR->setKneeConstraints(radians(jointsLR[KNEE].minAngle), radians(jointsLR[KNEE].maxAngle));
  
  legRF = new LegController(108, 132, 15, 60, 20, motorsRF[HX], motorsRF[HY], motorsRF[KNEE], servoController);
  legRF->setHxTranslationAndOffset(radians(jointsRF[HX].translate), radians(jointsRF[HX].offset), jointsRF[HX].sign);
  legRF->setHyTranslationAndOffset(radians(jointsRF[HY].translate), radians(jointsRF[HY].offset), jointsRF[HY].sign);
  legRF->setKneeTranslationAndOffset(radians(jointsRF[KNEE].translate), radians(jointsRF[KNEE].offset), jointsRF[KNEE].sign);
  legRF->setHxConstraints(radians(jointsRF[HX].minAngle), radians(jointsRF[HX].maxAngle));
  legRF->setHyConstraints(radians(jointsRF[HY].minAngle), radians(jointsRF[HY].maxAngle));
  legRF->setKneeConstraints(radians(jointsRF[KNEE].minAngle), radians(jointsRF[KNEE].maxAngle));

  legRR = new LegController(108, 132, 15, 60, 20, motorsRR[HX], motorsRR[HY], motorsRR[KNEE], servoController);
  legRR->setHxTranslationAndOffset(radians(jointsRR[HX].translate), radians(jointsRR[HX].offset), jointsRR[HX].sign);
  legRR->setHyTranslationAndOffset(radians(jointsRR[HY].translate), radians(jointsRR[HY].offset), jointsRR[HY].sign);
  legRR->setKneeTranslationAndOffset(radians(jointsRR[KNEE].translate), radians(jointsRR[KNEE].offset), jointsRR[KNEE].sign);
  legRR->setHxConstraints(radians(jointsRR[HX].minAngle), radians(jointsRR[HX].maxAngle));
  legRR->setHyConstraints(radians(jointsRR[HY].minAngle), radians(jointsRR[HY].maxAngle));
  legRR->setKneeConstraints(radians(jointsRR[KNEE].minAngle), radians(jointsRR[KNEE].maxAngle));
}

SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
SerialCommand cmd_hipx_("HX", cmd_hipx);
SerialCommand cmd_hipy_("HY", cmd_hipy);
SerialCommand cmd_knee_("KNEE", cmd_knee);
SerialCommand cmd_leg_("LEG", cmd_leg);
SerialCommand cmd_ik_("IK", cmd_ik);
SerialCommand cmd_stand_("STAND", cmd_stand);
SerialCommand cmd_sit_("SIT", cmd_sit);
SerialCommand cmd_lay_("LAY", cmd_lay);
SerialCommand cmd_circle_("CIRC", cmd_circle);
SerialCommand cmd_walk_("WALK", cmd_walk);

int gaitCounter;

void setup() {
  Serial.begin(57600);
  delay(10);

	serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_servo_enable_);
  serial_commands_.AddCommand(&cmd_hipx_);
  serial_commands_.AddCommand(&cmd_hipy_);
  serial_commands_.AddCommand(&cmd_knee_);
  serial_commands_.AddCommand(&cmd_leg_);
  serial_commands_.AddCommand(&cmd_ik_);
  serial_commands_.AddCommand(&cmd_stand_);
  serial_commands_.AddCommand(&cmd_sit_);
  serial_commands_.AddCommand(&cmd_lay_);
  serial_commands_.AddCommand(&cmd_circle_);
  serial_commands_.AddCommand(&cmd_walk_);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  configureJoints();
  configureMotors();
  configureLegs();
  configureController();
  configureTasks();

  TRACE("%s", "Spot is ready!");
}

void loop() {
  serial_commands_.ReadSerial();

  taskManager->updateTasks();
}