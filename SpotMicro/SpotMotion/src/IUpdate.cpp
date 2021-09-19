#include "IUpdate.h"

void IUpdate::Update() {
  if (!getEnabled()) {
    return;
  }
  
  if ((millis() - lastUpdate) > updateInterval)
  {
    lastUpdate = millis();
    performUpdate();
  }
}

void IUpdate::setEnabled(bool state) {
  enabled = state;
}

bool IUpdate::getEnabled() {
  return enabled;
}