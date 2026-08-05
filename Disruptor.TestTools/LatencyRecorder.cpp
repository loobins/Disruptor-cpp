#include "stdafx.h"
#include "LatencyRecorder.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <ostream>

#include "Disruptor.TestTools/DurationHumanizer.h"


namespace Disruptor
{
namespace Tests
{

    struct DurationPrinter
    {
        explicit DurationPrinter(std::int64_t nanoseconds)
            : value(nanoseconds)
        {
        }

        std::int64_t value;
    };

    std::ostream& operator<<(std::ostream& stream, const DurationPrinter& printer)
    {
        auto humanDuration = Tests::DurationHumanizer::deduceHumanDuration(std::chrono::nanoseconds(printer.value));

        return stream << humanDuration.value << " " << humanDuration.shortUnitName;
    }

    namespace
    {
        // Linear-interpolation percentile over a sorted sample, matching the
        // "right tail" quantile previously provided by Boost.Accumulators.
        double percentile(const std::vector< double >& sorted, double probability)
        {
            if (sorted.empty())
                return 0.0;

            if (sorted.size() == 1)
                return sorted.front();

            const auto rank = probability * (sorted.size() - 1);
            const auto lowerIndex = static_cast< std::size_t >(std::floor(rank));
            const auto upperIndex = static_cast< std::size_t >(std::ceil(rank));
            const auto weight = rank - lowerIndex;

            return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
        }
    }


    LatencyRecorder::LatencyRecorder(std::int64_t sampleSize)
    {
        if (sampleSize > 0)
            m_samples.reserve(static_cast< std::size_t >(sampleSize));
    }

    void LatencyRecorder::record(std::int64_t value)
    {
        m_samples.push_back(static_cast< double >(value));
    }

    void LatencyRecorder::writeReport(std::ostream& stream) const
    {
        if (m_samples.empty())
        {
            stream << "no samples recorded";
            return;
        }

        std::vector< double > sorted(m_samples);
        std::sort(sorted.begin(), sorted.end());

        const auto count = static_cast< double >(sorted.size());
        const auto sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
        const auto mean = sum / count;

        stream
            << "min: " << DurationPrinter(static_cast< std::int64_t >(sorted.front()))
            << ", mean: " << DurationPrinter(static_cast< std::int64_t >(mean))
            << ", max: " << DurationPrinter(static_cast< std::int64_t >(sorted.back()))
            << ", Q99.99: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.9999)))
            << ", Q99.9: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.999)))
            << ", Q99: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.99)))
            << ", Q98: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.98)))
            << ", Q95: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.95)))
            << ", Q93: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.93)))
            << ", Q90: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.90)))
            << ", Q50: " << DurationPrinter(static_cast< std::int64_t >(percentile(sorted, 0.50)))
            ;
    }

} // namespace Tests
} // namespace Disruptor
