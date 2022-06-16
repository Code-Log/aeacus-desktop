#include <event/listener.h>

namespace aeacus
{
    ListenerContext &ListenerContext::get()
    {
        static ListenerContext s_Instance;
        return s_Instance;
    }

    ListenerContext::ListenerContext()
        : m_Listeners()
    {
    }

    void ListenerContext::addListener(const std::weak_ptr<EventListener>& listener)
    {
        m_Listeners.push_back(listener);
    }

    void ListenerContext::removeListener(const std::weak_ptr<EventListener>& listener)
    {
        for (int i = (int)m_Listeners.size() - 1; i >= 0; i--)
        {
            if (m_Listeners[i].lock() == listener.lock())
                m_Listeners.erase(m_Listeners.begin() + i);
        }
    }

    void ListenerContext::event(const MessageEvent &event) const
    {
        for (const auto& listener : m_Listeners)
        {
            if (std::shared_ptr<EventListener> ptr = listener.lock())
            {
                ptr->event(event);
            }
        }
    }
}