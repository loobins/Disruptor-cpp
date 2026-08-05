#pragma once

#include <cstdint>
#include <iosfwd>
#include <vector>


namespace Disruptor
{
namespace Tests
{

    class LatencyRecorder
    {
    public:
        explicit LatencyRecorder(std::int64_t sampleSize);

        void record(std::int64_t value);

        void writeReport(std::ostream& stream) const;

    private:
        std::vector< double > m_samples;
    };

} // namespace Tests
} // namespace Disruptor
