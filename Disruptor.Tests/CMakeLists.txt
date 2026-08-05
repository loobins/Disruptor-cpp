project(Disruptor.Tests)
cmake_minimum_required(VERSION 2.6)

include(GoogleTest)

if (WITH_CONAN)
  set(GMock_LIBRARY CONAN_PKG::gtest)
else()
  set(GMock_LIBRARY gtest gmock)
endif()

set(DisruptorTests_sources

    main.cpp
    
    AggregateEventHandlerTests.cpp
    AggregateEventHandlerTestsFixture.cpp
    BatchEventProcessorTests.cpp
    BatchEventProcessorTestsFixture.cpp
    BatchingTests.cpp
    BusySpinWaitStrategyTests.cpp
    ConsumerRepositoryTests.cpp
    ConsumerRepositoryTestsFixture.cpp
    DelayedEventHandler.cpp
    DisruptorFixture.cpp
    DisruptorStressTest.cpp
    DisruptorTests.cpp
    DummySequenceBarrier.cpp
    EventPollerTests.cpp
    EventPublisherTests.cpp
    ExceptionThrowingEventHandler.cpp
    FatalExceptionHandlerTests.cpp
    FixedSequenceGroupTest.cpp
    IgnoreExceptionHandlerTests.cpp
    LifecycleAwareTests.cpp
    LiteBlockingWaitStrategyTests.cpp
    LiteTimeoutBlockingWaitStrategyTests.cpp
    MultiProducerSequencerTests.cpp
    PhasedBackoffWaitStrategyTests.cpp
    RingBufferTests.cpp
    RingBufferTestsFixture.cpp
    RingBufferWithMocksTest.cpp
    SequenceBarrierTests.cpp
    SequenceBarrierTestsFixture.cpp
    SequenceReportingCallbackTests.cpp
    SequencerTests.cpp
    SequenceUpdater.cpp
    SleepingEventHandler.cpp
    SleepingWaitStrategyTests.cpp
    SpinWaitWaitStrategyTests.cpp
    StubEvent.cpp
    StubExecutor.cpp
    StubPublisher.cpp
    TestWaiter.cpp
    TestWorkHandler.cpp
    TimeoutBlockingWaitStrategyTests.cpp
    UtilTests.cpp
    WaitStrategyTestUtil.cpp
    WorkerPoolTests.cpp
    YieldingWaitStrategyTests.cpp
)

add_executable(Disruptor.Tests ${DisruptorTests_sources})
gtest_discover_tests(Disruptor.Tests)

target_link_libraries(Disruptor.Tests
  PRIVATE
    Disruptor.TestTools
    ${GMock_LIBRARY}
)
