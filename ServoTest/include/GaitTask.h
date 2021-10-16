#include "Controller.h"
#include "ITask.h"
#include "LegController.h"

class GaitTask : public ITask {
    Controller *controller;
    State *state;
    Command *command;
    float newLocations[3][4];
    float prevLocations[3][4];
    uint8_t footContacts[4];

    int taskCounter;

    LegController *RFLeg;
    LegController *LFLeg;
    LegController *RRLeg;
    LegController *LRLeg;

    void printNewLocation();
    bool areAnyLegBuffersFull();

protected:
    void performUpdate() override;

public:
    GaitTask(int interval, Controller *cntlr, State *ste, Command *cmd);
    void setLegs(LegController *RF, LegController *LF, LegController *RR, LegController *LR);
    void setEnabled(bool state) override;
};