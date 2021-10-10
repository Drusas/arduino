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
  std::cout << "x: " << delta[0] << ", y: " << delta[1] << ", z: " << delta[2] << std::endl;
  EXPECT_LT(delta[0], 0.0);
  EXPECT_NEAR(delta[1], 0.0, tolerance);  // y should not have 

  // we're moving bakcwards so delta x should be postive
  cmd->horizontalVelocity[0] = -1.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  sc->getPositionDelta(legIdx, state, cmd, delta);
  std::cout << "x: " << delta[0] << ", y: " << delta[1] << ", z: " << delta[2] << std::endl;
  EXPECT_GT(delta[0], 0.0);
  EXPECT_NEAR(delta[1], 0.0, tolerance);  // y should not have 
}