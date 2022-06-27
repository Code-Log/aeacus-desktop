#ifndef AEACUS_DAEMON_H
#define AEACUS_DAEMON_H

#pragma once

#include <event/listener.h>
#include <util/timer.h>
#include <exception>

namespace aeacus
{
    class DuplicateDaemonException : public std::exception
    {
    private:
        const char* m_Message;
    public:
        DuplicateDaemonException();
        ~DuplicateDaemonException() noexcept override;
        [[nodiscard]] const char * what() const noexcept override;
    };

    class Daemon : public EventListener
    {
    private:
        long m_Delay;
        Timer* m_Timer;
        bool m_Running;
        static bool s_Lock;

    public:
        static Daemon* s_Daemon;

        explicit Daemon(long pollingDelay);
        ~Daemon();

        void start();
        void stop();
        [[nodiscard]] bool isRunning() const;
        void event(const MessageEvent& event) const override;
        void wait();
    };
}

#endif //AEACUS_DAEMON_H
