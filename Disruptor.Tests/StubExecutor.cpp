#include "stdafx.h"
#include "StubExecutor.h"


namespace Disruptor
{
namespace Tests
{

    std::future< void > StubExecutor::execute(const std::function< void() >& command)
    {
        ++m_executionCount;

        std::future< void > result;

        if (!m_ignoreExecutions)
        {
            std::packaged_task< void() > task(command);
            result = task.get_future();

            std::lock_guard< decltype(m_mutex) > lock(m_mutex);
            m_threads.push_back(std::thread(std::move(task)));
        }

        return result;
    }

    void StubExecutor::joinAllThreads()
    {
        std::lock_guard< decltype(m_mutex) > lock(m_mutex);

        while (!m_threads.empty())
        {
            std::thread thread(std::move(m_threads.front()));
            m_threads.pop_front();

            if (!thread.joinable())
                continue;

            // std::thread has no timed join (boost::thread::timed_join previously
            // gave up on workers that would not stop). Run the join on a helper thread
            // and wait on it with a timeout; if the worker does not stop in time, abandon
            // it rather than block the test suite forever.
            auto worker = std::make_shared< std::thread >(std::move(thread));
            std::packaged_task< void() > joinTask([worker] { worker->join(); });
            auto joined = joinTask.get_future();
            std::thread joiner(std::move(joinTask));

            if (joined.wait_for(std::chrono::seconds(2)) == std::future_status::timeout)
            {
                std::cerr << "StubExecutor: thread did not stop within timeout, abandoning: " << worker->get_id() << std::endl;
                joiner.detach();
            }
            else
            {
                joiner.join();
            }
        }
    }

    void StubExecutor::ignoreExecutions()
    {
        m_ignoreExecutions = true;
    }

    std::int32_t StubExecutor::getExecutionCount() const
    {
        return m_executionCount;
    }

} // namespace Tests
} // namespace Disruptor
