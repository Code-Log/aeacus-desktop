#include <util/InvalidArgumentException.h>

InvalidArgumentException::InvalidArgumentException(const char *message)
    : m_Message(message)
{
}

const char* InvalidArgumentException::what() const noexcept
{
    return m_Message;
}
