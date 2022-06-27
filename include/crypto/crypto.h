#ifndef AEACUS_CRYPTO_H
#define AEACUS_CRYPTO_H

#pragma once

#include <string>
#include <api/token.h>

namespace aeacus
{
    bool verifySignature(const std::string& msg, const std::string& signature, const std::string& keyStr);
    std::string sign(const std::string& msg, const std::string& keyStr);
    std::string pbkdf2(const std::string& password, const std::string& salt, size_t keyLen);
    Token generateToken(const std::string& keyStr, const std::string& vaultKey);
    std::string hmacSha256(const std::string& secret, const std::string& msg);
    void secureFree(void* ptr, size_t size);
}

#endif //AEACUS_CRYPTO_H
