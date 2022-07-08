#include <curl/curl.h>
#include <user.h>
#include <daemon/daemon.h>
#include <daemon/socket_daemon.h>
#include <reference.h>
#include <iostream>
#include <util/cline_argument_parser.h>
#include <util/InvalidArgumentException.h>
#include <glog/logging.h>

CommandLineArgumentParser setupParser()
{
    CommandLineArgumentParser parser = CommandLineArgumentParser();
    Argument setup("s", "setup", true);
    setup.addAlias("setup");
    parser.registerArgument(setup);

    Argument username("u", "username", false);
    parser.registerArgument(username);

    Argument daemon("d", "daemon", true);
    daemon.addAlias("daemon");
    parser.registerArgument(daemon);

    Argument socket("S", "socket", true);
    socket.addAlias("socket");
    parser.registerArgument(socket);

    return std::move(parser);
}

int main(int argc, const char** argv)
{
    google::InitGoogleLogging( argv[0]);
    LOG(INFO) << "Start initialization..." << std::endl;
    curl_global_init(CURL_GLOBAL_ALL);

    auto parser = setupParser();
    std::unordered_map<std::string, std::string> options;
    try
    {
        options = parser.parse(argc, argv);
    }
    catch (InvalidArgumentException& e)
    {
        LOG(ERROR) << e.what() << std::endl;
        return -1;
    }

    if (options.find("setup") != options.end())
    {
        LOG(INFO) << "Setup called" << std::endl;
        if (options.find("username") == options.end())
        {
            LOG(ERROR) << "Missing username argument!" << std::endl;
            return -1;
        }

        std::string username = options.at("username");
        std::string password;
        LOG(INFO) << "Please enter your password: ";
        std::cin >> password;

        aeacus::UserContext::create(username, password);
        if (aeacus::UserContext::get().getUser() == nullptr)
        {
            LOG(ERROR) << "Login failed!" << std::endl;
            return -1;
        }

        aeacus::UserContext::get().save();
    }

    if (options.find("daemon") != options.end() || options.find("socket") != options.end())
    {
        if (!aeacus::UserContext::recall())
        {
            LOG(ERROR) << "Please run setup before running the daemon!" << std::endl;
            return 0;
        }
    }

    if (options.find("daemon") != options.end())
    {
        LOG(INFO) << "PollingDaemon called" << std::endl;

        auto daemon = std::make_shared<aeacus::PollingDaemon>(5000);
        aeacus::ListenerContext::get().addListener(daemon);

        LOG(INFO) << "Starting daemon..." << std::endl;
        daemon->start();
        LOG(INFO) << "PollingDaemon started" << std::endl;

        daemon->wait();

        aeacus::UserContext::destroy();
    }

    if (options.find("socket") != options.end())
    {
        LOG(INFO) << "SocketDaemon called" << std::endl;

        auto daemon = std::make_shared<aeacus::SocketDaemon>(AEACUS_SOCKET_URI, 3000);
        aeacus::ListenerContext::get().addListener(daemon);

        LOG(INFO) << "Starting daemon..." << std::endl;
        daemon->start();
        LOG(INFO) << "SocketDaemon started" << std::endl;

        daemon->wait();

        aeacus::UserContext::destroy();
    }

    curl_global_cleanup();
    return 0;
}