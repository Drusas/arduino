#ifndef _SPOT_STATE_H
#define _SPOT_STATE_H

enum BehaviorState {
    REST = 0,
    WALK = 1,
};

class State {

    float footLocations[3][4];

public:
    float horizontalVelocity[2];
    BehaviorState behaviorState;
    long ticks;
    
    float jointAngles[3][4];
    float height = -0.16;

    State();
    ~State();

    void printfootLocations();

    // locations[3][4]
    void setAllFootLocations(float locations[][4]);

    // legIdx [0-3], location float[3]
    void setFootLocation(int legIdx, float *location);

    // legIdx [0-3], location float[3]
    void getFootLocation(int legIdx, float *location);

    void getAllFootLocations(float dest[][4]);

    // legIdx [0-3]
    float getZPosition(int legIdx);
};

#endif