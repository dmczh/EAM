#pragma once
#include <jwt-cpp/jwt.h>
#include <string>
#include <vector>
#include <chrono>

namespace eam_utils
{
    class JwtUtils
    {
    private:
        static std::string secretKey_;
        static int expiresInSeconds_;
        // 获取密钥（可从配置文件读取）
        static std::string getSecretKey();
        // Base64 URL 编码
        static std::string base64UrlEncode(const std::string &input);

    public:
        static void init(const std::string &key, int expiresInHours = 24);
        // 生成 JWT Token
        static std::string generateToken(int userId,
                                         const std::string &username,
                                         const std::vector<std::string> &permissions,
                                         int expiresInHours = 24);

        // 验证 JWT Token
        static bool validateToken(const std::string &token,
                                  int &userId,
                                  std::string &username,
                                  std::vector<std::string> &permissions,
                                  const std::string &secretKey = "");

        // 从 Token 中获取用户 ID
        static int getUserIdFromToken(const std::string &token,
                                      const std::string &secretKey = "");

        // 从 Token 中获取用户名
        static std::string getUsernameFromToken(const std::string &token,
                                                const std::string &secretKey = "");

        // 检查 Token 是否过期
        static bool isTokenExpired(const std::string &token,
                                   const std::string &secretKey = "");
        static int getExpiresIn();
    };

} // namespace eam_utils