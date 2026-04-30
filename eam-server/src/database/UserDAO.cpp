#include "UserDAO.h"
#include "utils/logger.h"

std::optional<model::User> UserDao::getUserByUsername(const std::string &username)
{
    using namespace drogon::orm;
    try
    {
        auto result = dbClient_->execSqlSync("SELECT id, username, password_hash, email, phone,  account_non_expired,account_non_locked,enabled, created_at, updated_at, last_login_at, login_fail_count FROM users WHERE username = $1", username);
        if (!result.empty())
        {
            const auto &row = result[0];
            model::User user;
            user.id = (row["id"].as<int64_t>());
            user.username = (row["username"].as<std::string>());
            user.passwordHash = (row["password_hash"].as<std::string>());
            user.email = (row["email"].as<std::string>());
            user.phone = (row["phone"].as<std::string>());
            user.account_non_expired = (row["account_non_expired"].as<bool>());
            user.account_non_locked = (row["account_non_locked"].as<bool>());
            user.enabled = (row["enabled"].as<bool>());
            user.created_at = (row["created_at"].as<time_t>());
            user.updated_at = (row["updated_at"].as<time_t>());
            user.last_login_at = (row["last_login_at"].as<time_t>());
            user.login_fail_count = (row["login_fail_count"].as<int>());
            return user;
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Database error in getUserByUsername for username: " + username + ", error: " + e.base().what());
    }

    return std::nullopt;
}

std::optional<model::User> UserDao::getUserById(int64_t id)
{
    using namespace drogon::orm;
    try
    {
        auto result = dbClient_->execSqlSync("SELECT id, username, password_hash, email, phone,  account_non_expired,account_non_locked,enabled, created_at, updated_at, last_login_at, login_fail_count FROM users WHERE id = $1", id);
        if (!result.empty())
        {
            const auto &row = result[0];
            model::User user;
            user.id = (row["id"].as<int64_t>());
            user.username = (row["username"].as<std::string>());
            user.passwordHash = (row["password_hash"].as<std::string>());
            user.email = (row["email"].as<std::string>());
            user.phone = (row["phone"].as<std::string>());
            user.account_non_expired = (row["account_non_expired"].as<bool>());
            user.account_non_locked = (row["account_non_locked"].as<bool>());
            user.enabled = (row["enabled"].as<bool>());
            user.created_at = (row["created_at"].as<time_t>());
            user.updated_at = (row["updated_at"].as<time_t>());
            user.last_login_at = (row["last_login_at"].as<time_t>());
            user.login_fail_count = (row["login_fail_count"].as<int>());
            return user;
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Database error in getUserById for user ID: " + std::to_string(id) + ", error: " + e.base().what());
    }

    return std::nullopt;
}
// 记录最后登录时间
void UserDao::updateLastLogin(int64_t userId)
{
    dbClient_->execSqlAsync("UPDATE users SET last_login_at = NOW() WHERE id = $1", [userId](const drogon::orm::Result &result)
                            {
        if (result.affectedRows() == 0)
        {
            eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Failed to update last login time for user ID: " + std::to_string(userId));
        } }, [userId](const drogon::orm::DrogonDbException &e)
                            { eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Database error in updateLastLogin for user ID: " + std::to_string(userId) + ", error: " + e.base().what()); }, userId);
}

// 记录登录日志
void UserDao::recordLoginAttempt(const std::string &username, const std::string &ip, const std::string &userAgent, bool success)
{
    dbClient_->execSqlAsync("INSERT INTO login_attempts (username, ip_address, user_agent, success) VALUES ($1, $2, $3, $4)", [username](const drogon::orm::Result &result) {

    },
                            [username](const drogon::orm::DrogonDbException &e)
                            { eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Failed to record login attempt for username: " + username + ", " + e.base().what()); }, username, ip, userAgent, success);
}
// 获取用户权限列表
std::vector<std::string> UserDao::getUserPermissions(int64_t userId)
{
    using namespace drogon::orm;
    std::vector<std::string> permissions;
    try
    {
        auto result = dbClient_->execSqlSync("SELECT p.code FROM permissions p JOIN user_permissions up ON p.id = up.permission_id WHERE up.user_id = $1", userId);
        for (const auto &row : result)
        {
            permissions.push_back(row["code"].as<std::string>());
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        eam::Logger::getInstance().log(eam::LogLevel::ERROR, "Database error in getUserPermissions for user ID: " + std::to_string(userId) + ", " + e.base().what());
    }

    return permissions;
}
