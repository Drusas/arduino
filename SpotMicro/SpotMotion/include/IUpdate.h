#ifndef _IUPDATE_H
#define _IUPDATE_H
#include <Arduino.h>

class IUpdate {
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