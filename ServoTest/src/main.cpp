#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "SerialCommands.h"
#include "ServoController.h"
#include "ServoMotor.h"

#if defined NDEBUG
    #define TRACESC(format, ...)
    #define TRACE(format, ...)
#else
    #define TRACESC(format, ...)   sender->GetSerial()->printf( "%s::%s(%d) " format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
    #define TRACE(format, ...)   Serial.printf( "%s::%s(%d) " format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
#endif

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
IServoController* servoController;
ServoMotor* servoMotor;

Joint hipy, hipx, knee;

bool enabled = 0;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
  TRACESC("Unrecognized command [%s]\n", cmd);
}

void cmd_servo_enable(SerialCommands* sender) {
  enabled = !enabled;
  servoController->setEnabled(enabled);
  servoMotor->setEnabled(enabled);
  TRACESC("SERVO ENABLE: %d\n", enabled);
}

void cmd_hipx(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HX [ANGLE] ");
		return;
	}

	uint8_t angle = atoi(angle_str);
  hipx.cmdAngle = angle;
}

void cmd_hipy(SerialCommands* sender) {
  char* angle_str = sender->Next();
	if (angle_str == NULL) {
		sender->GetSerial()->println("ERROR NO ANGLE - HY [ANGLE] ");
		return;
	}
  
	uint8_t angle = atoi(angle_str);
  hipy.cmdAngle = angle;
  servoMotor->setPosition(angle);
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

void configureJoints()
{
  hipx.servoIndex = 4; // RR 10; // RF 5; // LR 11; // LF 4;
  hipx.minPulse = hipy.minPulse = knee.minPulse = SERVOMIN;
  hipx.maxPulse = hipy.maxPulse = knee.maxPulse = SERVOMAX;
  hipx.minAngle = 0;
  hipx.maxAngle = 180;
  hipx.cmdAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75
  hipx.homeAngle = 75; // RR 125; // RF 105; // LR 50; // LF 75

  hipy.servoIndex = 2; // RR 12; // RF 3; // LR 13; // LF 2;
  hipy.minAngle = 0; // RR/RF 180; // LF/LR 0;
  hipy.maxAngle = 180; // RR/RF 0; // LF/LR 180;
  hipy.cmdAngle = 130; // RR 50; // RF 50 // LF/LR 130;
  hipy.homeAngle = 130; // RF/RR 50; // LF/LR 130;

  knee.servoIndex = 0; // RR 14; // RF 1 // LR 15; // LF 0;
  knee.minAngle = 30; // RF/RR 170; // LF/LR 30;
  knee.maxAngle = 0;
  knee.cmdAngle = 50; // RF/RR 170; // LF/LR 50;
  knee.homeAngle = 50; // RF/RR 170; // LF/LR 50;

  Serial.println("configureJoints COMPLETE");
  TRACE("%s\n", "configureMotors COMPLETE");
}

void configureMotors() {
  servoController = new ServoController(1);
  servoMotor = new ServoMotor(20, &hipy, servoController, pwm);
  TRACE("%s\n", "configureMotors COMPLETE");
}

SerialCommand cmd_servo_enable_("ENABLE", cmd_servo_enable);
// SerialCommand cmd_mode_("MODE", cmd_mode);
SerialCommand cmd_hipx_("HX", cmd_hipx);
SerialCommand cmd_hipy_("HY", cmd_hipy);
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

  configureJoints();
  configureMotors();
 
  TRACE("%s", "Spot is ready!");
}

void loop() {
  serial_commands_.ReadSerial();
  servoMotor->Update();

  // if (enabled) {
    // long pulseLength = map(hipx.cmdAngle, 0, 180, hipx.minPulse, hipx.maxPulse);
    // pwm.setPWM(hipx.servoIndex, 0, pulseLength);

    // pulseLength = map(hipy.cmdAngle, 0, 180, hipy.minPulse, hipy.maxPulse);
    // pwm.setPWM(hipy.servoIndex, 0, pulseLength);
    // servoMotor->performUpdate();

    // pulseLength = map(knee.cmdAngle, 0, 180, knee.minPulse, knee.maxPulse);
    // pwm.setPWM(knee.servoIndex, 0, pulseLength);
  // }
}