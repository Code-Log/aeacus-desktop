#ifndef AEACUS_COMMAND_EVENT_H
#define AEACUS_COMMAND_EVENT_H

#pragma once

#include <event/message_event.h>
#include <api/message.h>

namespace aeacus
{
    class CommandEvent : public MessageEvent
    {
    public:
        explicit CommandEvent(const Message& message);

        void handle() const override;
        const Message& getMessage() const override;
    };
}

#endif //AEACUS_COMMAND_EVENT_H
