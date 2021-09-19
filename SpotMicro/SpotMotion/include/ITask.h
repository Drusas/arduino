// Implementation of Arduino multi-tasking defined here: https://learn.adafruit.com/multi-tasking-the-arduino-part-1
// Inheritors 
//    1. Implement ITask::performUpdate
//    2. Set update interval
//    3. Call Update() in main loop()

#ifndef _ITASK_H
#define _ITASK_H
#include <Arduino.h>

class ITask {
protected:
  bool enabled;
  int  updateInterval;      
  unsigned long lastUpdate;
  virtual void performUpdate() = 0;
public:
  virtual void Update();
  virtual void setEnabled(bool state);
  virtual bool getEnabled();
};

#endif