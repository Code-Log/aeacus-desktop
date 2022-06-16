#include <event/command_event.h>

namespace aeacus
{
    CommandEvent::CommandEvent(const Message& message)
    {
        m_Message = message;
    }

    void CommandEvent::handle()
    {
    }

    Message &CommandEvent::getMessage()
    {
        return m_Message;
    }
}