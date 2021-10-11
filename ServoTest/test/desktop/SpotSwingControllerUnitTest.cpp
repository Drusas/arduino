#include "gtest/gtest.h"
#include "GaitController.h"
#include "SwingController.h"

TEST(SpotSwingControllerTestSuite, ConstructorTest) {
  Configuration *spotConfig = new Configuration();
  SwingController *sc = new SwingController(spotConfig);
}

TEST(SpotSwingControllerTestSuite, SwingHeightTest) {
  /*
  When a foot is in the swing phase it follows a triangular motion profile
  
        -         ^
      -   -       |
    -       -     | z-height
  -           -   |

  time (ticks) ---> 

  The time period of the swing phase is normalized to [0.0, 1.0].
  The first half of the time period the foot is rising
  The second half of the time period the foot is falling
  At either end 0.0 or 1.0 the foot has not left he ground and height is zero
  0.5 is the peak height of the swing.
  */

  Configuration *spotConfig = new Configuration();
  SwingController *sc = new SwingController(spotConfig);

  float height = sc->getSwingHeight(1.0);
  std::cout << "HEIGHT: " << height << std::endl;
  float tolerance = 0.001;
  EXPECT_NEAR(height, 0.0, tolerance);

  // test for increasing values from 0.0 to 0.5, this is the rising side of the triangle
  float h1 = sc->getSwingHeight(0.0);
  float h2 = h1;
  for (float i = 0.1; i <= 0.5; i++) {
    h2 = sc->getSwingHeight(i);
    EXPECT_GT(h2, h1);
    h1 = h2;
  }

  // test falling side
  h1 = sc->getSwingHeight(0.5);
  h2 = h1;
  for (float i = 0.5; i <= 0.0; i++) {
    h2 = sc->getSwingHeight(i);
    EXPECT_LE(h2, h1);
    h1 = h2;
  }

  // midpoint of swing should be the highest point of the triangle.
  height = sc->getSwingHeight(0.5);
  EXPECT_NEAR(height, spotConfig->zClearance, tolerance);
}

TEST(SpotSwingControllerTestSuite, RaibertTouchdownLocationTest) {
  Configuration *spotConfig = new Configuration();
  SwingController *sc = new SwingController(spotConfig);
  Command *cmd = new Command();
  float touchDownLoc[3], defaultLoc[3];
  int legIdx = 0;
  defaultLoc[0] = spotConfig->defaultStance[0][legIdx];
  defaultLoc[1] = spotConfig->defaultStance[1][legIdx];
  defaultLoc[2] = spotConfig->defaultStance[2][legIdx];

  cmd->horizontalVelocity[0] = 0.5;  // move in positive x-axis direction
  sc->getRaibertTouchdownLocation(legIdx, cmd, touchDownLoc);
  float tolerance = 0.001;
  EXPECT_NEAR(touchDownLoc[2], 0.0, tolerance); // z is never touched by this method
  EXPECT_GT(touchDownLoc[0], defaultLoc[0]);
  EXPECT_NEAR(touchDownLoc[1], defaultLoc[1], tolerance);

  cmd->horizontalVelocity[1] = -0.25; // step to the right
  sc->getRaibertTouchdownLocation(legIdx, cmd, touchDownLoc);
  EXPECT_NEAR(touchDownLoc[2], 0.0, tolerance); // z is never touched by this method
  EXPECT_LT(touchDownLoc[1], defaultLoc[1]);
}

TEST(SpotSwingControllerTestSuite, NextFootLocationTest) {
  Configuration *spotConfig = new Configuration();
  SwingController *sc = new SwingController(spotConfig);
  Command *cmd = new Command();
  GaitController *gc = new GaitController(spotConfig);
  State *state = new State();

  // 1. Get location 1/4 of the way through the subphase
  float defaultLoc[3][4];
  memset(defaultLoc, 0, 12*sizeof(float));
  state->setAllFootLocations(defaultLoc);
  state->setAllFootLocations(spotConfig->defaultStance);
  cmd->horizontalVelocity[0] = 0.5;  // move in positive x-axis direction
  state->ticks = 0.25 * spotConfig->swingTicks;  // set time to 1/4 of swing phase
  float swingProp = (float)(gc->getSubPhaseTicks(state->ticks)) / (float)spotConfig->swingTicks;
  int legIdx = 3;
  float target[] = {0.0, 0.0, 0.0};
  sc->nextFootLocation(swingProp, legIdx, state, cmd, target);
  EXPECT_GT(target[0], spotConfig->defaultStance[0][legIdx]);
  //EXPECT_GT(nextTarget[1], target[1]);  // don't understand why y coord is changing??
  EXPECT_GT(target[2], 0.0);

  // 2. Get location 1/2 of the way through the subphase
  float nextTarget[] = {0.0, 0.0, 0.0};
  state->ticks += 0.25 * spotConfig->swingTicks;  // set time to 1/4 of swing phase
  swingProp = (float)(gc->getSubPhaseTicks(state->ticks)) / (float)spotConfig->swingTicks;
  legIdx = 0;
  cmd->horizontalVelocity[0] = 0.5;
  cmd->horizontalVelocity[1] = 0.0;
  sc->nextFootLocation(swingProp, legIdx, state, cmd, nextTarget);

  EXPECT_GT(nextTarget[0], spotConfig->defaultStance[0][legIdx]);
  //EXPECT_GT(nextTarget[1], target[1]);  // don't understand why y coord is changing??
  EXPECT_GT(nextTarget[2], target[2]);

  // 3. Get location 3/4 of the way through the subphase
  target[0] = nextTarget[0];
  target[1] = nextTarget[1];
  target[2] = nextTarget[2];
  state->ticks += 0.25 * spotConfig->swingTicks;  // set time to 1/4 of swing phase
  swingProp = (float)(gc->getSubPhaseTicks(state->ticks)) / (float)spotConfig->swingTicks;
  legIdx = 0;
  cmd->horizontalVelocity[0] = 0.5;
  cmd->horizontalVelocity[1] = 0.0;
  sc->nextFootLocation(swingProp, legIdx, state, cmd, nextTarget);

  EXPECT_GT(nextTarget[0], spotConfig->defaultStance[0][legIdx]);
  //EXPECT_GT(nextTarget[1], target[1]);  // don't understand why y coord is changing??
  EXPECT_LT(nextTarget[2], target[2]);  // passed the 1/2 way point of swing, z should not be decreasing
}

