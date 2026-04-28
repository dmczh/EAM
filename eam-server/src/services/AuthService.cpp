#include "AuthService.h"
#include <string>
#include "utils/PasswordUtils.h"
#include "utils/JwtUtils.h"
#include "utils/logger.h"

LoginResult AuthService::login(const std::string &username, const std::string &password, const std::string &ip, const std::string &userAgent)
{
    // 1. 输入验证
    if (username.empty() || password.empty())
    {
        return LoginResult::failureResult("Username and password are required");
    }
    // 2. 验证用户是否存在
    auto user = userDao_->getUserByUsername(username);

    EAM_LOG_INFO("Login attempt for username: " + username + " from IP: " + ip + " with user agent: " + userAgent);
    if (!user)
    {
        userDao_->recordLoginAttempt(username, ip, userAgent, false);
        return LoginResult::failureResult("User not found");
    }
    auto &userInfo = user.value();
    // 3. 验证密码
    if (!eam_utils::PasswordUtils::verifyPassword(password, userInfo.passwordHash))
    {
        userDao_->recordLoginAttempt(username, ip, userAgent, false);
        return LoginResult::failureResult("Invalid password");
    }
    // userDao_->recordLoginAttempt(username, ip, userAgent, true);

    // // 4.检查用户状态
    // if (userInfo.status  != "active")
    // {
    //     userDao_->recordLoginAttempt(username, ip, userAgent, false);
    //     return LoginResult::failureResult("User account is not active");
    // }

    // 5.获取用户权限
    auto permissions = userDao_->getUserPermissions(userInfo.id);

    // 6.. 生成token

    std::string token = eam_utils::JwtUtils::generateToken(userInfo.id, userInfo.username, permissions); 

    userDao_->updateLastLogin(userInfo.id);

    // userDao_->logLoginAttempt(username, ip, userAgent, true);
     return LoginResult::successResult(userInfo.id, userInfo.username, token, permissions);
}
