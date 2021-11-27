#ifndef _REST_SERVICE_TASK_H
#define _REST_SERVICE_TASK_H

#include "ArdTask.h"
#include "Quadruped.h"
#include "QuadrupedFsm.h"
#include "SpotFacade.h"
#include "State.h"
#include <map>

typedef void (*SpotFunction)(void);

class RestServiceTask : public ArdTask {

    static SpotFacade *spotFacade;
    static Quadruped *spot;
    static State *spotState;
    static void (*f)(void);
    static std::map<std::string, SpotFunction> commandMap;

public:
    RestServiceTask();
    void configure(int interval, SpotFacade *sf, Quadruped *q, State *s);
    void start();
    static boolean getConfigured();
    static void executeCommand(std::string command);
    static void getLegJointAngles();
    static void postLegJointAngles();
    static void postLegPosition();
    
protected:
    void performUpdate() override;
    

private:
    static void enable();
    static void disable();
    
    // void connectToWiFi();
    // void setupRouting();
    // void getModeOfOperation();
    // void postModeOfOperation();
    // void sendCrossOriginHeader();
    // void setCrossOrigin();
    // void getLegCoordinates();
    
    
    
    // void startJson(char *tag, const char *value);
    // void endJson();
    // void addJsonArray(char *tag, float x, float y, float z);
    
};

#endif