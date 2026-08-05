# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Disruptor-cpp is a C++20 port of the LMAX Disruptor — a high-performance inter-thread
messaging library built around a pre-allocated ring buffer with lock-free sequence
coordination. It implements the feature set of the Java Disruptor v3.3.7. The library is
distributed as a static (or shared) library plus public headers under `Disruptor/`.

The library has **no third-party dependencies** — only the C++20 standard library and the
platform thread library (pthreads via CMake `Threads::Threads`). Boost was removed in favour
of `<condition_variable>`, `<mutex>`, `<thread>`, `<chrono>`, `<any>`, `<optional>`,
`<barrier>`, and `<filesystem>`.

## Building

Requires a C++20 compiler (GCC 10+, Clang 10+, MSVC 2019+).

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=release
make
```

Key CMake options:
- `DISRUPTOR_BUILD_TESTS` (default OFF) — builds tests, perf tests, and TestTools. Required to run any tests.
- `DISRUPTOR_CONAN` (default ON) — if a `conanbuildinfo.cmake` is present it is used to locate GoogleTest; otherwise the bundled `googletest-release-1.8.0/` is built. The library itself needs no conan packages.
- `DISRUPTOR_BUILD_STATIC` (default ON) / `DISRUPTOR_BUILD_SHARED` (default OFF).

## Testing

Tests only build when configured with `-DDISRUPTOR_BUILD_TESTS=true`. The unit tests use
**GoogleTest / GoogleMock** (bundled under `googletest-release-1.8.0/`, gtest 1.8.0 — note it
uses `TYPED_TEST_CASE`, not the newer `TYPED_TEST_SUITE`). `main.cpp` provides the entry point
(`InitGoogleMock` + `RUN_ALL_TESTS`). Tests are auto-registered with CTest via CMake's built-in
`gtest_discover_tests`.

```sh
# from build/, after building with tests enabled
make
ctest                            # runs all discovered unit tests
ctest -R <regex>                 # run a subset by test name
./Disruptor.Tests/Disruptor.Tests                          # run the test binary directly
./Disruptor.Tests/Disruptor.Tests --gtest_filter=<Suite>.* # single gtest suite/case
```

The performance benchmarks are a separate executable, `Disruptor.PerfTests/Disruptor.PerfTests`
(throughput and ping-pong latency sessions); it is not run by CTest.

## Architecture

The design mirrors the Java Disruptor closely. Nearly everything is a class template parameterized
on the user's event type `T`; template classes live entirely in headers, while non-templated concrete
classes (wait strategies, sequencers' shared pieces, schedulers, `Sequence`, `Util`) are split into
`.h` + `.cpp`. Interfaces are `I`-prefixed pure-virtual headers (e.g. `IEventHandler`, `IWaitStrategy`,
`ISequencer`). Objects are wired together with `std::shared_ptr` throughout.

Data flow: a **producer** claims the next slot(s) from the ring buffer, writes the event in place, and
publishes the sequence. **Consumers** (event processors) each own a `Sequence` (a cache-line-padded
atomic counter) and advance it as they process; a **wait strategy** blocks/spins them until the
required sequence is available, and **sequence barriers** enforce ordering dependencies between
consumer stages.

Core components:
- **`disruptor<T>` (`Disruptor.h`)** — the DSL/builder front-end. `handleEventsWith(...)`, `after(...)`,
  and `handleEventsWithWorkerPool(...)` declare the consumer dependency graph; `start()` wires gating
  sequences and launches processors; `shutdown()`/`halt()` tear them down. Most tasks start here.
- **`RingBuffer<T>`** — the pre-allocated event storage; delegates claim/publish to a `Sequencer`.
- **Sequencers** — `SingleProducerSequencer` and `MultiProducerSequencer` (selected via `ProducerType`)
  implement the claim/publish protocol and hand out `SequenceBarrier`s.
- **`Sequence`** — padded atomic `int64`, the fundamental coordination primitive.
- **`BatchEventProcessor<T>`** — the standard consumer loop that reads a barrier and dispatches to an
  `IEventHandler<T>`. `WorkerPool<T>` / `WorkProcessor<T>` implement competing-consumer (each event
  handled once) semantics via `IWorkHandler<T>`.
- **Wait strategies** (`IWaitStrategy`) — `Blocking`, `LiteBlocking`, `TimeoutBlocking`,
  `LiteTimeoutBlocking`, `BusySpin`, `Yielding`, `Sleeping`, `SpinWait`, `PhasedBackoff`. Trade latency
  against CPU usage.
- **`ProcessingSequenceBarrier`** — coordinates a consumer against the cursor and its dependency sequences.
- **Executors / schedulers** — `IExecutor`/`BasicExecutor` run processors on threads supplied by an
  `ITaskScheduler` (`ThreadPerTaskScheduler`, `RoundRobinThreadAffinedTaskScheduler`).
- **`ConsumerRepository<T>` / `EventHandlerGroup<T>`** — track the consumer graph and support the fluent
  `after(...).handleEventsWith(...)` chaining.

## Conventions

- Exceptions are thrown through `DISRUPTOR_THROW_*` macros (see `ExceptionBase.h` and the individual
  `*Exception.h` headers), not raw `throw`. Exception handling during processing is pluggable via
  `IExceptionHandler<T>` (`FatalExceptionHandler`, `IgnoreExceptionHandler`, `ExceptionHandlerWrapper`).
- Platform-specific thread affinity/naming lives in `ThreadHelper_{Linux,macOS,Windows}.cpp` behind
  `ThreadHelper.h`.
- `stdafx.h`/`targetver.h` exist for MSVC precompiled-header support; each subproject has its own.
- Windows can alternatively be built with the Visual Studio solutions under `msvc/`
  (`Disruptor-lib.sln`, `Disruptor-all.sln`); adjust Boost paths in `boost.props`.
