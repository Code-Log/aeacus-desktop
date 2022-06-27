#ifndef AEACUS_MESSAGE_EVENT_H
#define AEACUS_MESSAGE_EVENT_H

#pragma once

#include <api/message.h>
#include <ostream>

namespace aeacus
{
    class MessageEvent
    {
    protected:
        Message m_Message;
    public:
        virtual void handle() const = 0;
        [[nodiscard]] virtual const Message& getMessage() const = 0;
        friend std::ostream& operator<<(std::ostream& stream, const MessageEvent& event);
    };
}

std::ostream& operator<<(std::ostream& stream, const aeacus::MessageEvent& event);

#endif //AEACUS_MESSAGE_EVENT_H
