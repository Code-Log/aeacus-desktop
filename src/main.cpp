#include <curl/curl.h>
#include <user.h>
#include <daemon.h>
#include <crypto/crypto.h>
#include <iostream>

int main(int argc, const char** argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    aeacus::UserContext::create("joe", "1234");

    auto daemon = std::make_shared<aeacus::Daemon>(5000);
    aeacus::ListenerContext::get().addListener(daemon);

    daemon->start();

    std::cin.get();

    aeacus::UserContext::destroy();
    curl_global_cleanup();
    return 0;
}