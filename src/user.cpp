#include <user.h>
#include <utility>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>
#include <api/request.h>
#include <crypto/crypto.h>

namespace aeacus
{
    User::User(std::string username, std::string publicKey, std::string secretKey, std::string vaultKey)
            : m_Username(std::move(username)), m_PublicKey(std::move(publicKey)), m_SecretKey(std::move(secretKey)),
            m_VaultKey(std::move(vaultKey)),
              m_LastAck(0)
    {
    }

    User* User::create(const std::string &username, const std::string &password)
    {
        auto* req = new APIRequest("getSalt", false);
        req->setParam("uname", username);

        auto response = req->send(nullptr, nullptr);
        std::string salt = response["salt"];

        std::string vaultKey = pbkdf2(username + password, salt, 32);
        std::string authKey = pbkdf2(vaultKey + password, salt, 16);

        delete req;
        req = new APIRequest("getUser", false);
        req->setParam("uname", username);
        req->setParam("authKey", authKey);

        response = req->send(nullptr, nullptr);
        std::cout << response << std::endl;

        std::string publicKey = response["user"]["publicKey"];
        std::string secretKey = response["user"]["secretKey"];

        auto* result = new User(username, publicKey, secretKey, vaultKey);

        return result;
    }

    const std::string& User::getUsername() const
    {
        return m_Username;
    }

    const std::string& User::getKey() const
    {
        return m_PublicKey;
    }

    Token User::getToken() const
    {
        Token t = generateToken(m_SecretKey, m_VaultKey);
        if (!verifySignature(t.token, t.signature, m_PublicKey))
            std::cerr << "Generated an invalid token!" << std::endl;
        return t;
    }

    std::vector<Message> User::getNewMessages()
    {
        using namespace nlohmann;
        auto* req = new APIRequest("getNewMessages", true);
        auto token = getToken();
        auto response = req->send(&m_Username, &token);
        std::vector<Message> result = {};

        m_LastAck = response["messagePackage"]["lastAcknowledgedTimestamp"];
        json messages = response["messagePackage"]["messages"];
        if (messages.empty())
        {
            delete req;
            return std::move(result);
        }
        else
        {
            for (const json& msgJson : messages)
            {
                Message msg = Message::fromJSON(msgJson);
                result.push_back(msg);
            }

            // Client must sort the array to avoid missing a timestamp due to not processing the data
            // in order.
            std::sort(result.begin(), result.end());
            std::reverse(result.begin(), result.end());
            std::vector<long> toAck = {};
            std::vector<long> bad = {};
            for (int i = (int)result.size() - 1; i >= 0; i--)
            {
                bool validSignature = result[i].verify(m_PublicKey);
                if (validSignature && result[i].payload->timestamp > m_LastAck)
                {
                    m_LastAck = result[i].payload->timestamp;
                    toAck.push_back(m_LastAck);
                }
                else
                {
                    bad.push_back(result[i].payload->timestamp);
                    if (!validSignature)
                        std::cout << "Invalid signature for timestamp " << result[i].payload->timestamp << std::endl;
                    else
                        std::cout << "Bad timestamp. Last ack: " << m_LastAck <<
                            ". We got " << result[i].payload->timestamp << std::endl;

                    result.erase(result.begin() + i);
                }
            }

            delete req;

            req = new APIRequest("acknowledgeMessages", true);
            req->setParam("timestamps", toAck);
            req->setParam("bad", bad); // Specify which messages are bad and should be discarded
            req->send(&m_Username, &token);

            delete req;
            return std::move(result);
        }
    }

    UserContext* UserContext::s_Instance;
    UserContext::UserContext(const std::string& username, const std::string& password)
    {
        m_User = User::create(username, password);
    }

    UserContext& UserContext::get()
    {
        return *s_Instance;
    }

    void UserContext::create(const std::string& username, const std::string& password)
    {
        auto* context = new UserContext(username, password);
        s_Instance = context;
    }

    void UserContext::destroy()
    {
        delete s_Instance;
    }

    User &UserContext::getUser()
    {
        return *m_User;
    }

    UserContext::~UserContext()
    {
        delete m_User;
    }
}