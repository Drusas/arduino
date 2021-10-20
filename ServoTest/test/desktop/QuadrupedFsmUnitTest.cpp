#include "gtest/gtest.h"
#include "Disabled.h"
#include "Enabled.h"
#include "Idle.h"
#include "QuadrupedFsm.h"
#include "Standing.h"
#include "Walking.h"

FSM_INITIAL_STATE(QuadrupedFsm, Disabled);

class QuadrupedFsmTest : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        QuadrupedFsm::reset();
        QuadrupedFsm::start();
    }

    // virtual void TearDown() override
    // {
    // }
};

TEST_F(QuadrupedFsmTest, ConstructorTest) {
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());

    Enable event;
    QuadrupedFsm::dispatch(event);
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Enabled>());
}

TEST_F(QuadrupedFsmTest, EnabledTest) {
    Enable event;
    QuadrupedFsm::dispatch(event);
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Enabled>());

    QuadrupedFsm::dispatch(ToIdle());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Idle>());

    // cannot transition to walking from idle
    QuadrupedFsm::dispatch(ToWalk());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Idle>());

    QuadrupedFsm::dispatch(ToStand());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Standing>());

    QuadrupedFsm::dispatch(ToWalk());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Walking>());

    // cannot transition to idel from walking
    // must transition to standing first
    QuadrupedFsm::dispatch(ToIdle());
    EXPECT_FALSE(QuadrupedFsm::is_in_state<Idle>());

    QuadrupedFsm::dispatch(ToStand());
    QuadrupedFsm::dispatch(ToIdle());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Idle>());
}