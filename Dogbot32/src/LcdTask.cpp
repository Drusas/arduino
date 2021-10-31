#include "LcdTask.h"

LcdTask::LcdTask() {}

void LcdTask::configure(queue<string> *q) {
    statusQueue = q;
}

void LcdTask::performUpdate() {
    if (statusQueue->empty()) {
        return;
    }

    
}