#include <api/message.h>
#include <util/base64.h>
#include <crypto/crypto.h>
#include <iostream>

namespace aeacus
{
    bool Message::verify(const std::string& keyStr) const
    {
        return verifySignature(payload->serialize().dump(), m_Sig, keyStr);
    }

    nlohmann::json MessagePayload::serialize() const
    {
        using namespace nlohmann;
        json result;

        result["type"] = type;
        result["timestamp"] = timestamp;
        result["target"] = target;

        return result;
    }

    Message Message::fromJSON(nlohmann::json json)
    {
        Message msg;
        msg.m_Sig = json["signature"];
        msg.m_User = json["user"];
        msg.payload = MessagePayload::fromJSON(json["payload"]);

        return msg;
    }

    MessagePayload* MessagePayload::fromJSON(nlohmann::json json)
    {
        if (json["type"] == "command")
            return CommandPayload::fromJSON(json);

        return nullptr;
    }

    MessagePayload::~MessagePayload()
    {
    }

    Message::~Message()
    {
        delete payload;
        payload = nullptr;
    }

    bool Message::operator>(const Message &other) const
    {
        return payload->timestamp > other.payload->timestamp;
    }

    bool Message::operator<(const Message &other) const
    {
        return payload->timestamp < other.payload->timestamp;
    }

    Message::Message(const Message &other)
        : m_Sig(other.m_Sig), m_User(other.m_User)
    {
        payload = new CommandPayload();
        payload->timestamp = other.payload->timestamp;
        payload->type = other.payload->type;
        payload->target = other.payload->target;
        ((CommandPayload*)payload)->command = ((CommandPayload*)other.payload)->command;
    }

    Message& Message::operator=(const Message &other)
    {
        m_Sig = other.m_Sig;
        m_User = other.m_User;
        payload = new CommandPayload();
        payload->timestamp = other.payload->timestamp;
        payload->type = other.payload->type;
        payload->target = other.payload->target;
        ((CommandPayload*)payload)->command = ((CommandPayload*)other.payload)->command;

        return *this;
    }

    nlohmann::json CommandPayload::serialize() const
    {
        using namespace nlohmann;
        json result;

        result["command"] = command;
        result["type"] = type;
        result["timestamp"] = timestamp;
        result["target"] = target;

        return result;
    }

    CommandPayload* CommandPayload::fromJSON(nlohmann::json json)
    {
        auto* payload = new CommandPayload();
        payload->timestamp = json["timestamp"];
        payload->target = json["target"];
        payload->type = json["type"];
        payload->command = json["command"];

        return payload;
    }
}