#ifndef _LCD_TASK_H
#define _LCD_TASK_H

#include "ITask.h"
#include <queue>
#include <string>

using namespace std;

class LcdTask : ITask {
    queue<string> *statusQueue;

protected:
    void performUpdate() override;

public:
    LcdTask();
    void configure(queue<string> *q);
};

#endif