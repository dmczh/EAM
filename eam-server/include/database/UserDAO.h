#pragma once
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include "models/user.h"
#include <memory>
#include <optional>

class UserDao
{
private:
    std::shared_ptr<drogon::orm::DbClient> dbClient_;

public:
    explicit UserDao(std::shared_ptr<drogon::orm::DbClient> dbClient) : dbClient_(dbClient) {}

    std::optional<model::User> getUserByUsername(const std::string &username);
    std::optional<model::User> getUserById(int64_t id);
    void updateLastLogin(int64_t userId);
    void recordLoginAttempt(const std::string &username, const std::string &ip, const std::string &userAgent, bool success);
    std::vector<std::string> getUserPermissions(int64_t userId);
};
