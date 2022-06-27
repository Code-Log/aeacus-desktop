#ifndef AEACUS_MESSAGE_H
#define AEACUS_MESSAGE_H

#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace aeacus
{
    struct MessagePayload
    {
        std::string type;
        long timestamp;
        int target;

        virtual ~MessagePayload();

        [[nodiscard]] virtual nlohmann::json serialize() const = 0;
        static MessagePayload* fromJSON(nlohmann::json json);
    };

    struct CommandPayload : public MessagePayload
    {
        std::string command;

        [[nodiscard]] nlohmann::json serialize() const override;
        static CommandPayload* fromJSON(nlohmann::json json);
    };

    struct Message
    {
        Message() = default;
        Message(const Message& other);
        std::string m_Sig;
        std::string m_User;
        MessagePayload* payload;

        [[nodiscard]] bool verify(const std::string& keyStr) const;
        static Message fromJSON(nlohmann::json json);

        ~Message();

        bool operator>(const Message& other) const;
        bool operator<(const Message& other) const;

        Message& operator=(const Message& other);
    };
}

#endif //AEACUS_MESSAGE_H
