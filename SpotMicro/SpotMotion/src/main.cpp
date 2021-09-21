#include <Arduino.h>
#include "SerialCommands.h"
#include "LegController.h"
#include "IKModel.h"
#include "ServoController.h"
#include "ServoMotor.h"

#define DEBUG_MAIN 0

#define NUM_TASKS 20
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

enum JointIndex {
  HIPY = 0,
  HIPX = 1,
  KNEE = 2,
};

Joint jointsLF[3];
Joint jointsLR[3];
Joint jointsRF[3];
Joint jointsRR[3];

Joint hipy, hipx, knee;
MotionMode motionMode = MotionMode::NONE;
IServoController* servoController;

ServoMotor** servosLF;
ServoMotor** servosLR;
ServoMotor** servosRF;
ServoMotor** servosRR;

ServoMotor* hipyMotor;
ServoMotor* hipxMotor;
ServoMotor* kneeMotor;

// todo remove
LegController* leg;
// todo remove

enum LegIndex {
  LF = 0,
  LR = 1,
  RF = 2,
  RR = 3,
};

LegController* legLF;
LegController* legLR;
LegController* legRF;
LegController* legRR;

LegController** legList;



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
  hipxMotor->setPosition((angle));
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPY [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  hipyMotor->setPosition((angle));
}

void cmd_knee(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  kneeMotor->setPosition((angle));
}

// LEG XX JX ANGLE (E.G. LEG LF HIPX 90)
void cmd_leg(SerialCommands* sender) {
  char* leg_idx_str = sender->Next();
	if (leg_idx_str == NULL) {
		sender->GetSerial()->println("ERROR NO LEG IDX");
		return;
	}

  char* joint_idx_str = sender->Next();
	if (joint_idx_str == NULL) {
		sender->GetSerial()->println("ERROR NO JOINT IDX");
		return;
	}

  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE");
		return;
	}

  uint8_t legIdx = atoi(leg_idx_str);
  uint8_t jointIdx = atoi(joint_idx_str);
  uint8_t angle = atoi(angle_str);

  legList[legIdx]->getJoint(jointIdx)->setPosition(angle);
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
SerialCommand cmd_leg_("LEG", cmd_leg);

void configureCommands() {
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_servo_enable_);
  serial_commands_.AddCommand(&cmd_mode_);
  serial_commands_.AddCommand(&cmd_hipx_);
  serial_commands_.AddCommand(&cmd_hipy_);
  serial_commands_.AddCommand(&cmd_knee_);
  serial_commands_.AddCommand(&cmd_status_);
  serial_commands_.AddCommand(&cmd_target_);
  serial_commands_.AddCommand(&cmd_home_);

  Serial.println("configureCommands COMPLETE");
}

void configureJoints() {
  
  // LEFT FRONT
  jointsLF[HIPY].servoIndex = 4;
  jointsLF[HIPY].minAngle = 0;
  jointsLF[HIPY].maxAngle = 180;
  jointsLF[HIPY].homeAngle = 90;

  jointsLF[HIPX].servoIndex = 2;
  jointsLF[HIPX].minAngle = 0;
  jointsLF[HIPX].maxAngle = 180;
  jointsLF[HIPX].homeAngle = 130;

  jointsLF[KNEE].servoIndex = 0;
  jointsLF[KNEE].minAngle = 30;
  jointsLF[KNEE].maxAngle = 180;
  jointsLF[KNEE].homeAngle = 150;

  // LEFT REAR
  jointsLR[HIPY].servoIndex = 11;
  jointsLR[HIPY].minAngle = 0;
  jointsLR[HIPY].maxAngle = 180;
  jointsLR[HIPY].homeAngle = 90;

  jointsLR[HIPX].servoIndex = 13;
  jointsLR[HIPX].minAngle = 0;
  jointsLR[HIPX].maxAngle = 180;
  jointsLR[HIPX].homeAngle = 130;

  jointsLR[KNEE].servoIndex = 15;
  jointsLR[KNEE].minAngle = 30;
  jointsLR[KNEE].maxAngle = 180;
  jointsLR[KNEE].homeAngle = 150;

  // RIGHT FRONT
  jointsRF[HIPY].servoIndex = 5;
  jointsRF[HIPY].minAngle = 180;
  jointsRF[HIPY].maxAngle = 0;
  jointsRF[HIPY].homeAngle = 90;

  jointsRF[HIPX].servoIndex = 3;
  jointsRF[HIPX].minAngle = 180;
  jointsRF[HIPX].maxAngle = 0;
  jointsRF[HIPX].homeAngle = 50;

  jointsRF[KNEE].servoIndex = 1;
  jointsRF[KNEE].minAngle = 170;
  jointsRF[KNEE].maxAngle = 0;
  jointsRF[KNEE].homeAngle = 170;

  // RIGHT REAR
  jointsRR[HIPY].servoIndex = 10;
  jointsRR[HIPY].minAngle = 180;
  jointsRR[HIPY].maxAngle = 0;
  jointsRR[HIPY].homeAngle = 90;

  jointsRR[HIPX].servoIndex = 12;
  jointsRR[HIPX].minAngle = 180;
  jointsRR[HIPX].maxAngle = 0;
  jointsRR[HIPX].homeAngle = 50;

  jointsRR[KNEE].servoIndex = 14;
  jointsRR[KNEE].minAngle = 180;
  jointsRR[KNEE].maxAngle = 0;
  jointsRR[KNEE].homeAngle = 170;

  Serial.println("configureJoints COMPLETE");
}

