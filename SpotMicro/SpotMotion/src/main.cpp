#include <Arduino.h>
#include "SerialCommands.h"
#include "Motion.h"
// #include "IKModel.h"

enum MotionMode {
  NONE = 0,
  MAN = 1,
  POSE = 2,
  WALK = 3,
  POINT = 4,
};

MotionMode motionMode = MotionMode::NONE;
ServoController* servoController;
Sweeper* hipyController;
Sweeper* hipxController;
Sweeper* kneeController;
LegController* leg;

Bone *femur, *tibia;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

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

  if (strcmp(mode_str, "MAN") == 0) {
    motionMode = MotionMode::MAN;
  } else if (strcmp(mode_str, "POSE") == 0) {
    motionMode = MotionMode::POSE;
  } else if (strcmp(mode_str, "WALK") == 0) {
    motionMode = MotionMode::WALK;
    leg->generateTrajectory(femur);
  } else if (strcmp(mode_str, "POINT") == 0) {
    motionMode = MotionMode::POINT;
  } else {
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

void cmd_hipx(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPX [ANGLE] ");
		return;
	}

	int angle = atoi(angle_str);
  hipxController->SetPosition(angle);
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPY [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  hipyController->SetPosition(angle);
}

void cmd_knee(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}
  
	int angle = atoi(angle_str);
  kneeController->SetPosition(angle);
}

void cmd_status(SerialCommands* sender) {
  
  sender->GetSerial()->print("CMDPOS: ");
  sender->GetSerial()->print(hipyController->cmdPosition()); Serial.print(" ,"); 
  sender->GetSerial()->print(hipxController->cmdPosition()); Serial.print(" ,"); 
  sender->GetSerial()->println(kneeController->cmdPosition());

  sender->GetSerial()->print("ATPOS: ");
  sender->GetSerial()->print(hipyController->atPosition()); Serial.print(" ,"); 
  sender->GetSerial()->print(hipxController->atPosition()); Serial.print(" ,"); 
  sender->GetSerial()->println(kneeController->atPosition());
}

// TARGET X Y
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

  Point target;
  target.x = atof(x_str);
  target.y = atof(y_str);
  for (int i = 0; i < 10; i++) {
    femur->updateIK(target);
  }

  femur->updateIK(target);

  sender->GetSerial()->print("Target: (");
  sender->GetSerial()->print(target.x); sender->GetSerial()->print(","); 
  sender->GetSerial()->print(target.y); sender->GetSerial()->print(") Angles: ("); 
  sender->GetSerial()->print(toDegrees(femur->getAngle())); sender->GetSerial()->print(",");
  sender->GetSerial()->print(toDegrees(tibia->getAngle())); sender->GetSerial()->println(")");
  sender->GetSerial()->print("End effector: (");
  
  Point p = femur->getEndOfChain();
  sender->GetSerial()->print(p.x); sender->GetSerial()->print(","); 
  sender->GetSerial()->print(p.y); sender->GetSerial()->println(")");  
}

SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
SerialCommand cmd_mode_("MODE", cmd_mode);
SerialCommand cmd_hipx_("HIPX", cmd_hipx);
SerialCommand cmd_hipy_("HIPY", cmd_hipy);
SerialCommand cmd_knee_("KNEE", cmd_knee);
SerialCommand cmd_status_("STATUS", cmd_status);
SerialCommand cmd_target_("TARGET", cmd_target);

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

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  servoController = new ServoController();

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

  shoulder.minAngle = 0;
  shoulder.maxAngle = 180;
  shoulder.cmdAngle = 0;

  knee.minAngle = 30;
  knee.maxAngle = 180;
  knee.cmdAngle = 0;

  hipyController = new Sweeper(20, &capsule, servoController);
  hipxController = new Sweeper(20, &shoulder, servoController);
  kneeController = new Sweeper(20, &knee, servoController);
  leg = new LegController(40, hipyController, hipxController, kneeController, servoController);
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

	Serial.println("Ready!");

  tibia = new Bone(108, 0, 0.0, 132, 0);
  femur = new Bone(0, 0, 0.0, 108, tibia);

  Point* points = CurveGenerator::GenerateCircle(160, 160, 25, 10);
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 15; j++) {
        femur->updateIK(points[i]);
      }
      Serial.print("Target: (");
      Serial.print(points[i].x); Serial.print(","); 
      Serial.print(points[i].y); Serial.print(")" );
      float shoulder = toDegrees(femur->getAngle());
      float knee = 180 - toDegrees(tibia->getAngle());
      uint8_t s = (uint8_t)shoulder;
      uint8_t k = (uint8_t)knee;
      Serial.print("Angles: (");
      Serial.print(s); Serial.print(","); 
      Serial.print(k); Serial.println(")");
    }
    delete points;
}

void loop() 
{
  
	serial_commands_.ReadSerial();
  if (motionMode == WALK) {
    leg->Update();
 }  
 
 if (motionMode == MAN || motionMode == WALK) {
    kneeController->Update();
    hipxController->Update();
    hipyController->Update();
  }
}