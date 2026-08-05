#include "stdafx.h"
#include "TestRepository.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace
{
    std::string toLowerCopy(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast< char >(std::tolower(c)); });
        return value;
    }
}

// Raw
#include "OneToOneRawBatchThroughputTest.h"
#include "OneToOneRawThroughputTest.h"

// Sequenced
#include "OneToOneSequencedBatchThroughputTest.h"
#include "OneToOneSequencedLongArrayThroughputTest.h"
#include "OneToOneSequencedPollerThroughputTest.h"
#include "OneToOneSequencedThroughputTest.h"
#include "OneToThreeDiamondSequencedThroughputTest.h"
#include "OneToThreePipelineSequencedThroughputTest.h"
#include "OneToThreeSequencedThroughputTest.h"
#include "PingPongSequencedLatencyTest.h"
#include "ThreeToOneSequencedBatchThroughputTest.h"
#include "ThreeToOneSequencedThroughputTest.h"
#include "ThreeToThreeSequencedThroughputTest.h"

// Translator
#include "OneToOneTranslatorThroughputTest.h"

// WorkHandler
#include "OneToThreeReleasingWorkerPoolThroughputTest.h"
#include "OneToThreeWorkerPoolThroughputTest.h"
#include "TwoToTwoWorkProcessorThroughputTest.h"


namespace Disruptor
{
namespace PerfTests
{

    TestRepository::TestRepository()
    {
        // Raw
        registerTest< OneToOneRawBatchThroughputTest >();
        registerTest< OneToOneRawThroughputTest >();

        // Sequenced
        registerTest< OneToOneSequencedBatchThroughputTest >();
        registerTest< OneToOneSequencedLongArrayThroughputTest >();
        registerTest< OneToOneSequencedPollerThroughputTest >();
        registerTest< OneToOneSequencedThroughputTest >();
        registerTest< OneToThreeDiamondSequencedThroughputTest >();
        registerTest< OneToThreePipelineSequencedThroughputTest >();
        registerTest< OneToThreeSequencedThroughputTest >();
        registerTest< PingPongSequencedLatencyTest >();
        registerTest< ThreeToOneSequencedBatchThroughputTest >();
        registerTest< ThreeToOneSequencedThroughputTest >();
        registerTest< ThreeToThreeSequencedThroughputTest >();

        // Translator
        registerTest< OneToOneTranslatorThroughputTest >();

        // WorkHandler
        registerTest< OneToThreeReleasingWorkerPoolThroughputTest >();
        registerTest< OneToThreeWorkerPoolThroughputTest >();
        registerTest< TwoToTwoWorkProcessorThroughputTest >();
    }

    void TestRepository::registerTest(const TypeInfo& typeInfo, const std::function<std::shared_ptr< IThroughputTest >()>& testFactory)
    {
        ThroughputTestInfo info{ typeInfo.name(), testFactory };

        m_throughputTestInfosByName.insert(std::make_pair(toLowerCopy(typeInfo.fullyQualifiedName()), info));
        m_throughputTestInfosByName.insert(std::make_pair(toLowerCopy(typeInfo.name()), info));
    }

    void TestRepository::registerTest(const TypeInfo& typeInfo, const std::function<std::shared_ptr< ILatencyTest >()>& testFactory)
    {
        LatencyTestInfo info{ typeInfo.name(), testFactory };

        m_latencyTestInfosByName.insert(std::make_pair(toLowerCopy(typeInfo.fullyQualifiedName()), info));
        m_latencyTestInfosByName.insert(std::make_pair(toLowerCopy(typeInfo.name()), info));
    }

    const TestRepository& TestRepository::instance()
    {
        static TestRepository instance;
        return instance;
    }

    std::vector< ThroughputTestInfo > TestRepository::allThrougputTests() const
    {
        std::vector< ThroughputTestInfo > result;
        std::set< std::string > testNames;

        for (auto&& x : m_throughputTestInfosByName)
        {
            if (testNames.count(x.second.name) > 0)
                continue;

            testNames.insert(x.second.name);
            result.push_back(x.second);
        }
        return result;
    }

    bool TestRepository::tryGetThroughputTest(const std::string& testName, ThroughputTestInfo& testInfo) const
    {
        auto it = m_throughputTestInfosByName.find(toLowerCopy(testName));
        if (it == m_throughputTestInfosByName.end())
            return false;

        testInfo = it->second;
        return true;
    }

    std::vector< LatencyTestInfo > TestRepository::allLatencyTests() const
    {
        std::vector< LatencyTestInfo > result;
        std::set< std::string > testNames;

        for (auto&& x : m_latencyTestInfosByName)
        {
            if (testNames.count(x.second.name) > 0)
                continue;

            testNames.insert(x.second.name);
            result.push_back(x.second);
        }
        return result;
    }

    bool TestRepository::tryGetLatencyTest(const std::string& testName, LatencyTestInfo& testInfo) const
    {
        auto it = m_latencyTestInfosByName.find(toLowerCopy(testName));
        if (it == m_latencyTestInfosByName.end())
            return false;

        testInfo = it->second;
        return true;
    }

} // namespace PerfTests
} // namespace Disruptor
