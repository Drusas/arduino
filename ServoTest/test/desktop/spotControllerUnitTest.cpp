#include "gtest/gtest.h"

// using ::testing::_;
// using ::testing::NiceMock;
// using ::testing::Return;

class loopInToOut_Test : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        // m_loopInToOut = new LoopInToOut(hal_mock);
    }

    virtual void TearDown()
    {
        // delete m_loopInToOut;
    }

    // NiceMock<Hal_mock> hal_mock{};
    // LoopInToOut* m_loopInToOut{nullptr};
};