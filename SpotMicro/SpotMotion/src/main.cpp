#include <Arduino.h>
#include "SerialCommands.h"
#include "LegController.h"
#include "IKModel.h"
#include "ServoController.h"
#include "ServoMotor.h"

#define DEBUG_MAIN 0

#define NUM_TASKS 10
ITask **taskList;
uint8_t managedTaskIdx = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

enum MotionMode {
  NONE = 0,
  MAN = 1,
  POSE = 2,
  WALK = 3,
  POINT = 4,
};

uint8_t servonum = 0;
uint16_t minPulse = SERVOMIN;
uint16_t maxPulse = SERVOMAX;

Joint* joints[3];
Joint capsule, shoulder, knee;
MotionMode motionMode = MotionMode::NONE;
IServoController* servoController;
ServoMotor* hipyMotor;
ServoMotor* hipxMotor;
ServoMotor* kneeMotor;
LegController* leg;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void addTask(ITask *task) {
  if (managedTaskIdx < NUM_TASKS) {
    taskList[managedTaskIdx++] = task;
  }
}

void updateTasks() {
  for (uint8_t i = 0; i < NUM_TASKS; i++) {
    ITask* t = taskList[i];
    if (t != 0) {
      t->Update();
    }
  }
}

void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
	sender->GetSerial()->print("Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

void cmd_mode(SerialCommands* sender) {
  char* mode_str = sender->Next();
	if (mode_str == NULL)	{
		sender->GetSerial()->println("ERROR NO MODE - MODE [MAN/POSE/WALK]");
		return;
	}

  if (strcmp(mode_str, "NONE") == 0) {
    motionMode = MotionMode::NONE;
    hipxMotor->setEnabled(false);
    hipyMotor->setEnabled(false);
    kneeMotor->setEnabled(false);
    leg->setEnabled(false);
  } 
  else if (strcmp(mode_str, "MAN") == 0) {
    motionMode = MotionMode::MAN;
    hipxMotor->setEnabled(true);
    hipyMotor->setEnabled(true);
    kneeMotor->setEnabled(true);
    leg->setEnabled(false);
  } 
  else if (strcmp(mode_str, "POSE") == 0) {
    motionMode = MotionMode::POSE;
  } 
  else if (strcmp(mode_str, "WALK") == 0) {
    leg->generateTrajectory();
    motionMode = MotionMode::WALK;
    motionMode = MotionMode::MAN;
    hipxMotor->setEnabled(true);
    hipyMotor->setEnabled(true);
    kneeMotor->setEnabled(true);
    leg->setEnabled(true);
  } 
  else if (strcmp(mode_str, "POINT") == 0) {
    motionMode = MotionMode::POINT;
  } 
  else {
    sender->GetSerial()->print("ERROR UNRECOGNIZED MODE: ");
    sender->GetSerial()->println(mode_str);
  }

  sender->GetSerial()->print("MOTION MODE: ");
	sender->GetSerial()->println(motionMode);
}

void cmd_servo_enable(SerialCommands* sender) {
  servoController->setEnabled(!servoController->getEnabled());
  sender->GetSerial()->print("SERVO ENABLE: ");
	sender->GetSerial()->println(servoController->getEnabled());
}

void cmd_home(SerialCommands* sender) {
  servoController->homeMotors();
}

void cmd_hipx(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPX [ANGLE] ");
		return;
	}

	int angle = atoi(angle_str);
  hipxMotor->SetPosition((angle));
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPY [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  hipyMotor->SetPosition((angle));
}

void cmd_knee(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  kneeMotor->SetPosition((angle));
}

void cmd_status(SerialCommands* sender) {
  
  sender->GetSerial()->print("CMDPOS: ");
  sender->GetSerial()->print(hipyMotor->cmdPosition()); Serial.print(" ,"); 
  sender->GetSerial()->print(hipxMotor->cmdPosition()); Serial.print(" ,"); 
  sender->GetSerial()->println(kneeMotor->cmdPosition());

  sender->GetSerial()->print("ATPOS: ");
  sender->GetSerial()->print(hipyMotor->atPosition()); Serial.print(" ,"); 
  sender->GetSerial()->print(hipxMotor->atPosition()); Serial.print(" ,"); 
  sender->GetSerial()->println(kneeMotor->atPosition());
}

// TARGET X Y Z
void cmd_target(SerialCommands* sender) {
  char* x_str = sender->Next();
	if (x_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}

  char* y_str = sender->Next();
	if (y_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}

  char* z_str = sender->Next();
	if (z_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}

  Point p;
  p.x = atoi(x_str);
  p.y = atoi(y_str);
  p.z = atoi(z_str);

  leg->moveToXYZ(p.x, p.y, p.z);

  JointAngles j;

  LegIKModel model = LegIKModel(108, 132, 15, 60);
  model.getJointAnglesFromVectors(&p, 1, &j);

  sender->GetSerial()->print("Target: (");
  sender->GetSerial()->print(p.x); sender->GetSerial()->print(","); 
  sender->GetSerial()->print(p.y); sender->GetSerial()->print(","); 
  sender->GetSerial()->print(p.z); sender->GetSerial()->println(")");
  sender->GetSerial()->print("Angles: ("); 
  sender->GetSerial()->print(degrees(j.hy)); sender->GetSerial()->print(",");
  sender->GetSerial()->print(degrees(j.hx)); sender->GetSerial()->print(",");
  sender->GetSerial()->print(degrees(j.k)); sender->GetSerial()->println(")");
}


SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
SerialCommand cmd_mode_("MODE", cmd_mode);
SerialCommand cmd_hipx_("HIPX", cmd_hipx);
SerialCommand cmd_hipy_("HIPY", cmd_hipy);
SerialCommand cmd_knee_("KNEE", cmd_knee);
SerialCommand cmd_status_("STATUS", cmd_status);
SerialCommand cmd_target_("TARGET", cmd_target);
SerialCommand cmd_home_("HOME", cmd_home);

void setup() 
{
	Serial.begin(57600);

	serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_servo_enable_);
  serial_commands_.AddCommand(&cmd_mode_);
  serial_commands_.AddCommand(&cmd_hipx_);
  serial_commands_.AddCommand(&cmd_hipy_);
  serial_commands_.AddCommand(&cmd_knee_);
  serial_commands_.AddCommand(&cmd_status_);
  serial_commands_.AddCommand(&cmd_target_);
  serial_commands_.AddCommand(&cmd_home_);

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

  capsule.minAngle = 0;
  capsule.maxAngle = 180;
  capsule.cmdAngle = 0;
  capsule.homeAngle = 90;

  shoulder.minAngle = 0;
  shoulder.maxAngle = 180;
  shoulder.cmdAngle = 0;
  shoulder.homeAngle = 130;

  knee.minAngle = 30;
  knee.maxAngle = 180;
  knee.cmdAngle = 0;
  knee.homeAngle = 50;

  servoController = new ServoController(3);
  hipyMotor = new ServoMotor(20, &capsule, servoController, pwm);
  hipxMotor = new ServoMotor(20, &shoulder, servoController, pwm);
  kneeMotor = new ServoMotor(20, &knee, servoController, pwm);

  servoController->addMotor(hipyMotor);
  servoController->addMotor(hipxMotor);
  servoController->addMotor(kneeMotor);

  leg = new LegController(108, 132, 15, 60, 40, hipyMotor, hipxMotor, kneeMotor, servoController);

  taskList = (ITask**)malloc(sizeof(ITask*) * NUM_TASKS);
  memset(taskList, 0, sizeof(ITask*) * NUM_TASKS);
  addTask(hipxMotor);
  addTask(hipyMotor);
  addTask(kneeMotor);
  addTask(leg);

  Serial.println("Ready!");
}

void loop() 
{
	serial_commands_.ReadSerial();
  updateTasks();
}