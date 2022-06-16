#include <crypto/crypto.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <util/base64.h>
#include <sodium.h>
#include <openssl/sha.h>
#include <cstring>
#include <openssl/pem.h>

namespace aeacus
{
    void sha256Digest(const std::string& msg, unsigned char* digest) {
        auto* sha = new SHA256_CTX;
        SHA256_Init(sha);
        SHA256_Update(sha, msg.c_str(), msg.size());

        SHA256_Final(digest, sha);
    }

    bool verifySignature(const std::string& msg, const std::string& signature, const std::string& keyStr)
    {
        RSA* rsa = nullptr;
        BIO* mem;

        mem = BIO_new_mem_buf(keyStr.c_str(), -1);
        PEM_read_bio_RSA_PUBKEY(mem, &rsa, nullptr, nullptr);

        auto* digest = new unsigned char[SHA256_DIGEST_LENGTH];
        sha256Digest(msg, digest);

        auto sigBuf = new unsigned char[signature.size()];
        auto sigBufLen = 0;
        sodium_hex2bin(reinterpret_cast<unsigned char* const>(sigBuf), signature.size() / 2,
                       signature.c_str(), signature.size(), nullptr, reinterpret_cast<size_t* const>(&sigBufLen), nullptr);

        auto result = RSA_verify(NID_sha256, digest, SHA256_DIGEST_LENGTH,
                                 sigBuf, sigBufLen, rsa);

        RSA_free(rsa);
        BIO_free(mem);
        delete[] sigBuf;
        delete[] digest;

        return result == 1;
    }

    int password_cb(char* buf, int size, int rwflag, void* u) {
        const char* password = (const char*)u;
        int sizeToWrite = (size < std::strlen(password)) ? size : std::strlen(password);
        std::memcpy(buf, password, sizeToWrite);
        return sizeToWrite;
    }

    std::string sign(const std::string& msg, const std::string& keyStr, const char* password)
    {
        RSA* rsa = nullptr;
        BIO* mem;

        mem = BIO_new_mem_buf(keyStr.c_str(), -1);

        PEM_read_bio_RSAPrivateKey(mem, &rsa, &password_cb, (void*)password);

        unsigned int siglen = RSA_size(rsa);
        auto* sigBuf = new unsigned char[siglen];

        auto* digest = new unsigned char[SHA256_DIGEST_LENGTH];
        sha256Digest(msg, digest);

        auto result = RSA_sign(NID_sha256, digest, SHA256_DIGEST_LENGTH, sigBuf,
                               &siglen, rsa);

        if (result != 1)
            return "";

        auto* hex = new char[siglen * 2 + 1];
        const size_t hex_maxlen = siglen * 2 + 1;
        sodium_bin2hex(hex, hex_maxlen, sigBuf, siglen);

        std::string sigString(hex);

        delete[] digest;
        delete[] hex;
        delete[] sigBuf;

        RSA_free(rsa);
        BIO_free(mem);

        return sigString;
    }

    std::string pbkdf2(const std::string& password, const std::string& salt, size_t keyLen)
    {
        auto salt_decoded = base64_decode(salt);

        auto* resultBytes = new unsigned char[keyLen];
        PKCS5_PBKDF2_HMAC(
            password.c_str(), password.size(),
            reinterpret_cast<const unsigned char*>(salt_decoded.c_str()),
            salt_decoded.size(), 64000, EVP_sha256(),
            keyLen, resultBytes
        );

        return base64_encode(resultBytes, keyLen);
    }

    Token generateToken(const std::string& keyStr, const std::string& vaultKey)
    {
        char buf[16];
        randombytes_buf(buf, 16);

        char hex[33];
        sodium_bin2hex(hex, 33, reinterpret_cast<const unsigned char* const>(buf), 16);

        std::string token = hex;
        std::string sig = sign(token, keyStr, vaultKey.c_str());

        return { token, sig };
    }
}