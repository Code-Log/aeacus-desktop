#ifndef AEACUS_REQUEST_H
#define AEACUS_REQUEST_H

#pragma once

#include <string>
#include <vector>
#include <exception>
#include <nlohmann/json.hpp>
#include "token.h"

namespace aeacus
{
    struct APIException : public std::exception
    {
        const char* message;
        explicit APIException(const std::string& message);
        ~APIException() override;

        [[nodiscard]] const char* what() const noexcept override;
    };

    class APIRequest
    {
    private:
        std::string m_Method;
        nlohmann::json m_Body;
        bool m_Auth;

    public:
        APIRequest(std::string method, bool authenticate);

        APIRequest(const APIRequest& other) = delete;
        APIRequest& operator=(const APIRequest& other) = delete;

        template <typename T>
        void setParam(const std::string& key, T value)
        {
            m_Body[key] = value;
        }

        nlohmann::json send(const std::string* username, const Token* token);
    };
}

#endif //AEACUS_REQUEST_H
