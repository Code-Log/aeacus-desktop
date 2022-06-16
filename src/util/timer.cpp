#include <util/timer.h>
#include <chrono>
#include <thread>
#include <utility>

namespace aeacus
{
    Timer::Timer(std::function<void()> callback, long duration, bool repeat)
        : m_Duration(duration), m_Cancelled(false)
    {
        using namespace std::chrono;
        m_StartTime = high_resolution_clock::now();
        m_Callback = std::move(callback);
        m_Function = [&]() {
            while (!m_Cancelled)
            {
                auto time = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(time - m_StartTime);
                if (duration.count() >= m_Duration)
                {
                    m_Callback();

                    if (repeat)
                        m_StartTime = high_resolution_clock::now();
                    else
                        break;
                }
            }
        };

        m_Worker = std::thread(m_Function);
    }

    void Timer::cancel()
    {
        m_Cancelled = true;
        m_Worker.join();
    }

    bool Timer::isCancelled() const
    {
        return m_Cancelled;
    }
}