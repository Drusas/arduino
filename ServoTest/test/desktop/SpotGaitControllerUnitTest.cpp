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
    
    // phase 1: all feet touching
    gc->getFootContacts(0, contacts);
    for (int i = 0; i < 4; i++) {
        std::cout << contacts[i] << ", ";
        EXPECT_EQ(contacts[i], (uint8_t)1);
    }
    std::cout << std::endl;
}