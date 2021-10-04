#include "gtest/gtest.h"
#include "State.h"

TEST(SpotStateTestSuite, ConstructorTest) {
    State state;
    EXPECT_EQ(state.behaviorState, BehaviorState::REST);
    
    float tolerance = 0.001;
    float location[3];

    for (int j = 0; j < 4; j++) {
        state.getFootLocation(j, location);
        for (int i = 0; i < 3; i++) {
            EXPECT_NEAR(location[i], 0.0, tolerance);
        }

        EXPECT_NEAR(state.getZPosition(j), 0, tolerance);
    }
}

TEST(SpotStateTestSuite, FootLocationsTest) {
    State state;
    float loc1[3] = { 1.1, 1.2, 1.3 };
    float loc2[3] = { 2.1, 2.2, 2.3 };
    float loc3[3] = { 3.1, 3.2, 3.3 };
    float loc4[3] = { 4.1, 4.2, 4.3 };

    state.setFootLocation(0, loc1);
    state.setFootLocation(1, loc2);
    state.setFootLocation(2, loc3);
    state.setFootLocation(3, loc4);

    float tolerance = 0.001;

    float rLoc[3];
    state.getFootLocation(0, rLoc);
    for (int i = 0; i < 3; i++) {
        float expectedResult = 1 + ((i+1) * 0.1);
        EXPECT_NEAR(rLoc[i], expectedResult, tolerance);
    }

    state.getFootLocation(3, rLoc);
    for (int i = 0; i < 3; i++) {
        float expectedResult = 4 + ((i+1) * 0.1);
        EXPECT_NEAR(rLoc[i], expectedResult, tolerance);
    }

    tolerance = 0.1;
    float allFeet[3][4] = {{1, 2, 3, 4},
                           {2, 4, 6, 8},
                           {3, 6, 9, 12}};
    state.setAllFootLocations(allFeet);
    for (int j = 0; j < 4; j++) {
        state.getFootLocation(j, rLoc);
        for (int i = 0; i < 3; i++) {
            float expectedResult = (j+1)*(i+1);
            EXPECT_NEAR(rLoc[i], expectedResult, tolerance);
        }
    }

    // test getting just z position
    for (int j = 0; j < 4; j++) {
        float expectedResult = (j+1)*3;
        EXPECT_NEAR(state.getZPosition(j), expectedResult, tolerance);
    }
}