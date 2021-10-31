#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Wire.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int numBlinks;

void receiveEvent(int howMany) {
  // Serial.println("receive event");
  while (Wire.available()) {
    char c = Wire.read();
    numBlinks = c;
    Serial.print("received -> ");
    Serial.println(numBlinks);
  }
}

void requestEvent() {
  Serial.println("request event");
  delay(100);
  char* ptr = "Wire";
  Wire.write(ptr, 4);
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("hello, world!");

  Wire.begin(0x8);
  Wire.onReceive(receiveEvent);
  // Wire.onRequest(requestEvent);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}