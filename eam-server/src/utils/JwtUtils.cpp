// utils/JwtUtils.cpp
#include "JwtUtils.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <utils/logger.h>
#include <nlohmann/json.hpp>

namespace eam_utils
{
    std::string JwtUtils::secretKey_ = "";
    int JwtUtils::expiresInSeconds_ = 3600;

    std::string JwtUtils::getSecretKey()
    {
        return secretKey_;
    }

    int JwtUtils::getExpiresIn()
    {
        return expiresInSeconds_;
    }

    void JwtUtils::init(const std::string &key, int expiresInSeconds)
    {
        if (key.empty())
        {
            EAM_LOG_WARNING("JWT secret key is empty, using default secret key. Please set a strong secret key in production!");
            return;
        }
        if (key.length() < 32)
        {
            EAM_LOG_WARNING("JWT secret key is too short, it should be at least 32 characters for security. Please set a stronger secret key in production!");
            return;
        }

        secretKey_ = key;
        expiresInSeconds_ = expiresInSeconds;
    }

    std::string JwtUtils::generateToken(int userId,
                                        const std::string &username,
                                        const std::vector<std::string> &permissions,
                                        int expiresInSeconds)
    {
        if (secretKey_.empty())
        {
            EAM_LOG_WARNING("JWT secret key is empty, using default secret key. Please set a strong secret key in production!");
            return "";
        }

        try
        {
            int expiresSec = expiresInSeconds > 0 ? expiresInSeconds : expiresInSeconds_;
            auto now = std::chrono::system_clock::now();
            auto expires = now + std::chrono::seconds(expiresSec);

            std::string permissionsJson = "[";
            for (size_t i = 0; i < permissions.size(); ++i)
            {
                if (i > 0)
                    permissionsJson += ",";
                permissionsJson += "\"" + permissions[i] + "\"";
            }
            permissionsJson += "]";
            // 创建 JWT Token
            auto token = jwt::create()
                             .set_issuer("asset-backend")
                             .set_type("JWT")
                             .set_subject(std::to_string(userId))
                             .set_issued_at(now)
                             .set_expires_at(expires)
                             .set_payload_claim("username", jwt::claim(username))
                             .set_payload_claim("permissions", jwt::claim(permissionsJson));

            return token.sign(jwt::algorithm::hs256{secretKey_});
        }
        catch (const std::exception &e)
        {
            EAM_LOG_ERROR("Failed to generate JWT token: " + std::string(e.what()));
            return "";
        }
    }

    bool JwtUtils::validateToken(const std::string &token,
                                 int &userId,
                                 std::string &username,
                                 std::vector<std::string> &permissions,
                                 const std::string &secretKey)
    {
        std::string key = secretKey.empty() ? getSecretKey() : secretKey;

        if (key.empty())
        {
            return false; // 没有密钥无法验证
        }
        try
        {

            auto decoded = jwt::decode(token);

            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{key})
                                .with_issuer("asset-backend");

            verifier.verify(decoded);

            userId = std::stoi(decoded.get_subject());
            username = decoded.get_payload_claim("username").as_string();

            auto permsClaim = decoded.get_payload_claim("permissions");
            std::string permsStr = permsClaim.as_string();

            if (!permsStr.empty() && permsStr != "null")
            {

                // 解析 JSON 数组: ["perm1","perm2"]
                size_t pos = 0;
                while (pos < permsStr.length())
                {
                    pos = permsStr.find('"', pos);
                    if (pos == std::string::npos)
                        break;

                    size_t end = permsStr.find('"', pos + 1);
                    if (end == std::string::npos)
                        break;

                    std::string perm = permsStr.substr(pos + 1, end - pos - 1);
                    if (!perm.empty())
                    {
                        permissions.push_back(perm);
                    }

                    pos = end + 1;
                }
            }
            return true;
        }
        catch (const std::exception &e)
        {
            userId = -1;
            username = "";
            permissions.clear();
            return false;
        }
    }

    int JwtUtils::getUserIdFromToken(const std::string &token,
                                     const std::string &secretKey)
    {
        try
        {
            std::string key = secretKey.empty() ? getSecretKey() : secretKey;
            auto decoded = jwt::decode(token);

            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{key})
                                .with_issuer("asset-backend");

            verifier.verify(decoded);

            return std::stoi(decoded.get_subject());
        }
        catch (const std::exception &e)
        {
            return -1;
        }
    }

    std::string JwtUtils::getUsernameFromToken(const std::string &token,
                                               const std::string &secretKey)
    {
        try
        {
            std::string key = secretKey.empty() ? getSecretKey() : secretKey;
            auto decoded = jwt::decode(token);

            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{key})
                                .with_issuer("asset-backend");

            verifier.verify(decoded);

            return decoded.get_payload_claim("username").as_string();
        }
        catch (const std::exception &e)
        {
            return "";
        }
    }

    bool JwtUtils::isTokenExpired(const std::string &token,
                                  const std::string &secretKey)
    {
        std::string key = secretKey.empty() ? getSecretKey() : secretKey;

        if (key.empty())
        {
            return true; // 没有密钥无法验证，认为是过期的
        }
        try
        {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{key})
                                .with_issuer("asset-backend");

            verifier.verify(decoded);
            return false; // 验证通过，token未过期
        }
        catch (const jwt::error::token_verification_exception &e)
        {
            return true; // token过期
        }
        catch (const std::exception &e)
        {
            return false; // 其他验证错误不认为是过期
        }
    }

} // namespace eam_utils