#pragma once
#include <string>
#include "utils/json_util.h"

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

class ConfigManager
{
private:
    std::string configPath_;
    std::unordered_map<std::string,json> configs_;
public:
    ConfigManager(/* args */);
    ~ConfigManager();
    static ConfigManager& getInstance();
    bool loadConfig(const std::string& configName = "log.json");
    json getConfig(const std::string& configName) const;
};

#endif //CONFIG_MANAGER_H