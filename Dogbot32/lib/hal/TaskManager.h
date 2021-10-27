#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#include "ITask.h"

class TaskManager {
  
public:
    const static uint8_t MAX_NUM_TASKS = 255;
    TaskManager();
    ~TaskManager();
    void addTask(ITask* taskToAdd);
    void setTasksEnabled(bool state);
    void updateTasks();

private:
    uint8_t numManagedTasks;
    uint8_t managedTaskIdx;
    ITask *taskList[MAX_NUM_TASKS];
};

#endif