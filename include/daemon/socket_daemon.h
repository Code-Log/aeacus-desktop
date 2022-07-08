#ifndef AEACUS_SOCKET_DAEMON_H
#define AEACUS_SOCKET_DAEMON_H

#pragma once

#include "daemon.h"
#include <easywsclient.hpp>

namespace aeacus
{
    class SocketDaemon : public Daemon
    {
    private:
        std::thread* m_Worker;
        bool m_Running;
        easywsclient::WebSocket::pointer m_Socket;
        std::string m_Addr;
        static bool s_Lock;
    public:
        static SocketDaemon* s_Daemon;

        SocketDaemon(std::string url, short port);
        ~SocketDaemon();

        void start() override;
        void stop() override;
        void event(const MessageEvent& event) const override;
        void wait() override;
    };
}

#endif //AEACUS_SOCKET_DAEMON_H
