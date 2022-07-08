#ifndef AEACUS_DAEMON_H
#define AEACUS_DAEMON_H

#pragma once

#include "event/listener.h"
#include "util/timer.h"
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
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void wait() = 0;
    };

    class PollingDaemon : public Daemon
    {
    private:
        long m_Delay;
        Timer* m_Timer;
        bool m_Running;
        static bool s_Lock;

    public:
        static PollingDaemon* s_Daemon;

        explicit PollingDaemon(long pollingDelay);
        ~PollingDaemon();

        void start() override;
        void stop() override;
        [[nodiscard]] bool isRunning() const;
        void event(const MessageEvent& event) const override;
        void wait() override;
    };
}

#endif //AEACUS_DAEMON_H
