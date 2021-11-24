#ifndef _REST_SERVICE_TASK_H
#define _REST_SERVICE_TASK_H

#include "ArdTask.h"
#include "Quadruped.h"
#include "QuadrupedFsm.h"
#include "State.h"

class RestServiceTask : public ArdTask {
    

public:
    RestServiceTask();
    void configure(int interval, Quadruped *q, State *s);
    void start();
    boolean getConfigured();

protected:
    void performUpdate() override;

private:
    // void connectToWiFi();
    // void setupRouting();
    // void getModeOfOperation();
    // void postModeOfOperation();
    // void sendCrossOriginHeader();
    // void setCrossOrigin();
    // void getLegCoordinates();
    // void getLegJointAngles();
    // void postLegCoordinates();
    // void postLegJointAngles();
    // void startJson(char *tag, const char *value);
    // void endJson();
    // void addJsonArray(char *tag, float x, float y, float z);
    
};

#endif