#include <api/request.h>

#include <utility>
#include <util/http_request.h>
#include <reference.h>
#include <iostream>

namespace aeacus
{
    APIRequest::APIRequest(std::string method, bool authenticate)
        : m_Method(std::move(method)), m_Auth(authenticate), m_Body()
    {
    }

    APIException::APIException(const std::string& msgString)
    {
        char* buf = new char[msgString.size() + 1];
        std::memcpy((void*)buf, msgString.c_str(), msgString.size());
        buf[msgString.size()] = 0;
        message = buf;
    }

    APIException::~APIException()
    {
        delete[] message;
    }

    const char* APIException::what() const noexcept
    {
        return message;
    }

    nlohmann::json APIRequest::send(const std::string* username, const Token* token)
    {
        using namespace nlohmann;
        if (m_Auth) {
            if (token == nullptr || username == nullptr)
            {
                throw APIException("Username and token required!");
            }
            else
            {
                m_Body["token"] = token->token;
                m_Body["signature"] = token->signature;
                m_Body["uname"] = *username;
            }
        }

        HttpRequest req(AEACUS_API_URL + m_Method, m_Body);
        req.sendRequest();

        json response = req.getResponse();

//        LOG(INFO) << response << std::endl;

        if (!response.contains("status"))
        {
            throw APIException("Response did not contain a status field. Something has gone horribly wrong!");
        }

        if (response["status"] != "ok")
        {
            std::string message = response["message"];
            throw APIException(message);
        }

        return response;
    }
}