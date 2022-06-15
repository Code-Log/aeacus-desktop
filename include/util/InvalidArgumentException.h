#include <exception>

class InvalidArgumentException : public std::exception
{
private:
    const char* m_Message;
public:
    explicit InvalidArgumentException(const char* message);
    [[nodiscard]] const char* what() const noexcept override;
};

#ifndef AEACUS_INVALIDARGUMENTEXCEPTION_H
#define AEACUS_INVALIDARGUMENTEXCEPTION_H

#endif //AEACUS_INVALIDARGUMENTEXCEPTION_H
