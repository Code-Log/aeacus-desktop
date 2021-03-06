#include "daemon/daemon.h"
#include "util/timer.h"
#include "user.h"
#include "event/listener.h"
#include "event/command_event.h"
#include <iostream>
#include <csignal>
#include <glog/logging.h>

namespace aeacus
{
    bool PollingDaemon::s_Lock;
    PollingDaemon* PollingDaemon::s_Daemon = nullptr;

    PollingDaemon::PollingDaemon(long pollingDelay)
        : m_Delay(pollingDelay), m_Timer(nullptr), m_Running(false)
    {
        if (!s_Lock)
            s_Lock = true;
        else
            throw DuplicateDaemonException();
    }

    void PollingDaemon::event(const MessageEvent& event) const
    {
        event.handle();
    }

    void PollingDaemon::start()
    {
        s_Daemon = this;
        m_Running = true;
        m_Timer = new Timer([&]() {
            if (UserContext::get().getUser() == nullptr) {
                LOG(ERROR) << "No logged in user!" << std::endl;
                stop();
                return;
            }

            std::vector<Message> messages = UserContext::get().getUser()->getNewMessages();
            if (!messages.empty())
            {
                for (const Message& msg : messages)
                    ListenerContext::get().event(CommandEvent(msg));
            }
        }, m_Delay, true);

        signal(SIGINT, [](int signum) {
            if (PollingDaemon::s_Daemon != nullptr)
                PollingDaemon::s_Daemon->stop();
        });

        signal(SIGTERM, [](int signum) { raise(SIGINT); });
    }

    void PollingDaemon::stop()
    {
        m_Running = false;
        m_Timer->cancel();
    }

    PollingDaemon::~PollingDaemon()
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    bool PollingDaemon::isRunning() const
    {
        return m_Running;
    }

    DuplicateDaemonException::DuplicateDaemonException()
    {
        m_Message = "Attempting to create multiple Daemons. Only one is allowed!";
    }

    DuplicateDaemonException::~DuplicateDaemonException() noexcept
    {
        delete[] m_Message;
    }

    const char* DuplicateDaemonException::what() const noexcept
    {
        return m_Message;
    }

    void PollingDaemon::wait()
    {
        m_Timer->waitForCancel();
    }
}