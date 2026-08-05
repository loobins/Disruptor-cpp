#include "stdafx.h"

#include <algorithm>
#include <cctype>
#include <string>

#include "LatencyTestSession.h"
#include "TestRepository.h"
#include "ThroughputTestSession.h"

using namespace Disruptor::PerfTests;

namespace
{
    void trim(std::string& value)
    {
        const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
        value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    }

    bool iequals(const std::string& lhs, const std::string& rhs)
    {
        return lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                          [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
    }
}

void runAllTests(const TestRepository& testRepository);
void runOneTest(const TestRepository& testRepository, const std::string& testName);

int main(int, char**)
{
    auto& testRepository = TestRepository::instance();

    std::string testName;

    std::cout << "Test name (ALL by default):  " << testName << " ?" << std::endl;

    std::getline(std::cin, testName);

    trim(testName);

    if (iequals(testName, "ALL") || testName.empty())
    {
        runAllTests(testRepository);
    }
    else
    {
        runOneTest(testRepository, testName);
    }

    return 0;
}

void runAllTests(const TestRepository& testRepository)
{
    for (auto&& info : testRepository.allLatencyTests())
    {
        LatencyTestSession session(info);
        session.run();
    }

    for (auto&& info : testRepository.allThrougputTests())
    {
        ThroughputTestSession session(info);
        session.run();
    }
}

void runOneTest(const TestRepository& testRepository, const std::string& testName)
{
    LatencyTestInfo latencyTestInfo;
    if (testRepository.tryGetLatencyTest(testName, latencyTestInfo))
    {
        LatencyTestSession session(latencyTestInfo);
        session.run();
    }
    else
    {
        ThroughputTestInfo throughputTestInfo;
        if (testRepository.tryGetThroughputTest(testName, throughputTestInfo))
        {
            ThroughputTestSession session(throughputTestInfo);
            session.run();
        }
    }
}
