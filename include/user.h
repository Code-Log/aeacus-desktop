#ifndef AEACUS_USER_H
#define AEACUS_USER_H

#pragma once

#include <string>
#include <vector>
#include <api/message.h>
#include <user.h>
#include "api/token.h"

namespace aeacus
{
    class User
    {
    private:
        std::string m_Username;
        std::string m_PublicKey;
        std::string m_SecretKey;
        std::string m_VaultKey;
        long m_LastAck;

        User(std::string username, std::string publicKey, std::string secretKey, std::string vaultKey);
    public:
        static User* create(const std::string& username, const std::string& password);

        [[nodiscard]] std::vector<Message> getNewMessages();

        [[nodiscard]] const std::string& getUsername() const;
        [[nodiscard]] const std::string& getKey() const;
        [[nodiscard]] Token getToken() const;
    };

    class UserContext
    {
    private:
        User* m_User;
        UserContext(const std::string& username, const std::string& password);
        static UserContext* s_Instance;

    public:
        static UserContext& get();
        ~UserContext();

        static void create(const std::string& username, const std::string& password);
        static void destroy();

        User& getUser();
    };
}

#endif //AEACUS_USER_H