void configureServos() {
  servoController = new ServoController(3);

  servosLF = (ServoMotor**) malloc(sizeof(ServoMotor*) * 3);
  servosLF[HIPY] = new ServoMotor(20, &jointsLF[HIPY], servoController, pwm);
  servosLF[HIPX] = new ServoMotor(20, &jointsLF[HIPX], servoController, pwm);
  servosLF[KNEE] = new ServoMotor(20, &jointsLF[KNEE], servoController, pwm);

  servosLR = (ServoMotor**) malloc(sizeof(ServoMotor*) * 3);
  servosLR[HIPY] = new ServoMotor(20, &jointsLR[HIPY], servoController, pwm);
  servosLR[HIPX] = new ServoMotor(20, &jointsLR[HIPX], servoController, pwm);
  servosLR[KNEE] = new ServoMotor(20, &jointsLR[KNEE], servoController, pwm);

  servosRF = (ServoMotor**) malloc(sizeof(ServoMotor*) * 3);
  servosRF[HIPY] = new ServoMotor(20, &jointsRF[HIPY], servoController, pwm);
  servosRF[HIPX] = new ServoMotor(20, &jointsRF[HIPX], servoController, pwm);
  servosRF[KNEE] = new ServoMotor(20, &jointsRF[KNEE], servoController, pwm);

  servosRR = (ServoMotor**) malloc(sizeof(ServoMotor*) * 3);
  servosRR[HIPY] = new ServoMotor(20, &jointsRR[HIPY], servoController, pwm);
  servosRR[HIPX] = new ServoMotor(20, &jointsRR[HIPX], servoController, pwm);
  servosRR[KNEE] = new ServoMotor(20, &jointsRR[KNEE], servoController, pwm);

  servoController->addMotor(servosLF[HIPY]);
  servoController->addMotor(servosLF[HIPX]);
  servoController->addMotor(servosLF[KNEE]);

  servoController->addMotor(servosLR[HIPY]);
  servoController->addMotor(servosLR[HIPX]);
  servoController->addMotor(servosLR[KNEE]);

  servoController->addMotor(servosRF[HIPY]);
  servoController->addMotor(servosRF[HIPX]);
  servoController->addMotor(servosRF[KNEE]);

  servoController->addMotor(servosRR[HIPY]);
  servoController->addMotor(servosRR[HIPX]);
  servoController->addMotor(servosRR[KNEE]);

  // todo remove
  hipyMotor = new ServoMotor(20, &hipy, servoController, pwm);
  hipxMotor = new ServoMotor(20, &hipx, servoController, pwm);
  kneeMotor = new ServoMotor(20, &knee, servoController, pwm);

  servoController->addMotor(hipyMotor);
  servoController->addMotor(hipxMotor);
  servoController->addMotor(kneeMotor);
  // todo remove

  Serial.println("configureServos COMPLETE");
}

void configureLegs() {

  legLF = new LegController(108, 132, 15, 60, 40, servosLF[HIPY], servosLF[HIPX], servosLF[KNEE], servoController);
  legLR = new LegController(108, 132, 15, 60, 40, servosLR[HIPY], servosLR[HIPX], servosLR[KNEE], servoController);
  legRF = new LegController(108, 132, 15, 60, 40, servosRF[HIPY], servosRF[HIPX], servosRF[KNEE], servoController);
  legRR = new LegController(108, 132, 15, 60, 40, servosRR[HIPY], servosRR[HIPX], servosRR[KNEE], servoController);

  legList = (LegController**) (malloc(sizeof(LegController*) * 4));
  legList[LF] = legLF;
  legList[LR] = legLR;
  legList[RF] = legRF;
  legList[RR] = legRR;

  // todo remove
  leg = new LegController(108, 132, 15, 60, 40, hipyMotor, hipxMotor, kneeMotor, servoController);
  // todo remove

  Serial.println("configureLegs COMPLETE");
}

void configureTasks() {
  taskList = (ITask**)malloc(sizeof(ITask*) * NUM_TASKS);
  memset(taskList, 0, sizeof(ITask*) * NUM_TASKS);
  
  addTask(servosLF[HIPY]);
  addTask(servosLF[HIPX]);
  addTask(servosLF[KNEE]);

  addTask(servosLR[HIPY]);
  addTask(servosLR[HIPX]);
  addTask(servosLR[KNEE]);

  addTask(servosRF[HIPY]);
  addTask(servosRF[HIPX]);
  addTask(servosRF[KNEE]);

  addTask(servosRR[HIPY]);
  addTask(servosRR[HIPX]);
  addTask(servosRR[KNEE]);

  addTask(legLF);
  addTask(legLR);
  addTask(legRF);
  addTask(legRR);


  // todo remove
  addTask(hipxMotor);
  addTask(hipyMotor);
  addTask(kneeMotor);
  addTask(leg);
  // todo remove
}

void setup() 
{
	Serial.begin(57600);
  
  configureCommands();

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  configureJoints();
  configureServos();
  // configureLegs();
  // configureTasks();
  
  Serial.println("Ready!");
}

void loop() 
{
	serial_commands_.ReadSerial();
  // updateTasks();
}