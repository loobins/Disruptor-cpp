#include "stdafx.h"
#include "LiteTimeoutBlockingWaitStrategy.h"

#include <ostream>

#include "ISequenceBarrier.h"
#include "Sequence.h"
#include "TimeoutException.h"


namespace Disruptor
{

    LiteTimeoutBlockingWaitStrategy::LiteTimeoutBlockingWaitStrategy(ClockConfig::Duration timeout)
        : m_timeout(timeout)
    {
    }

    std::int64_t LiteTimeoutBlockingWaitStrategy::waitFor(std::int64_t sequence,
                                                          Sequence& cursor,
                                                          ISequence& dependentSequence,
                                                          ISequenceBarrier& barrier)
    {
        auto timeSpan = std::chrono::duration_cast< std::chrono::microseconds >(m_timeout);

        if (cursor.value() < sequence)
        {
            std::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            while (cursor.value() < sequence)
            {
                m_signalNeeded.exchange(true);

                barrier.checkAlert();

                if (m_conditionVariable.wait_for(uniqueLock, timeSpan) == std::cv_status::timeout)
                    DISRUPTOR_THROW_TIMEOUT_EXCEPTION();
            }
        }

        std::int64_t availableSequence;
        while ((availableSequence = dependentSequence.value()) < sequence)
        {
            barrier.checkAlert();
        }

        return availableSequence;
    }

    void LiteTimeoutBlockingWaitStrategy::signalAllWhenBlocking()
    {
        if (m_signalNeeded.exchange(false))
        {
            std::unique_lock< decltype(m_gate) > uniqueLock(m_gate);

            m_conditionVariable.notify_all();
        }
    }

    void LiteTimeoutBlockingWaitStrategy::writeDescriptionTo(std::ostream& stream) const
    {
        stream << "LiteTimeoutBlockingWaitStrategy";
    }

} // namespace Disruptor
