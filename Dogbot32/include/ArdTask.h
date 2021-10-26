#ifndef _ARDTASK_H
#define _ARDTASK_H

#include <Arduino.h>
#include "..\..\lib\hal\ITask.h"

class ArdTask : public ITask {
protected:
    unsigned long milliSeconds() override {
        return millis();
    }

    virtual void performUpdate() override {};

public:
    void setEnabled(bool state) override {
       ITask::setEnabled(state);
    }
};

#endif