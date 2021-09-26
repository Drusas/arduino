#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "LegController.h"
#include "SerialCommands.h"
#include "ServoController.h"
#include "ServoMotor.h"
#include "TaskManager.h"
#include "Utils.h"

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
  taskManager->setTasksEnabled(enabled);
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

void configureJoints()
{
  jointsLF[HX].minPulse = jointsLF[HY].minPulse = jointsLF[KNEE].minPulse = SERVOMIN;
  jointsLF[HX].maxPulse = jointsLF[HY].maxPulse = jointsLF[KNEE].maxPulse = SERVOMAX;

  jointsLF[HX].servoIndex = 4; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsLF[HX].minAngle = 60;
  jointsLF[HX].maxAngle = 90;
  jointsLF[HX].cmdAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLF[HX].homeAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75

  jointsLF[HY].servoIndex = 2; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsLF[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsLF[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsLF[HY].cmdAngle = 130; // RR 50; // RF 50 // LF/LR 130;
  jointsLF[HY].homeAngle = 130; // RF/RR 50; // LF/LR 130;

  jointsLF[KNEE].servoIndex = 0; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsLF[KNEE].minAngle = 30; // RF/RR 170; // LF/LR 30;
  jointsLF[KNEE].maxAngle = 180;
  jointsLF[KNEE].cmdAngle = 50; // RF/RR 170; // LF/LR 50;
  jointsLF[KNEE].homeAngle = 50; // RF/RR 170; // LF/LR 50;

  jointsLR[HX].minPulse = jointsLR[HY].minPulse = jointsLR[KNEE].minPulse = SERVOMIN;
  jointsLR[HX].maxPulse = jointsLR[HY].maxPulse = jointsLR[KNEE].maxPulse = SERVOMAX;

  jointsLR[HX].servoIndex = 11; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsLR[HX].minAngle = 35;
  jointsLR[HX].maxAngle = 65;
  jointsLR[HX].cmdAngle = 50; // RR 125; // RF 105; // LR 50; // LF 75
  jointsLR[HX].homeAngle = 50; // RR 125; // RF 105; // LR 50; // LF 75

  jointsLR[HY].servoIndex = 13; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsLR[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsLR[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsLR[HY].cmdAngle = 130; // RR 50; // RF 50 // LF/LR 130;
  jointsLR[HY].homeAngle = 130; // RF/RR 50; // LF/LR 130;

  jointsLR[KNEE].servoIndex = 15; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsLR[KNEE].minAngle = 30; // RF/RR 170; // LF/LR 30;
  jointsLR[KNEE].maxAngle = 180;
  jointsLR[KNEE].cmdAngle = 50; // RF/RR 170; // LF/LR 50;
  jointsLR[KNEE].homeAngle = 50; // RF/RR 170; // LF/LR 50;

  jointsRF[HX].minPulse = jointsRF[HY].minPulse = jointsRF[KNEE].minPulse = SERVOMIN;
  jointsRF[HX].maxPulse = jointsRF[HY].maxPulse = jointsRF[KNEE].maxPulse = SERVOMAX;

  jointsRF[HX].servoIndex = 5; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsRF[HX].minAngle = 90;
  jointsRF[HX].maxAngle = 120;
  jointsRF[HX].cmdAngle = 105; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRF[HX].homeAngle = 105; // RR 125; // RF 105; // LR 50; // LF 75

  jointsRF[HY].servoIndex = 3; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsRF[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsRF[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsRF[HY].cmdAngle = 50; // RR 50; // RF 50 // LF/LR 130;
  jointsRF[HY].homeAngle = 50; // RF/RR 50; // LF/LR 130;

  jointsRF[KNEE].servoIndex = 1; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsRF[KNEE].minAngle = 0; // RF/RR 170; // LF/LR 30;
  jointsRF[KNEE].maxAngle = 170;
  jointsRF[KNEE].cmdAngle = 170; // RF/RR 170; // LF/LR 50;
  jointsRF[KNEE].homeAngle = 170; // RF/RR 170; // LF/LR 50;

  jointsRR[HX].minPulse = jointsRR[HY].minPulse = jointsRR[KNEE].minPulse = SERVOMIN;
  jointsRR[HX].maxPulse = jointsRR[HY].maxPulse = jointsRR[KNEE].maxPulse = SERVOMAX;

  jointsRR[HX].servoIndex = 10; // RR 10; // RF 5; // LR 11; // LF 4;
  jointsRR[HX].minAngle = 110;
  jointsRR[HX].maxAngle = 140;
  jointsRR[HX].cmdAngle = 125; // RR 125; // RF 105; // LR 50; // LF 75
  jointsRR[HX].homeAngle = 125; // RR 125; // RF 105; // LR 50; // LF 75

  jointsRR[HY].servoIndex = 12; // RR 12; // RF 3; // LR 13; // LF 2;
  jointsRR[HY].minAngle = 0; // RR/RF 180; // LF/LR 0;
  jointsRR[HY].maxAngle = 180; // RR/RF 0; // LF/LR 180;
  jointsRR[HY].cmdAngle = 50; // RR 50; // RF 50 // LF/LR 130;
  jointsRR[HY].homeAngle = 50; // RF/RR 50; // LF/LR 130;

  jointsRR[KNEE].servoIndex = 14; // RR 14; // RF 1 // LR 15; // LF 0;
  jointsRR[KNEE].minAngle = 0; // RF/RR 170; // LF/LR 30;
  jointsRR[KNEE].maxAngle = 170;
  jointsRR[KNEE].cmdAngle = 170; // RF/RR 170; // LF/LR 50;
  jointsRR[KNEE].homeAngle = 170; // RF/RR 170; // LF/LR 50;

  TRACE("%s\n", "configureJoints COMPLETE");
}

void configureMotors() {
  servoController = new ServoController(1);

  motorsLF[HX] = new ServoMotor(20, &jointsLF[HX], servoController, pwm);
  motorsLF[HY] = new ServoMotor(20, &jointsLF[HY], servoController, pwm);
  motorsLF[KNEE] = new ServoMotor(20, &jointsLF[KNEE], servoController, pwm);

  motorsLR[HX] = new ServoMotor(20, &jointsLR[HX], servoController, pwm);
  motorsLR[HY] = new ServoMotor(20, &jointsLR[HY], servoController, pwm);
  motorsLR[KNEE] = new ServoMotor(20, &jointsLR[KNEE], servoController, pwm);

  motorsRF[HX] = new ServoMotor(20, &jointsRF[HX], servoController, pwm);
  motorsRF[HY] = new ServoMotor(20, &jointsRF[HY], servoController, pwm);
  motorsRF[KNEE] = new ServoMotor(20, &jointsRF[KNEE], servoController, pwm);

  motorsRR[HX] = new ServoMotor(20, &jointsRR[HX], servoController, pwm);
  motorsRR[HY] = new ServoMotor(20, &jointsRR[HY], servoController, pwm);
  motorsRR[KNEE] = new ServoMotor(20, &jointsRR[KNEE], servoController, pwm);

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
  
}

void configureLegs() {
  legLF = new LegController(108, 132, 15, 60, 20, motorsLF[HX], motorsLF[HY], motorsLF[KNEE], servoController);
  legLR = new LegController(108, 132, 15, 60, 20, motorsLR[HX], motorsLR[HY], motorsLR[KNEE], servoController);
  legRF = new LegController(108, 132, 15, 60, 20, motorsRF[HX], motorsRF[HY], motorsRF[KNEE], servoController);
  legRF->jointTranslationFactor(180);
  legRR = new LegController(108, 132, 15, 60, 20, motorsRR[HX], motorsRR[HY], motorsRR[KNEE], servoController);
  legRR->jointTranslationFactor(180);
}

SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
// SerialCommand cmd_mode_("MODE", cmd_mode);
SerialCommand cmd_hipx_("HX", cmd_hipx);
SerialCommand cmd_hipy_("HY", cmd_hipy);
SerialCommand cmd_knee_("KNEE", cmd_knee);
SerialCommand cmd_leg_("LEG", cmd_leg);
SerialCommand cmd_ik_("IK", cmd_ik);
// SerialCommand cmd_status_("STATUS", cmd_status);
// SerialCommand cmd_target_("TARGET", cmd_target);
// SerialCommand cmd_angle_("ANGLE", cmd_angle);
// SerialCommand cmd_home_("HOME", cmd_home);

void setup() {
  Serial.begin(57600);
  delay(100);
	serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_servo_enable_);
  // serial_commands_.AddCommand(&cmd_mode_);
  serial_commands_.AddCommand(&cmd_hipx_);
  serial_commands_.AddCommand(&cmd_hipy_);
  serial_commands_.AddCommand(&cmd_knee_);
  serial_commands_.AddCommand(&cmd_leg_);
  // serial_commands_.AddCommand(&cmd_status_);
  serial_commands_.AddCommand(&cmd_ik_);
  // serial_commands_.AddCommand(&cmd_angle_);
  // serial_commands_.AddCommand(&cmd_home_);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);

  configureJoints();
  configureMotors();
  configureLegs();
  configureTasks();
 
  TRACE("%s", "Spot is ready!");
}

void loop() {
  serial_commands_.ReadSerial();
  taskManager->updateTasks();
}