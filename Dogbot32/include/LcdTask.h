#ifndef _LCD_TASK_H
#define _LCD_TASK_H

#include "ArdTask.h"
#include <queue>
#include <string>

using namespace std;

class LcdTask : public ArdTask {
    queue<string> *statusQueue;

    void helloWorld();

protected:
    void performUpdate() override;

public:
    LcdTask();
    void configure(int interval, queue<string> *q);
    void start();
};

#endif