#include "stdlib.h"
#include "TaskManager.h"
// #include "Utils.h"

//#define DEBUG_TASKMANAGER

TaskManager::TaskManager(uint8_t numTasks) :
  numManagedTasks(numTasks),
  managedTaskIdx(0) {
  taskList = (ITask**)malloc(sizeof(ITask*) * numManagedTasks);
}

TaskManager::~TaskManager() {
  free(taskList);
  taskList = 0;
}

void TaskManager::addTask(ITask* taskToAdd) {
  if (taskToAdd != 0) {
    // TRACE("%s, %d\n", "TaskManager::addTask", managedTaskIdx);
    taskList[managedTaskIdx++] = taskToAdd;
  }
  else {
    // TRACE("%s\n", "Tried to add null task.");
  }
}
 void TaskManager::setTasksEnabled(bool state) {
  //  TRACE("%s, %d, %d\n", "TaskManager::setTasksEnabled", state, managedTaskIdx);
   for (uint8_t i = 0; i < managedTaskIdx; i++) {
     taskList[i]->setEnabled(state);
   }
 }

 void TaskManager::updateTasks() {
   for (uint8_t i = 0; i < managedTaskIdx; i++) {
     if (taskList[i]->getEnabled()) {
#ifdef DEBUG_TASKMANAGER
        TRACE("%s%d\n", "updateTask", i);
#endif       
       taskList[i]->Update();
     }
     else {
#ifdef DEBUG_TASKMANAGER       
       TRACE("%s %d\n", "updateTask, task not enabled", i);
#endif
     }
   }
 }