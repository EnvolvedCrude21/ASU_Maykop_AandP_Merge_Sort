#include "Timer.h"

namespace NBenchmark
{

CTimer::CTimer()
    : m_startTime(std::chrono::steady_clock::now())
{
}

void CTimer::Restart()
{
    m_startTime = std::chrono::steady_clock::now();
}

long long CTimer::ElapsedMicroseconds() const
{
    const auto finishTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - m_startTime).count();
}

} // namespace NBenchmark
