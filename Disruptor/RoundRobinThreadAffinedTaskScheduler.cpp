#include "stdafx.h"
#include "RoundRobinThreadAffinedTaskScheduler.h"

#include "ArgumentOutOfRangeException.h"
#include "ThreadHelper.h"


namespace Disruptor
{

    void RoundRobinThreadAffinedTaskScheduler::start(std::int32_t numberOfThreads)
    {
        if (m_started)
            return;

        m_started = true;

        if (numberOfThreads < 1)
            DISRUPTOR_THROW_ARGUMENT_OUT_OF_RANGE_EXCEPTION(numberOfThreads);

        createThreads(numberOfThreads);
    }

    RoundRobinThreadAffinedTaskScheduler::~RoundRobinThreadAffinedTaskScheduler()
    {
        // std::thread calls std::terminate if destroyed while joinable, so the pool
        // must always be cleaned up (boost::thread previously detached on destruction).
        stop();
    }

    void RoundRobinThreadAffinedTaskScheduler::stop()
    {
        m_started = false;

        for (auto&& thread : m_threads)
        {
            if (!thread.joinable())
                continue;

            // std::thread has no timed join. Run the join on a helper thread and wait on
            // it with a timeout; if the worker task is still blocked (e.g. the consumers
            // were never halted), abandon it rather than block forever. This mirrors the
            // original boost::thread::timed_join safety net.
            auto worker = std::make_shared< std::thread >(std::move(thread));
            std::packaged_task< void() > joinTask([worker] { worker->join(); });
            auto joined = joinTask.get_future();
            std::thread joiner(std::move(joinTask));

            if (joined.wait_for(std::chrono::seconds(2)) == std::future_status::timeout)
                joiner.detach();
            else
                joiner.join();
        }

        m_threads.clear();
    }

    std::future< void > RoundRobinThreadAffinedTaskScheduler::scheduleAndStart(std::packaged_task< void() >&& task)
    {
        auto future = task.get_future();
        m_tasks.push(std::move(task));

        return future;
    }

    void RoundRobinThreadAffinedTaskScheduler::createThreads(std::int32_t numberOfThreads)
    {
        for (auto i = 0; i < numberOfThreads; ++i)
            m_threads.emplace_back([this, i]() { workingLoop(i); });
    }

    void RoundRobinThreadAffinedTaskScheduler::workingLoop(std::int32_t threadId)
    {
        static const auto processorCount = std::thread::hardware_concurrency();

        const auto processorIndex = threadId % processorCount;

        const auto affinityMask = ThreadHelper::AffinityMask(1ull << processorIndex);

        ThreadHelper::setThreadAffinity(affinityMask);

        while (m_started)
        {
            std::packaged_task< void() > task;
            while (m_tasks.timedWaitAndPop(task, std::chrono::milliseconds(100)))
            {
                tryExecuteTask(task);
            }
        }
    }

    void RoundRobinThreadAffinedTaskScheduler::tryExecuteTask(std::packaged_task< void() >& task)
    {
        task();
    }

} // namespace Disruptor
