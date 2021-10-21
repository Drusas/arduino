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

    QuadrupedFsm::dispatch(ToEnable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Enabled>());

    QuadrupedFsm::dispatch(ToDisable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());
}

TEST_F(QuadrupedFsmTest, StateTraversalTest) {
    //  States are;
    // DISABLED -> ENABLED
    // ENABLED -> DISABLED

    // All the following are within the enabled state.
    // Entering the enabled state sets the default state to idle.
    // By definition any of these states can transition to disabled.

    // IDLE -> STAND
    // STAND -> WALK
    // WALK -> STAND
    // STAND -> IDLE
    
    QuadrupedFsm::dispatch(ToEnable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Enabled>());

    QuadrupedFsm::dispatch(ToIdle());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Idle>());

    // can only transition to standing from idle
    // dispatching any other event should be ignored.
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

    QuadrupedFsm::dispatch(ToDisable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());
}

TEST_F(QuadrupedFsmTest, DisableTest) {
    
    // idle -> disabled
    QuadrupedFsm::dispatch(ToEnable());
    QuadrupedFsm::dispatch(ToIdle());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Idle>());
    QuadrupedFsm::dispatch(ToDisable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());

    // standing -> disabled
    QuadrupedFsm::dispatch(ToEnable());
    QuadrupedFsm::dispatch(ToIdle());
    QuadrupedFsm::dispatch(ToStand());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Standing>());
    QuadrupedFsm::dispatch(ToDisable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());

    // walking -> disabled
    QuadrupedFsm::dispatch(ToEnable());
    QuadrupedFsm::dispatch(ToIdle());
    QuadrupedFsm::dispatch(ToStand());
    QuadrupedFsm::dispatch(ToWalk());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Walking>());
    QuadrupedFsm::dispatch(ToDisable());
    EXPECT_TRUE(QuadrupedFsm::is_in_state<Disabled>());
}