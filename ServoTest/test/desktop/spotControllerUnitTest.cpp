#include "gtest/gtest.h"
#include "Controller.h"

// using ::testing::_;
// using ::testing::NiceMock;
// using ::testing::Return;

class SpotControllerTest : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        // m_loopInToOut = new LoopInToOut(hal_mock);
    }

    virtual void TearDown() override
    {
        // delete m_loopInToOut;
    }

    Controller theController;
};

TEST_F(SpotControllerTest, FirstTest) {
    
}