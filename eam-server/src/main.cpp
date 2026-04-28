#include "utils/logger.h"
#include <signal.h>
#include "utils/json_util.h"
#include "models/ConfigManager.h"
#include <drogon/drogon.h>
#include "controllers/AuthController.h"
#include "services/AuthService.h"
#include "database/UserDAO.h"
#include "utils/JwtUtils.h"
#include <unistd.h>
#include <utils/PasswordUtils.h>

#ifdef EAM_LOG_DEBUG
#pragma message("EAM_LOG_DEBUG is defined")
#else
#pragma message("EAM_LOG_DEBUG is NOT defined")
#error "EAM_LOG_DEBUG macro not found!"
#endif

void signalHandler(int)
{
    eam::Logger::getInstance().flush();
    _exit(0);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    JsonHelper::parse("\"str\"");
    // 初始化Log目录
    eam::Logger::getInstance().initialize();

    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != nullptr)
    {
        EAM_LOG_INFO("Current working directory: " + std::string(buffer));
    }
    else
    {
        EAM_LOG_ERROR("Failed to get current working directory");
    }

    // 加载配置文件
    auto log = ConfigManager::getInstance();

    json logConfig;

    if (log.loadConfig("log.json"))
    {
        logConfig = log.getConfig("log.json");
    }

    if (logConfig.contains("logDir"))
    {
        std::string logdir = logConfig["logDir"];
        EAM_LOG_DEBUG("Log directory: " + logdir);
    }

    ConfigManager::getInstance().loadConfig("config.json");
    auto config = ConfigManager::getInstance().getConfig("config.json");

    // 初始化HTTP服务器
    if (config.contains("server"))
    {
        // 设置监听的IP和端口
        std::string server = config["server"]["host"];
        int port = config["server"]["port"];
        drogon::app().addListener(server, port);
        EAM_LOG_INFO("Server listening on " + server + ":" + std::to_string(port));

        // 设置线程池大小
        int threadPoolSize = config["server"]["thread_pool_size"];
        drogon::app().setThreadNum(threadPoolSize);
        EAM_LOG_INFO("Thread pool size set to " + std::to_string(threadPoolSize));

        // 设置最大请求大小
        int maxRequestBodySize = config["server"]["max_request_size"];
        drogon::app().setClientMaxBodySize(maxRequestBodySize);
        EAM_LOG_INFO("Max request body size set to " + std::to_string(maxRequestBodySize));
    }
    else
    {
        EAM_LOG_WARNING("Server configuration not found, using default settings");
    }

    // 配置数据库连接
    EAM_LOG_INFO("Checking database configuration...");

    std::shared_ptr<drogon::orm::DbClient> dbClient;

    if (config.contains("database"))
    {
        EAM_LOG_INFO("Database config found, parsing...");

        std::string dbHost = config["database"]["host"];
        int dbPort = config["database"]["port"];
        std::string dbName = config["database"]["name"];
        std::string dbUser = config["database"]["user"];
        std::string dbPassword = config["database"]["password"];
        int dbPoolSize = config["database"]["pool_size"];

        if (dbPassword == "${DB_PASSWORD}")
        {
            const char *envPassword = std::getenv("DB_PASSWORD");
            if (envPassword)
            {
                dbPassword = envPassword;
                EAM_LOG_INFO("Database password loaded from environment variable");
            }
            else
            {
                EAM_LOG_WARNING("Environment variable DB_PASSWORD not set, using placeholder password");
            }
        }
        if (dbHost.empty() || dbName.empty() || dbUser.empty())
        {
            EAM_LOG_ERROR("Database configuration is incomplete, missing host, dbname, or user");
            return 1;
        }

        std::string connectionInfo = "host=" + dbHost +
                                     " port=" + std::to_string(dbPort) +
                                     " dbname=" + dbName +
                                     " user=" + dbUser +
                                     " password=" + dbPassword;

        // 使用drogon的newPgClient方法创建数据库连接池
        dbClient = drogon::orm::DbClient::newPgClient(connectionInfo, dbPoolSize);

        std::shared_ptr<UserDao> userDao = std::make_shared<UserDao>(dbClient);
        std::shared_ptr<AuthService> authService = std::make_shared<AuthService>(userDao);

        // 注入依赖到控制器
        AuthController::init(userDao, authService);
        // drogon::app().registerController(std::make_shared<AuthController>()); //不需要手动注册，Drogon会自动注册
        EAM_LOG_INFO("Database client configured with host: " + dbHost + ", port: " + std::to_string(dbPort) + ", dbname: " + dbName);
    }
    else
    {
        EAM_LOG_WARNING("Database configuration not found, database client not initialized");
    }

    // 配置JWT密钥
    if (config.contains("jwt"))
    {
        auto jwt = config["jwt"];
        std::string secretKey = config["jwt"]["secret"];
        int expiresInSeconds = jwt.value("expires_in", 3600); // 以秒为单位
        eam_utils::JwtUtils::init(secretKey, expiresInSeconds);
        EAM_LOG_INFO("JWT secret key configured");
    }
    else
    {
        EAM_LOG_WARNING("JWT configuration not found, using default settings");
    }
    drogon::app().run();
    return 0;
}