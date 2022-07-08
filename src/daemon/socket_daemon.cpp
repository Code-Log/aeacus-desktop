#include <daemon/socket_daemon.h>
#include <string>
#include <glog/logging.h>
#include <csignal>
#include <user.h>

namespace aeacus
{
    bool SocketDaemon::s_Lock = false;
    SocketDaemon* SocketDaemon::s_Daemon = nullptr;

    SocketDaemon::SocketDaemon(std::string url, short port)
        : m_Running(false), m_Worker(nullptr), m_Socket(nullptr)
    {
        if (!s_Lock)
            s_Lock = true;
        else
            throw DuplicateDaemonException();

        // If we get a trailing slash, get rid of it
        while (url[url.size() - 1] == '/')
            url = url.substr(0, url.size() - 1);

        std::stringstream stream;
        stream << url << ':' << port;

        std::string addr = stream.str();
        m_Addr = std::move(addr);
    }

    void SocketDaemon::start()
    {
        using easywsclient::WebSocket;
        using nlohmann::json;

        m_Running = true;
        s_Daemon = this;

        m_Worker = new std::thread([&]() {
            m_Socket = WebSocket::from_url(m_Addr);

            if (!m_Socket)
            {
               LOG(ERROR) << "Couldn't open socket!" << std::endl;
               m_Running = false;
               return;
            }

            json msg;
            msg["type"] = "auth";
            json auth;
            auth["uname"] = UserContext::get().getUser()->getUsername();
            auto token = UserContext::get().getUser()->getToken();
            auth["token"] = token.token;
            auth["signature"] = token.signature;
            msg["payload"] = auth;

            m_Socket->send(msg.dump());

            while (m_Running)
            {
                m_Socket->poll(0);
                m_Socket->dispatch([&](const std::string& msg) {
                    LOG(INFO) << "Received message: " << msg << std::endl;
                });
            }

            m_Socket->close();
        });

        signal(SIGINT, [](int signum) {
            if (SocketDaemon::s_Daemon != nullptr)
                SocketDaemon::s_Daemon->stop();
        });

        signal(SIGTERM, [](int signum) { raise(SIGINT); });
    }

    void SocketDaemon::stop()
    {
        m_Running = false;
    }

    void SocketDaemon::wait()
    {
        m_Worker->join();
    }

    SocketDaemon::~SocketDaemon()
    {
        delete m_Socket;
        delete m_Worker;
    }

    void SocketDaemon::event(const MessageEvent &event) const
    {

    }
}