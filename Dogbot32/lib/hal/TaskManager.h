#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

// #include <Arduino.h>
#include "ITask.h"

class TaskManager {
  uint8_t numManagedTasks;
  uint8_t managedTaskIdx;
  ITask ** taskList;

public:
  TaskManager(uint8_t numTasks);
  ~TaskManager();
  void addTask(ITask* taskToAdd);
  void setTasksEnabled(bool state);
  void updateTasks();
};

#endif