#include "gtest/gtest.h"
#include "StanceController.h"

TEST(SpotStanceControllerTestSuite, ConstructorTest) {
  Configuration *spotConfig = new Configuration();
  StanceController *sc = new StanceController(spotConfig); 
}

TEST(SpotStanceControllerTestSuite, GetPositionDeltaTest) {
  Configuration *spotConfig = new Configuration();
  StanceController *sc = new StanceController(spotConfig); 

  State *state = new State();
  Command *cmd = new Command();
  float delta[3] = {0.0, 0.0, 0.0};

  int legIdx = 0;
  sc->getPositionDelta(legIdx, state, cmd, delta);
  float tolerance = 0.001;
  EXPECT_NEAR(delta[0], 0.0, tolerance);
  EXPECT_NEAR(delta[1], 0.0, tolerance);
  // EXPECT_NEAR(delta[2], 0.0, tolerance); // todo - why does this change?

  // If we're moving/stepping forward then feet on the ground need to move
  // in the opposite direction to move the body forward
  // we're moving forward so delta x should be negative
  cmd->horizontalVelocity[0] = 1.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  sc->getPositionDelta(legIdx, state, cmd, delta);
  // std::cout << "x: " << delta[0] << ", y: " << delta[1] << ", z: " << delta[2] << std::endl;
  EXPECT_LT(delta[0], 0.0);
  EXPECT_NEAR(delta[1], 0.0, tolerance);  // y should not have 

  // we're moving bakcwards so delta x should be postive
  cmd->horizontalVelocity[0] = -1.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  sc->getPositionDelta(legIdx, state, cmd, delta);
  // std::cout << "x: " << delta[0] << ", y: " << delta[1] << ", z: " << delta[2] << std::endl;
  EXPECT_GT(delta[0], 0.0);
  EXPECT_NEAR(delta[1], 0.0, tolerance);  // y should not have 
}

TEST(SpotStanceControllerTestSuite, NextFootLocationTest) {
  Configuration *spotConfig = new Configuration();
  StanceController *sc = new StanceController(spotConfig); 

  State *state = new State();
  Command *cmd = new Command();
  int legIdx = 0;
  float location[3] = {0.0, 0.0, 0.0};
  state->setFootLocation(legIdx, location);
  
  // move forward
  // stance controller moves feet that are in contact with the ground
  // x location of new location should be less than starting location
  cmd->horizontalVelocity[0] = 0.5;
  cmd->horizontalVelocity[1] = 0.0;
  float nextLocation[3] = {0.0, 0.0, 0.0};
  sc->nextFootLocation(legIdx, state, cmd, nextLocation);
  // std::cout << "current:x: " << location[0] << " y: " << location[1] << " z: " << location[2] << std::endl;
  // std::cout << "next:x: " << nextLocation[0] << " y: " << nextLocation[1] << " z: " << nextLocation[2] << std::endl;
  EXPECT_LT(nextLocation[0], location[0]);
  std::cout << std::endl;

  // move left ? (todo, left handedness?)
  state->setFootLocation(legIdx, nextLocation);
  cmd->horizontalVelocity[0] = 0.0;
  cmd->horizontalVelocity[1] = -0.5;
  nextLocation[0] = nextLocation[1] = nextLocation[2] = 0.0;
  state->getFootLocation(legIdx, location);
  sc->nextFootLocation(legIdx, state, cmd, nextLocation);
  // std::cout << "current:x: " << location[0] << " y: " << location[1] << " z: " << location[2] << std::endl;
  // std::cout << "next:x: " << nextLocation[0] << " y: " << nextLocation[1] << " z: " << nextLocation[2] << std::endl;
  float tolerance = 0.001;
  EXPECT_NEAR(nextLocation[0], location[0], tolerance);  
  EXPECT_GT(nextLocation[1], location[1]);
  std::cout << std::endl;

  // move right and backwards
  // remember move backwards in x moves the contact feet forward in x
  // so x pos should be greater than previous
  state->setFootLocation(legIdx, nextLocation);
  cmd->horizontalVelocity[0] = -0.5;
  cmd->horizontalVelocity[1] = 0.5;
  nextLocation[0] = nextLocation[1] = nextLocation[2] = 0.0;
  state->getFootLocation(legIdx, location);
  sc->nextFootLocation(legIdx, state, cmd, nextLocation);
  tolerance = 0.001;
  // std::cout << "current:x: " << location[0] << " y: " << location[1] << " z: " << location[2] << std::endl;
  // std::cout << "next:x: " << nextLocation[0] << " y: " << nextLocation[1] << " z: " << nextLocation[2] << std::endl;
  EXPECT_GT(nextLocation[0], location[0]);  
  EXPECT_LT(nextLocation[1], location[1]);
  std::cout << std::endl;
}

TEST(SpotStanceControllerTestSuite, NextZFootLocationTest) {
  std::cout << "NEXT Z POS TEST" << std::endl;
  
  Configuration *spotConfig = new Configuration();
  StanceController *sc = new StanceController(spotConfig); 

  State *state = new State();
  Command *cmd = new Command();
  int legIdx = 0;
  float location[3] = {0.0, 0.0, 0.0};
  state->setFootLocation(legIdx, location);

  cmd->horizontalVelocity[0] = 0.0;
  cmd->horizontalVelocity[1] = 0.0;
  float nextLocation[3] = {0.0, 0.0, 0.0};
  sc->nextFootLocation(legIdx, state, cmd, nextLocation);
  // std::cout << "current:z: " << location[2] << std::endl;
  // std::cout << "next:z: " << nextLocation[2] << std::endl;

  for (int i = 0; i < 16; i++) {
    // std::cout << std::endl;
    state->setFootLocation(legIdx, nextLocation);
    sc->nextFootLocation(legIdx, state, cmd, nextLocation);
    // std::cout << "current:z: " << location[2] << std::endl;
    // std::cout << "next:z: " << nextLocation[2] << std::endl;
  }
}