#include "models/ConfigManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <utils/logger.h>
#include <utils/json_util.h>

ConfigManager::ConfigManager() {}

ConfigManager::~ConfigManager() {}

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

// 加载配置文件
bool ConfigManager::loadConfig(const std::string &configName)
{

    std::vector<std::string> tempPath = {
        "../config/" + configName,               // 从 build 目录运行
        "./config/" + configName,                // 从项目根目录运行
        configName,                              // 直接路径
        "/home/eam-server/config/" + configName, // 绝对路径
        "/var/logs/EAM/config/" + configName,
        "/var/logs/EAM/" + configName};

    std::ifstream configFile;

    for (std::string path : tempPath)
    {
        configFile.open(path);
        if (configFile.is_open())
        {
            configPath_ = path;
            EAM_LOG_INFO("Config path: " + configPath_);

            json jsonDoc;
            try
            {
                configFile >> jsonDoc;
                configs_[configName] = jsonDoc;

                return true;
            }
            catch (const json::parse_error &e)
            {
                EAM_LOG_ERROR("JSON parse error in file: " + configPath_);
                EAM_LOG_ERROR("ERROR details: " + e.what());
                return false;
            }
        }
    }
    EAM_LOG_WARNING("Connot find config file!");
    return false;
    // std::string configFile = "../config/"+configName;
    // std::ifstream logFile(configFile);
    // if (!logFile.is_open())
    // {
    //     EAM_LOG_WARNING("Connot open this logconfig file: "+configFile);
    //     return;
    // }

    // if (jsonDoc.contains("logDir"))
    // {
    //     std::string logdir = jsonDoc["logDir"];
    //     EAM_LOG_DEBUG("Log directory: " + logdir);
    // }
}

json ConfigManager::getConfig(const std::string &configName) const
{
    auto it = configs_.find(configName);
    if (it != configs_.end())
        return it->second;

    return json::object();
}
