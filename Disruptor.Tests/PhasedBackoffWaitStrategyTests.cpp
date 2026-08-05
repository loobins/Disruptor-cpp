#include "stdafx.h"

#include "Disruptor/PhasedBackoffWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;



TEST(PhasedBackoffWaitStrategyTests, ShouldHandleImmediateSequenceChange)
{
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(0, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

TEST(PhasedBackoffWaitStrategyTests, ShouldHandleSequenceChangeWithOneMillisecondDelay)
{
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(1, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

TEST(PhasedBackoffWaitStrategyTests, ShouldHandleSequenceChangeWithTwoMillisecondDelay)
{
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(2, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

TEST(PhasedBackoffWaitStrategyTests, ShouldHandleSequenceChangeWithTenMillisecondDelay)
{
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withLiteLock(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
    assertWaitForWithDelayOf(10, PhasedBackoffWaitStrategy::withSleep(std::chrono::milliseconds(1), std::chrono::milliseconds(1)));
}

