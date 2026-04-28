#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include<QJsonArray>

class ConfigManager
{

private:
    QString m_serverUrl;
    int m_timeout;
public:
    ConfigManager();
    ~ConfigManager();
    static ConfigManager& getInstance();
    bool loadConfig(const QString& configfile = "config.json");
    QString getServerUrl()const;
    int getTimeout()const;
};

#endif // CONFIGMANAGER_H
