#include <user.h>
#include <utility>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>
#include <api/request.h>
#include <crypto/crypto.h>
#include <csignal>
#include <pwd.h>
#include <fstream>
#include <sys/stat.h>
#include <glog/logging.h>

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
        std::string publicKey = response["user"]["publicKey"];
        std::string secretKey = response["user"]["secretKey"];

        std::string expectedHmac = response["user"]["hmac"];
        std::string hmac = hmacSha256(vaultKey, publicKey + secretKey + salt);

        if (hmac != expectedHmac)
            LOG(ERROR) << "User HMAC verification failure. Someone may be messing with our stuff!" << std::endl;

        auto* result = new User(username, publicKey, secretKey, vaultKey);

        delete req;
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
        bool valid = verifySignature(t.token, t.signature, m_PublicKey);
        if (!valid)
            LOG(ERROR) << "Generated an invalid token!" << std::endl;
        return t;
    }

    std::vector<Message> User::getNewMessages()
    {
        using namespace nlohmann;
        auto* req = new APIRequest("getNewMessages", true);
        auto token = getToken();
        json response;
        try
        {
            response = req->send(&m_Username, &token);
        } catch (APIException& e)
        {
            LOG(ERROR) << "API Exception!" << std::endl;
            LOG(ERROR) << '\t' << e.what() << std::endl;
            return {};
        }

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
                        LOG(INFO) << "Invalid signature for timestamp " << result[i].payload->timestamp << std::endl;
                    else
                        LOG(INFO) << "Bad timestamp. Last ack: " << m_LastAck <<
                            ". We got " << result[i].payload->timestamp << std::endl;

                    result.erase(result.begin() + i);
                }
            }

            delete req;

            req = new APIRequest("acknowledgeMessages", true);
            req->setParam("timestamps", toAck);
            req->setParam("bad", bad); // Specify which messages are bad and should be discarded

            try
            {
                req->send(&m_Username, &token);
            } catch (APIException& e)
            {
                LOG(ERROR) << "API Exception!" << std::endl;
                LOG(ERROR) << '\t' << e.what() << std::endl;
            }

            delete req;
            return std::move(result);
        }
    }

    User* User::fromJSON(nlohmann::json json)
    {
        std::string username = json["username"];
        std::string publicKey = json["publicKey"];
        std::string secretKey = json["secretKey"];
        std::string vaultKey = json["vaultKey"];
        return new User(username, publicKey, secretKey, vaultKey);
    }

    nlohmann::json User::serialize() const
    {
        using namespace nlohmann;
        json result;

        result["username"] = m_Username;
        result["publicKey"] = m_PublicKey;
        result["secretKey"] = m_SecretKey;
        result["vaultKey"] = m_VaultKey;

        return result;
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

    User* UserContext::getUser()
    {
        return m_User;
    }

    UserContext::~UserContext()
    {
        delete m_User;
    }

    void UserContext::save() const
    {
        LOG(INFO) << "Saving user context..." << std::endl;
        LOG(INFO) << "Fetching local username..." << std::endl;
        uid_t uid = geteuid();
        passwd* pw = getpwuid(uid);
        std::string sysUsername = pw->pw_name;

        LOG(INFO) << "whoami: " << sysUsername << std::endl;

        std::string path = "/home/" + sysUsername + "/.aeacus";
        if (!std::filesystem::exists(path))
        {
            LOG(INFO) << "Credentials directory doesn't exist. Creating..." << std::endl;
            std::filesystem::create_directories(path);
            chmod(path.c_str(), 0700);
        }

        path += "/user.json";

        LOG(INFO) << "Dumping user to " << path << "..." << std::endl;
        std::ofstream fs(path);
        fs << m_User->serialize().dump();
        fs.close();

        LOG(INFO) << "Userdump successful" << std::endl;
        chmod(path.c_str(), 0600);
    }

    bool UserContext::recall()
    {
        LOG(INFO) << "Recalling user context..." << std::endl;
        LOG(INFO) << "Fetching local username..." << std::endl;

        uid_t uid = geteuid();
        passwd* pw = getpwuid(uid);
        std::string sysUsername = pw->pw_name;

        LOG(INFO) << "whoami: " << sysUsername << std::endl;

        std::string path = "/home/" + sysUsername + "/.aeacus/user.json";
        if (!std::filesystem::exists(path))
        {
            LOG(ERROR) << "Couldn't find user credentials!" << std::endl;
            return false;
        }

        LOG(INFO) << "User credentials path: " << path << std::endl;

        std::ifstream fs(path);
        std::stringstream stream;
        stream << fs.rdbuf();
        std::string contents = stream.str();

        using namespace nlohmann;
        try
        {
            json userJSON = json::parse(contents);
            s_Instance = new UserContext(User::fromJSON(userJSON));
            LOG(INFO) << "Recall finished" << std::endl;
            return true;
        }
        catch (std::exception& e) {
            LOG(ERROR) << "Couldn't parse credentials!" << std::endl;
            LOG(ERROR) << e.what() << std::endl;
            return false;
        }
    }

    UserContext::UserContext(User* user)
    {
        m_User = user;
    }
}