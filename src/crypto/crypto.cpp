#include <crypto/crypto.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <util/base64.h>
#include <sodium.h>
#include <openssl/sha.h>
#include <cstring>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <iostream>
#include <glog/logging.h>

namespace aeacus
{
    void sha256Digest(const std::string& msg, unsigned char* digest)
    {
        auto* sha = new SHA256_CTX;
        SHA256_Init(sha);
        SHA256_Update(sha, msg.c_str(), msg.size());

        SHA256_Final(digest, sha);

        delete sha;
    }

    bool verifySignature(const std::string& msg, const std::string& signature, const std::string& keyStr)
    {
        RSA* rsa;
        BIO* mem;

        mem = BIO_new_mem_buf(keyStr.c_str(), -1);
        rsa = PEM_read_bio_RSA_PUBKEY(mem, nullptr, nullptr, nullptr);

        auto* digest = new unsigned char[SHA256_DIGEST_LENGTH];
        sha256Digest(msg, digest);

        auto sigBuf = new unsigned char[signature.size()];
        size_t sigBufLen = 0;
        sodium_hex2bin(reinterpret_cast<unsigned char* const>(sigBuf), signature.size() / 2,
                       signature.c_str(), signature.size(), nullptr, &sigBufLen, nullptr);

        auto result = RSA_verify(NID_sha256, digest, SHA256_DIGEST_LENGTH,
                                 sigBuf, sigBufLen, rsa);

//        int result = 1;

        RSA_free(rsa);
        BIO_free(mem);

        secureFree(sigBuf, (int)sigBufLen);
        secureFree(digest, SHA256_DIGEST_LENGTH);

        return result == 1;
    }

    int password_cb(char* buf, int size, int rwflag, void* u)
    {
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
        rsa = PEM_read_bio_RSAPrivateKey(mem, nullptr, &password_cb, (void*)password);

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

        secureFree(digest, SHA256_DIGEST_LENGTH);
        secureFree(hex, (int)hex_maxlen);
        secureFree(sigBuf, (int)siglen);

        RSA_free(rsa);
        BIO_free(mem);

        return sigString;
    }

    std::string pbkdf2(const std::string& password, const std::string& salt, size_t keyLen)
    {
        auto salt_decoded = base64_decode(salt);

        auto* resultBytes = new unsigned char[keyLen];
        PKCS5_PBKDF2_HMAC(
            password.c_str(), (int)password.size(),
            reinterpret_cast<const unsigned char*>(salt_decoded.c_str()),
            (int)salt_decoded.size(), 310000, EVP_sha256(),
            (int)keyLen, resultBytes
        );

        std::string result = base64_encode(resultBytes, keyLen);

        secureFree(resultBytes, (int)keyLen);

        return result;
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

    /**
     * Securely deletes a block of heap memory by settings every byte
     * to 0x00 before calling delete[] on the pointer.
     * @param ptr The block of memory to be deleted
     * @param size The size of the data at the pointer in bytes
     */
    void secureFree(void* ptr, size_t size)
    {
        auto* p = reinterpret_cast<unsigned char*>(ptr);

        for (int i = 0; i < size; i++)
            p[i] = 0x00;

        delete[] p;
    }

    std::string hmacSha256(const std::string& secret, const std::string& msg)
    {
        std::string key = base64_decode(secret);

        void* keyBuf = new char[key.size()];
        int keyLen = (int)key.size();
        std::memcpy(keyBuf, key.c_str(), keyLen);

        unsigned char md[SHA256_DIGEST_LENGTH];
        unsigned int mdLen = 0;
        if (!HMAC(
            EVP_sha256(),
            keyBuf,
            keyLen,
            reinterpret_cast<const unsigned char*>(msg.c_str()),
            msg.size(),
            md,
            &mdLen
        ))
        {
            LOG(ERROR) << "An error occurred while calculating an HMAC!" << std::endl;
            return "";
        }

        char hex[SHA256_DIGEST_LENGTH * 2 + 1];
        sodium_bin2hex(hex, SHA256_DIGEST_LENGTH * 2 + 1, md, mdLen);

        secureFree(keyBuf, keyLen);

        return hex;
    }
}