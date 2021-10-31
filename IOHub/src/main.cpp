#include <Arduino.h>
#include <LiquidCrystal.h>
#include "stdlib.h"
#include "Wire.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int numBlinks;
char readBuffer[BUFFER_LENGTH];
int readIdx = 0;

void receiveEvent(int howMany) {
  // Serial.println("receive event");
  while (Wire.available()) {
    Serial.println("receive event");
  //   char c = Wire.read();
  //   numBlinks = c;
  //   Serial.print("received -> ");
  //   Serial.println(numBlinks);

    readBuffer[readIdx++] = Wire.read();
    if (readIdx == BUFFER_LENGTH) {
      readIdx = 0;
    }

    if (readIdx < BUFFER_LENGTH - 2) {
      readBuffer[++readIdx] = '\0';
    }
    else {
      readBuffer[BUFFER_LENGTH - 1] = '\0';
    }

    printf("%s\n", readBuffer);
    lcd.setCursor(0, 0);
    lcd.write(readBuffer);
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