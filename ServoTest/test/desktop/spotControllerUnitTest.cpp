#include "gtest/gtest.h"
#include "Controller.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

// using ::testing::_;
// using ::testing::NiceMock;
// using ::testing::Return;

class SpotControllerTest : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        state = new State();
        state->setAllFootLocations(theController.spotConfig->defaultStance);
        cmd = new Command();
    }

    virtual void TearDown() override
    {
        delete state;
        delete cmd;
    }

    Controller theController;
    State *state;
    Command *cmd;
};

TEST_F(SpotControllerTest, DISABLED_FirstTest) {
    float currentLocation[3] = {0.0, 0.0, 0.0};
    state->getFootLocation(0, currentLocation);
    float tol = 0.001;
    Configuration *cfg = theController.spotConfig;
    EXPECT_NEAR(currentLocation[0], cfg->defaultStance[0][0], tol);
    EXPECT_NEAR(currentLocation[1], cfg->defaultStance[1][0], tol);
    EXPECT_NEAR(currentLocation[2], cfg->defaultStance[2][0], tol);
}

TEST_F(SpotControllerTest, DISABLED_StepGaitPhaseContactTest) {

    /*
    Test that foot contacts are correct for each phase,
    See GaitController unit tests for more detail.
    */

    // cmd->horizontalVelocity[0] = 0.5;
    float newLocations[3][4];
    memset(newLocations, 0, 12*sizeof(float));
    uint8_t contacts[4]; 

    // phase 0
    for (int i = 0; i < 10; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        for (int i = 0; i < 4; i++) {
            EXPECT_EQ(contacts[i], 1);
        }

        EXPECT_EQ(state->ticks, i+1);
    }

    // phase 1
    for (int i = 10; i < 25; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        EXPECT_EQ(contacts[0], 0);
        // std::cout << "i: " << i << " contact: " << contacts[1] << std::endl;
        EXPECT_EQ(contacts[1], 1);
        EXPECT_EQ(contacts[2], 1);
        EXPECT_EQ(contacts[3], 1);

        EXPECT_EQ(state->ticks, i+1);
    }

    // phase 2
    for (int i = 25; i < 35; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        for (int i = 0; i < 4; i++) {
            EXPECT_EQ(contacts[i], 1);
        }

        EXPECT_EQ(state->ticks, i+1);
    }

    // phase 3
    for (int i = 35; i < 50; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        EXPECT_EQ(contacts[0], 1);
        // std::cout << "i: " << i << " contact: " << contacts[1] << std::endl;
        EXPECT_EQ(contacts[1], 0);
        EXPECT_EQ(contacts[2], 1);
        EXPECT_EQ(contacts[3], 1);

        EXPECT_EQ(state->ticks, i+1);
    }

    // phase 5
    state->ticks = 60;
    for (int i = 60; i < 75; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        EXPECT_EQ(contacts[0], 1);
        // std::cout << "i: " << i << " contact: " << contacts[1] << std::endl;
        EXPECT_EQ(contacts[1], 1);
        EXPECT_EQ(contacts[2], 0);
        EXPECT_EQ(contacts[3], 1);

        EXPECT_EQ(state->ticks, i+1);
    }

    // phase 7
    state->ticks = 85;
    for (int i = 85; i < 100; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        EXPECT_EQ(contacts[0], 1);
        // std::cout << "i: " << i << " contact: " << contacts[1] << std::endl;
        EXPECT_EQ(contacts[1], 1);
        EXPECT_EQ(contacts[2], 1);
        EXPECT_EQ(contacts[3], 0);

        EXPECT_EQ(state->ticks, i+1);
    }
}

void copyLocations(float src[][4], float dest[][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

TEST_F(SpotControllerTest, DISABLED_StepGaitMovingForwardTest) {

    /*
    Test that each foot location is correct when moving forward
    The swing foot should move forward in the x direction
    The stance feet should move backwards in the x direction
    */
    
    float newLocations[3][4];
    memset(newLocations, 0, 12*sizeof(float));
    float prevLocations[3][4];
    memset(prevLocations, 0, 12*sizeof(float));

    state->setAllFootLocations(prevLocations);
    // state->printFootLocations();
    uint8_t contacts[4]; 

    // we only move if the velocity in the given direction is <> 0
    cmd->horizontalVelocity[0] = 0.5;

    // phase 0
    // all feet are touching
    // all feet should have x positions that are decreasing
    for (int i = 0; i < 10; i++) {
        // std::cout << "i: " << i << std::endl;
        theController.stepGait(state, cmd, newLocations, contacts);
        for (int i = 0, j=0; j < 4; j++) {
            EXPECT_LT(newLocations[i][j], prevLocations[i][j]);
        }
        copyLocations(newLocations, prevLocations);
        // state->printFootLocations();
    }

    // phase 1
    // left front foot is swinging and should have x coord increasing
    // all other feet in stance and should be decreasing
    for (int i = 10; i < 25; i++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        EXPECT_GT(newLocations[0][0], prevLocations[0][0]);
        // std::cout << "i: " << i << " contact: " << contacts[1] << std::endl;
        EXPECT_LT(newLocations[0][1], prevLocations[0][1]);
        EXPECT_LT(newLocations[0][2], prevLocations[0][2]);
        EXPECT_LT(newLocations[0][3], prevLocations[0][3]);
    }
}

static float defaultStance[3][4] = {{   0,   0, -50, -50},
                                    {  60,  60,  60,  60},
                                    { 220, 220, 220, 220}};

TEST_F(SpotControllerTest, GenerateMotionProfilesTest) {
    float newLocations[3][4];
    memset(newLocations, 0, 12*sizeof(float));
    float prevLocations[3][4];
    memset(prevLocations, 0, 12*sizeof(float));

    state->setAllFootLocations(prevLocations);
    // state->printFootLocations();
    uint8_t contacts[4]; 

    // we only move if the velocity in the given direction is <> 0
    cmd->horizontalVelocity[0] = 0.5;

    size_t NUM_TICKS = 400;

    float xPositions[NUM_TICKS];
    float yPositions[NUM_TICKS];
    float zPositions[NUM_TICKS];

    for (int i = 0; i < NUM_TICKS; i ++) {
        theController.stepGait(state, cmd, newLocations, contacts);
        xPositions[i] = newLocations[0][0];
        yPositions[i] = newLocations[1][0];
        zPositions[i] = newLocations[2][0];

        xPositions[i] = defaultStance[0][1] + newLocations[0][0];
        yPositions[i] = defaultStance[1][1] - newLocations[1][0];
        zPositions[i] = defaultStance[2][1] - newLocations[2][0];
    }

    

    ofstream myfile ("gait.csv");
    if (myfile.is_open())
    {
        for (int i = 0; i < NUM_TICKS; i++) {
            myfile << std::fixed << std::setprecision(2);
            myfile << xPositions[i] << "," << yPositions[i] << "," << zPositions[i] << endl;
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file";
    }
}