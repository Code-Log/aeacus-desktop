#ifndef AEACUS_TIMER_H
#define AEACUS_TIMER_H

#pragma once

#include <functional>
#include <chrono>
#include <thread>

namespace aeacus
{
    class Timer
    {
    private:
        std::chrono::time_point<std::chrono::system_clock> m_StartTime;
        std::function<void()> m_Function;
        std::function<void()> m_Callback;
        long m_Duration;
        std::thread m_Worker;
        bool m_Cancelled;

    public:
        explicit Timer(std::function<void()> callback, long duration, bool repeat = false);

        void cancel();
        bool isCancelled() const;
        void waitForCancel();
    };
}

#endif //AEACUS_TIMER_H
