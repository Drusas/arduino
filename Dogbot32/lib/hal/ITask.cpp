#include "ITask.h"

void ITask::Update() {
  if (!getEnabled()) {
    return;
  }

  if ((milliSeconds() - lastUpdate) > updateInterval)
  {
    lastUpdate = milliSeconds();
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