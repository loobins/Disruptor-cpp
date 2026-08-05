#include "stdafx.h"

#include "Disruptor/SpinWaitWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;



TEST(SpinWaitWaitStrategyTests, ShouldWaitForValue)
{
    assertWaitForWithDelayOf(50, std::make_shared< SpinWaitWaitStrategy >());
}

