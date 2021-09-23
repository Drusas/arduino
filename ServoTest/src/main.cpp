#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "SerialCommands.h"

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

struct Joint {
  uint8_t servoIndex;
  uint16_t minPulse;
  uint16_t maxPulse;
  uint8_t minAngle;  
  uint8_t maxAngle;
  uint8_t cmdAngle;
  uint8_t homeAngle;
} hipy, hipx, knee;

bool enabled = 0;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
	sender->GetSerial()->print("Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

void cmd_servo_enable(SerialCommands* sender) {
  enabled = !enabled;
  sender->GetSerial()->print("SERVO ENABLE: ");
	sender->GetSerial()->println(enabled);
}

void cmd_hipx(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPX [ANGLE] ");
		return;
	}

	uint8_t angle = atoi(angle_str);
  hipx.cmdAngle = angle;
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HIPY [ANGLE] ");
		return;
	}
  
	uint8_t angle = atoi(angle_str);
  hipy.cmdAngle = angle;
}

void cmd_knee(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - KNEE [ANGLE] ");
		return;
	}
  
	uint8_t angle = atoi(angle_str);
  knee.cmdAngle = angle;
}

SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
// SerialCommand cmd_mode_("MODE", cmd_mode);
SerialCommand cmd_hipx_("HIPX", cmd_hipx);
SerialCommand cmd_hipy_("HIPY", cmd_hipy);
SerialCommand cmd_knee_("KNEE", cmd_knee);
// SerialCommand cmd_status_("STATUS", cmd_status);
// SerialCommand cmd_target_("TARGET", cmd_target);
// SerialCommand cmd_angle_("ANGLE", cmd_angle);
// SerialCommand cmd_home_("HOME", cmd_home);

void setup() {
  Serial.begin(57600);

	serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_servo_enable_);
  // serial_commands_.AddCommand(&cmd_mode_);
  serial_commands_.AddCommand(&cmd_hipx_);
  serial_commands_.AddCommand(&cmd_hipy_);
  serial_commands_.AddCommand(&cmd_knee_);
  // serial_commands_.AddCommand(&cmd_status_);
  // serial_commands_.AddCommand(&cmd_target_);
  // serial_commands_.AddCommand(&cmd_angle_);
  // serial_commands_.AddCommand(&cmd_home_);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);

  hipy.servoIndex = 4; // RR 10; // RF 5; // LR 11; // LF 4;
  hipy.minPulse = hipx.minPulse = knee.minPulse = SERVOMIN;
  hipy.maxPulse = hipx.maxPulse = knee.maxPulse = SERVOMAX;
  hipy.minAngle = 0;
  hipy.maxAngle = 180;
  hipy.cmdAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75
  hipy.homeAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75

  hipx.servoIndex = 2; // RR 12; // RF 3; // LR 13; // LF 2;
  hipx.minAngle = 0; // RR/LR 180; // LF/LR 0;
  hipx.maxAngle = 180; // LR/RR 0; // LF/LR 180;
  hipx.cmdAngle = 130; // RR 50; // RF 50 // LF/LR 130;
  hipx.homeAngle = 130; // LR/RR 50; // RF 50 // LF/LR 130;

  knee.servoIndex = 0; // RR 14; // RF 1 // LR 15; // LF 0;
  knee.minAngle = 30; // RF/RR 170; // LF/LR 30;
  knee.maxAngle = 0;
  knee.cmdAngle = 50; // RF/RR 170; // LF/LR 50;
  knee.homeAngle = 50; // RF/RR 170; // LF/LR 50;

  Serial.println("READY");
}

void loop() {
  serial_commands_.ReadSerial();

  if (enabled) {
    long pulseLength = map(hipy.cmdAngle, 0, 180, hipy.minPulse, hipy.maxPulse);
    pwm.setPWM(hipy.servoIndex, 0, pulseLength);

    pulseLength = map(hipx.cmdAngle, 0, 180, hipx.minPulse, hipx.maxPulse);
    pwm.setPWM(hipx.servoIndex, 0, pulseLength);

    pulseLength = map(knee.cmdAngle, 0, 180, knee.minPulse, knee.maxPulse);
    pwm.setPWM(knee.servoIndex, 0, pulseLength);
  }

  delay(500);
}