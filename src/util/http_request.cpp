#include <util/http_request.h>
#include <curl/curl.h>
#include <utility>
#include <sstream>

namespace aeacus
{
    const char* HttpRequestException::what() const noexcept
    {
        return m_Message;
    }

    HttpRequestException::HttpRequestException(const char* message)
            : m_Message(message)
    {
    }

    HttpRequest::HttpRequest(std::string url, nlohmann::json body)
            : m_Url(std::move(url)), m_Body(std::move(body))
    {
    }

    size_t write(void* buffer, size_t size, size_t nmemb, void* userp)
    {
        using namespace nlohmann;
        size_t dataLength = nmemb + 1;

        char* data = new char[dataLength];
        std::memcpy(data, buffer, nmemb);

        // No matter what, we add a null-terminator
        data[dataLength - 1] = 0;

        reinterpret_cast<HttpRequest*>(userp)->chunks.emplace_back(data);
        return nmemb;
    }

    void HttpRequest::sendRequest()
    {
        CURL* handle = curl_easy_init();
        curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string bodyStr = m_Body.dump();

        curl_easy_setopt(handle, CURLOPT_URL, m_Url.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, bodyStr.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, -1L);
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)this);

        int status = curl_easy_perform(handle);
        if (status != 0)
            throw HttpRequestException("The requested URL returned an error code!");

        std::string jsonStr;
        for (const std::string& str : chunks)
            jsonStr += str;

        m_Response = nlohmann::json::parse(jsonStr);
    }

    nlohmann::json HttpRequest::getResponse() const
    {
        return m_Response;
    }
}