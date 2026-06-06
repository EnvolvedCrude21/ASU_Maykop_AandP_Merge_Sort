#pragma once

#include <chrono>

namespace NBenchmark
{

class CTimer
{
public:
    CTimer();

    void Restart();

    long long ElapsedMicroseconds() const;

private:
    std::chrono::steady_clock::time_point m_startTime;
};

} // namespace NBenchmark
