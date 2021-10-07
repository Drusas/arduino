#include "gtest/gtest.h"
#include "Configuration.h"
#include "GaitController.h"

TEST(SpotGaitControllerTestSuite, FootContactsTest) {
    // Spot uses an eight phase gate
    // each gait is defined by feet that are on the ground and feet in the air
    // transitions to each gate are based on elapsed time (ticks)
    
    Configuration *spotConfig = new Configuration();
    GaitController *gc = new GaitController(spotConfig);
    uint8_t contacts[4];
    
    // 0: all feet touching
    gc->getFootContacts(9, contacts);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(contacts[i], 1);
    }

    // 1: right front swing
    gc->getFootContacts(11, contacts);
    EXPECT_EQ(contacts[0], 0);
    EXPECT_EQ(contacts[1], 1);
    EXPECT_EQ(contacts[2], 1);
    EXPECT_EQ(contacts[3], 1);

    // 2: all feet touching
    gc->getFootContacts(25, contacts);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(contacts[i], 1);
    }

    // 3: LEFT front swing
    gc->getFootContacts(35, contacts);
    EXPECT_EQ(contacts[0], 1);
    EXPECT_EQ(contacts[1], 0);
    EXPECT_EQ(contacts[2], 1);
    EXPECT_EQ(contacts[3], 1);

    // 4: all feet touching
    gc->getFootContacts(50, contacts);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(contacts[i], 1);
    }

    // 5: Right rear swing
    gc->getFootContacts(60, contacts);
    EXPECT_EQ(contacts[0], 1);
    EXPECT_EQ(contacts[1], 1);
    EXPECT_EQ(contacts[2], 0);
    EXPECT_EQ(contacts[3], 1);

    // 6: all feet touching
    gc->getFootContacts(75, contacts);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(contacts[i], 1);
    }

    // 7: Left rear swing
    gc->getFootContacts(85, contacts);
    EXPECT_EQ(contacts[0], 1);
    EXPECT_EQ(contacts[1], 1);
    EXPECT_EQ(contacts[2], 1);
    EXPECT_EQ(contacts[3], 0);

    // 0: all feet touching
    gc->getFootContacts(100, contacts);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(contacts[i], 1);
    }
}

TEST(SpotGaitControllerTestSuite, PhaseIndexTest) {
    /* 
    We want to test that we return the proper index into the gait array.
    There are 8 phases to the gait controller; 
        - 4 phases for each leg as it is swung through the air (a.k.a. "a step")
        - 4 phases, one between each swing when all 4 feet are on the ground (a.k.a standing).
    
    For the Spot Mini configuration swing phases are 15 ticks each and standing phases are 10 ticks each
    Each phase swing phases are interweaved with standing phases
    */
    Configuration *spotConfig = new Configuration();
    // std::cout << "spotConfig->phaseLength: " << spotConfig->phaseLength << std::endl; // 4 * 15 + 4 * 10 == 100
    GaitController *gc = new GaitController(spotConfig);
    
    // 0 -> 9 ticks are phase 0
    EXPECT_EQ(gc->getPhaseIndex(0), 0);
    EXPECT_EQ(gc->getPhaseIndex(7), 0);
    EXPECT_EQ(gc->getPhaseIndex(205), 0);

    // 10 -> 24 are phase 1
    EXPECT_EQ(gc->getPhaseIndex(10), 1);
    EXPECT_EQ(gc->getPhaseIndex(19), 1);
    EXPECT_EQ(gc->getPhaseIndex(323), 1);

    // 25 -> 34 are phase 2
    EXPECT_EQ(gc->getPhaseIndex(25), 2);
    EXPECT_EQ(gc->getPhaseIndex(33), 2);
    EXPECT_EQ(gc->getPhaseIndex(1033), 2);

    // 85 -> 99 are phase 7
    EXPECT_EQ(gc->getPhaseIndex(85), 7);
    EXPECT_EQ(gc->getPhaseIndex(3398), 7);
}

TEST(SpotGaitControllerTestSuite, GetSubPhaseTicksTest) {
    /*
    Within any phase (swing or stand) we want to know how many ticks have passed
    since the start of that phase.
    This information is used with the SwingController to calcualte the height of the foot
    from the ground during the phase.  This is used to generate a triangular profile.
    */

    Configuration *spotConfig = new Configuration();
    GaitController *gc = new GaitController(spotConfig);

    // 25 -> 34 are phase 2
    int subTicks = gc->getSubPhaseTicks(34);
    EXPECT_EQ(subTicks, 9);

    // 85 -> 99 are phase 7
    subTicks = gc->getSubPhaseTicks(3389);
    EXPECT_EQ(subTicks, 4);
}