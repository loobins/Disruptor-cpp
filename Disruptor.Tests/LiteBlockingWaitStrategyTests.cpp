#include "stdafx.h"

#include "Disruptor/LiteBlockingWaitStrategy.h"
#include "WaitStrategyTestUtil.h"


using namespace Disruptor;
using namespace Disruptor::Tests;



TEST(LiteBlockingWaitStrategyTests, ShouldWaitForValue)
{
    assertWaitForWithDelayOf(50, std::make_shared< LiteBlockingWaitStrategy >());
}

