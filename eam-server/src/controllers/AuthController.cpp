#include "AuthController.h"
#include <string>
#include "utils/json_util.h"
#include "utils/logger.h"
#include "utils/JwtUtils.h"

std::shared_ptr<UserDao> AuthController::userDao_ = nullptr;
std::shared_ptr<AuthService> AuthController::authService_ = nullptr;

void AuthController::init(std::shared_ptr<UserDao> userDao, std::shared_ptr<AuthService> authService)
{
    {
        userDao_ = userDao;
        authService_ = authService;
        eam::Logger::getInstance().log(eam::LogLevel::INFO, "AuthController dependencies initialized");
    }
}

void AuthController::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    eam::Logger::getInstance().log(eam::LogLevel::INFO, "Received login request from IP: " + req->getPeerAddr().toIp());

    try
    {
        auto json = req->getJsonObject();
        if (!json)
        {
            callback(AuthController::createErrorResponse(400, "Invalid Json"));
            return;
        }
        std::string username = (*json)["username"].asString();
        std::string password = (*json)["password"].asString();

        EAM_LOG_DEBUG("Attempting login for user: " + username);

        // 获取客户端信息
        std::string ip = req->getPeerAddr().toIp();
        std::string userAgent = req->getHeader("User-Agent");
        // 调用service层进行登录验证
        LoginResult result = authService_->login(username, password, ip, userAgent);

        eam::Logger::getInstance().log(eam::LogLevel::INFO, "Login attempt for user: " + username + ", IP: " + ip + ", User-Agent: " + userAgent + ", Success: " + (result.success ? "true" : "false"));

        if (result.success)
        {
            Json::Value response;
            response["success"] = true;
            response["code"] = 200;
            response["timestamp"] = std::time(nullptr);

            Json::Value data;
            data["access_token"] = result.token;
            data["expires_in"] = eam_utils::JwtUtils::getExpiresIn();

            Json::Value userInfo;
            userInfo["user_id"] = result.userId;
            userInfo["username"] = result.username;
            userInfo["permissions"] = Json::Value(Json::arrayValue);
            for (const auto &perm : result.permissions)
            {
                userInfo["permissions"].append(perm);
            }
            data["user"] = userInfo;
            response["data"] = data;

            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
            callback(resp);
        }
        else
        {
            callback(AuthController::createErrorResponse(401, "Invalid username or password"));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

drogon::HttpResponsePtr AuthController::createErrorResponse(int code, const std::string &message)
{
    Json::Value response;
    response["success"] = false;
    response["code"] = code;
    response["error"] = message;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(static_cast<drogon::HttpStatusCode>(code));
    return resp;
}
