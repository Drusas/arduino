#include "ITask.h"

void ITask::Update() {
  if (!getEnabled()) {
    return;
  }

  if ((millis() - lastUpdate) > updateInterval)
  {
    lastUpdate = millis();
    performUpdate();
  }
}

void ITask::setEnabled(bool state) {
  enabled = state;
}

bool ITask::getEnabled() {
  return enabled;
}