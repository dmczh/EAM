// utils/PasswordUtils.cpp
#include "PasswordUtils.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <regex>
#include <random>
#include "utils/logger.h"
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "utils/logger.h"

namespace eam_utils
{
    // Base64 编码实现
    std::string PasswordUtils::base64Encode(const unsigned char *input, int length)
    {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_write(bio, input, length);
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);

        std::string result(bufferPtr->data, bufferPtr->length);
        BIO_free_all(bio);

        return result;
    }

    // Base64 解码实现
    std::vector<unsigned char> PasswordUtils::base64Decode(const std::string &input)
    {
        BIO *bio, *b64;
        size_t decodeLength = input.length();
        std::vector<unsigned char> result(decodeLength);

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(input.c_str(), -1);
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        int length = BIO_read(bio, result.data(), decodeLength);
        result.resize(length > 0 ? length : 0);

        BIO_free_all(bio);
        return result;
    }

    // SHA256 哈希实现
    std::string PasswordUtils::sha256(const std::string &input)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input.c_str(), input.length());
        SHA256_Final(hash, &sha256);

        return base64Encode(hash, SHA256_DIGEST_LENGTH);
    }

    // 生成随机字节
    std::vector<unsigned char> PasswordUtils::generateRandomBytes(int length)
    {
        std::vector<unsigned char> buffer(length);

        // 使用 OpenSSL 的随机数生成
        if (RAND_bytes(buffer.data(), length) != 1)
        {
            // 如果 OpenSSL 随机数生成失败，使用标准库
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);

            for (int i = 0; i < length; ++i)
            {
                buffer[i] = static_cast<unsigned char>(dis(gen));
            }
        }

        return buffer;
    }

    // 生成盐值
    std::string PasswordUtils::generateSalt(int length)
    {
        auto bytes = generateRandomBytes(length);
        return base64Encode(bytes.data(), bytes.size());
    }

    // 简单的哈希实现（仅用于兼容）
    std::string PasswordUtils::simpleHash(const std::string &password, const std::string &salt)
    {
        std::string combined = password + salt;
        return sha256(combined);
    }

    // 使用 PBKDF2 进行密码哈希（生产级安全）
    std::string PasswordUtils::hashPassword(const std::string &password)
    {
        const int iterations = 100000; // 10万次迭代
        const int saltLen = 16;        // 16字节盐
        const int keyLen = 32;         // 32字节输出

        // 1. 生成随机盐
        unsigned char salt[saltLen];
        if (RAND_bytes(salt, saltLen) != 1)
        {
            EAM_LOG_ERROR("Failed to generate random salt for password hashing");
            return "";
        }

        // 2. 使用 PBKDF2 派生密钥
        unsigned char derivedKey[keyLen];
        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                              salt, saltLen, iterations,
                              EVP_sha256(), keyLen, derivedKey) != 1)
        {
            EAM_LOG_ERROR("PBKDF2 key derivation failed");
            return "";
        }

        // 3. 编码并返回
        std::string saltBase64 = base64Encode(salt, saltLen);
        std::string hashBase64 = base64Encode(derivedKey, keyLen);

        // 格式: iterations$salt$hash
        return std::to_string(iterations) + "$" + saltBase64 + "$" + hashBase64;
    }

    // 验证密码
    bool PasswordUtils::verifyPassword(const std::string &password, const std::string &hash)
    {
        // 解析存储的哈希
        size_t firstSep = hash.find('$');
        size_t secondSep = hash.find('$', firstSep + 1);

        if (firstSep == std::string::npos || secondSep == std::string::npos)
        {
            EAM_LOG_WARNING("Invalid password hash format");
            return false;
        }

        int iterations = std::stoi(hash.substr(0, firstSep));
        std::string saltBase64 = hash.substr(firstSep + 1, secondSep - firstSep - 1);
        std::string expectedHashBase64 = hash.substr(secondSep + 1);

        // 解码盐和期望的哈希
        auto salt = base64Decode(saltBase64);
        auto expectedHash = base64Decode(expectedHashBase64);

        if (salt.empty() || expectedHash.empty())
        {
            EAM_LOG_WARNING("Failed to decode password hash components");
            return false;
        }

        // 重新计算哈希
        unsigned char derivedKey[expectedHash.size()];
        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                              salt.data(), salt.size(), iterations,
                              EVP_sha256(), expectedHash.size(), derivedKey) != 1)
        {
            EAM_LOG_ERROR("PBKDF2 verification failed");
            return false;
        }

        // 比较结果（使用常量时间比较更安全，但这里简化）
        std::string computedHashBase64 = base64Encode(derivedKey, expectedHash.size());
        return computedHashBase64 == expectedHashBase64;
    }

    // 生成随机密码
    std::string PasswordUtils::generateRandomPassword(int length)
    {
        const std::string charset =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789"
            "!@#$%^&*";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, charset.size() - 1);

        std::string password;
        password.reserve(length);

        for (int i = 0; i < length; ++i)
        {
            password += charset[dis(gen)];
        }

        return password;
    }

    // 检查密码强度
    PasswordStrength PasswordUtils::checkPasswordStrength(const std::string &password)
    {
        PasswordStrength result;
        result.score = 0;
        result.isValid = true;

        if (password.empty())
        {
            result.isValid = false;
            result.message = "密码不能为空";
            result.score = 0;
            return result;
        }

        // 检查长度
        if (password.length() < 6)
        {
            result.suggestions.push_back("密码长度至少为6个字符");
            result.score = 0;
        }
        else if (password.length() < 8)
        {
            result.score = 1;
            result.suggestions.push_back("建议增加密码长度到8位以上");
        }
        else if (password.length() < 10)
        {
            result.score = 2;
        }
        else
        {
            result.score = 3;
        }

        // 检查字符类型
        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;

        for (char c : password)
        {
            if (std::isupper(c))
                hasUpper = true;
            else if (std::islower(c))
                hasLower = true;
            else if (std::isdigit(c))
                hasDigit = true;
            else if (std::ispunct(c))
                hasSpecial = true;
        }

        int typeCount = 0;
        if (hasUpper)
            typeCount++;
        if (hasLower)
            typeCount++;
        if (hasDigit)
            typeCount++;
        if (hasSpecial)
            typeCount++;

        // 根据字符类型数量加分
        result.score += typeCount - 1;

        // 检查常见密码模式
        std::vector<std::string> commonPasswords = {
            "password", "123456", "qwerty", "admin", "letmein",
            "welcome", "monkey", "dragon", "master", "login"};

        std::string lowerPassword = password;
        std::transform(lowerPassword.begin(), lowerPassword.end(),
                       lowerPassword.begin(), ::tolower);

        for (const auto &common : commonPasswords)
        {
            if (lowerPassword.find(common) != std::string::npos)
            {
                result.suggestions.push_back("避免使用常见密码或字典单词");
                result.score = std::max(0, result.score - 2);
                break;
            }
        }

        // 检查连续字符
        bool hasSequence = false;
        for (size_t i = 0; i < password.length() - 2; ++i)
        {
            if (password[i + 1] == password[i] + 1 &&
                password[i + 2] == password[i] + 2)
            {
                hasSequence = true;
                break;
            }
        }

        if (hasSequence)
        {
            result.suggestions.push_back("避免使用连续的字符序列（如abc、123）");
            result.score = std::max(0, result.score - 1);
        }

        // 检查重复字符
        bool hasRepeat = false;
        for (size_t i = 0; i < password.length() - 2; ++i)
        {
            if (password[i] == password[i + 1] && password[i] == password[i + 2])
            {
                hasRepeat = true;
                break;
            }
        }

        if (hasRepeat)
        {
            result.suggestions.push_back("避免使用重复的字符（如aaa、111）");
            result.score = std::max(0, result.score - 1);
        }

        // 限制最高分
        result.score = std::min(4, result.score);

        // 生成建议消息
        if (result.score <= 1)
        {
            result.message = "密码强度较弱，请使用更复杂的密码";
        }
        else if (result.score == 2)
        {
            result.message = "密码强度中等，建议增加复杂度";
        }
        else
        {
            result.message = "密码强度良好";
        }

        result.isValid = result.score >= 1;

        return result;
    }

} // namespace eam_utils