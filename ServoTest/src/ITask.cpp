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

void ITask::setId(std::string name) {
  id = name;
}

std::string ITask::getId() {
  return id;
}