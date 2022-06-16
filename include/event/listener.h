#ifndef AEACUS_LISTENER_H
#define AEACUS_LISTENER_H

#pragma once

#include "message_event.h"

namespace aeacus
{
    class EventListener
    {
    public:
        virtual void event(const MessageEvent& event) const = 0;
    };

    class ListenerContext
    {
    private:
        std::vector<std::weak_ptr<EventListener>> m_Listeners;
        ListenerContext();
    public:
        static ListenerContext& get();

        void event(const MessageEvent& event) const;
        void addListener(const std::weak_ptr<EventListener>& listener);
        void removeListener(const std::weak_ptr<EventListener>& listener);
    };
}

#endif //AEACUS_LISTENER_H
