#include <daemon.h>
#include <util/timer.h>
#include <user.h>
#include <event/listener.h>
#include "event/command_event.h"
#include <iostream>

namespace aeacus
{
    bool Daemon::s_Lock;

    Daemon::Daemon(long pollingDelay)
        : m_Delay(pollingDelay), m_Timer(nullptr), m_Running(false)
    {
        if (!s_Lock)
            s_Lock = true;
        else
            throw DuplicateDaemonException();
    }

    void Daemon::event(const MessageEvent& event) const
    {
    }

    void Daemon::start()
    {
        m_Running = true;
        m_Timer = new Timer([&]() {
            std::vector<Message> messages = UserContext::get().getUser().getNewMessages();
            if (!messages.empty())
            {
                for (const Message& msg : messages)
                    ListenerContext::get().event(CommandEvent(msg));
            }
        }, m_Delay, true);
    }

    void Daemon::stop()
    {
        m_Running = false;
        m_Timer->cancel();
    }

    Daemon::~Daemon()
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    bool Daemon::isRunning() const
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
}