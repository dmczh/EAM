// utils/PasswordUtils.h
#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

namespace eam_utils {

class PasswordUtils {
public:
    // 生成密码的哈希值
    static std::string hashPassword(const std::string& password);
    
    // 验证密码是否匹配
    static bool verifyPassword(const std::string& password, const std::string& hash);
    
    // 生成随机密码
    static std::string generateRandomPassword(int length = 12);
    
    // 检查密码强度
    static struct PasswordStrength checkPasswordStrength(const std::string& password);
    
    // 生成盐值
    static std::string generateSalt(int length = 16);
    
    // 简单的密码加密（不推荐用于生产环境，建议使用bcrypt）
    static std::string simpleHash(const std::string& password, const std::string& salt);
    
private:
    // 使用 OpenSSL 的 SHA256 哈希（不推荐，仅用于简单场景）
    static std::string sha256(const std::string& input);
    
    // Base64 编码
    static std::string base64Encode(const unsigned char* input, int length);
    
    // Base64 解码
    static std::vector<unsigned char> base64Decode(const std::string& input);
    
    // 生成随机字节
    static std::vector<unsigned char> generateRandomBytes(int length);
};

// 密码强度结构体
struct PasswordStrength {
    bool isValid;
    int score;  // 0-4 分
    std::string message;
    std::vector<std::string> suggestions;
    
    // 强度等级
    enum Level {
        VERY_WEAK = 0,
        WEAK = 1,
        MEDIUM = 2,
        STRONG = 3,
        VERY_STRONG = 4
    };
    
    Level getLevel() const {
        if (score <= 1) return VERY_WEAK;
        if (score == 2) return MEDIUM;
        if (score == 3) return STRONG;
        return VERY_STRONG;
    }
    
    std::string getLevelString() const {
        switch(getLevel()) {
            case VERY_WEAK: return "非常弱";
            case WEAK: return "弱";
            case MEDIUM: return "中等";
            case STRONG: return "强";
            case VERY_STRONG: return "非常强";
            default: return "未知";
        }
    }
};

} // namespace eam_utils