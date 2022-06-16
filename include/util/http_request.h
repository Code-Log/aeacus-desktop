#ifndef AEACUS_HTTP_REQUEST_H
#define AEACUS_HTTP_REQUEST_H

#pragma once

#include <nlohmann/json.hpp>
namespace aeacus
{
    struct HttpRequestException : public std::exception
    {
        const char* m_Message;

        explicit HttpRequestException(const char* message);

        [[nodiscard]] const char* what() const noexcept override;
    };

    class HttpRequest
    {
    private:
        nlohmann::json m_Response;
        nlohmann::json m_Body;
        std::string m_Url;
        std::vector<std::string> chunks;

    public:
        HttpRequest(std::string url, nlohmann::json body);

        void sendRequest();

        friend size_t write(void* buffer, size_t size, size_t nmemb, void* userp);

        [[nodiscard]] nlohmann::json getResponse() const;
    };
}

#endif //AEACUS_HTTP_REQUEST_H
