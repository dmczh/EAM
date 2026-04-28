#pragma once
#include "database/UserDAO.h"
#include <vector>

struct LoginResult
{
    bool success;
    std::string token;
    int userId;
    std::string username;
    std::string message;
    std::vector<std::string> permissions;

    static LoginResult successResult(int userId, const std::string &username, const std::string &token, const std::vector<std::string> &permissions)
    {
        LoginResult result;
        result.success = true;
        result.token = token;
        result.userId = userId;
        result.username = username;
        result.permissions = permissions;
        return result;
    }

    static LoginResult failureResult(const std::string &message)
    {
        LoginResult result;
        result.success = false;
        result.message = message;
        return result;
    }
};

class AuthService
{
private:
    std::shared_ptr<UserDao> userDao_;

public:
    explicit AuthService(std::shared_ptr<UserDao> userDao) : userDao_(userDao) {};
    ~AuthService() {};

    LoginResult login(const std::string &username, const std::string &password, const std::string &ip, const std::string &userAgent);
};
