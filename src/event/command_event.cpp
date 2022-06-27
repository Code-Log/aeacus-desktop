#include <event/command_event.h>
#include <iostream>
#include <glog/logging.h>

namespace aeacus
{
    CommandEvent::CommandEvent(const Message& message)
    {
        m_Message = message;
    }

    void CommandEvent::handle() const
    {
        auto* payload = reinterpret_cast<CommandPayload*>(m_Message.payload);
        if (payload->command == "shutdown")
        {
            LOG(INFO) << "Received shutdown command!" << std::endl;
            LOG(INFO).flush();
            std::system("shutdown now");
        }
    }

    const Message& CommandEvent::getMessage() const
    {
        return m_Message;
    }
}