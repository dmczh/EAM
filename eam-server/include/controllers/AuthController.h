#pragma once
#include <drogon/HttpController.h>
#include "database/UserDAO.h"
#include "services/AuthService.h"
#include <memory>
#include <functional>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
private:
    static std::shared_ptr<UserDao> userDao_;
    static std::shared_ptr<AuthService> authService_;

    drogon::HttpResponsePtr createErrorResponse(int code, const std::string &message);

public:
    METHOD_LIST_BEGIN
    // 登录接口
    ADD_METHOD_TO(AuthController::login, "/api/login", Post);
    METHOD_LIST_END

    AuthController() = default;
    ~AuthController() {};
    // 注入依赖
    static void init(std::shared_ptr<UserDao> userDao, std::shared_ptr<AuthService> authService);
    void login(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
