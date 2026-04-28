#include "configmanager.h"
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "logger.h"

ConfigManager::ConfigManager() {}
ConfigManager::~ConfigManager(){}

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const QString &configfile)
{
    QFile file(configfile);
    if(!file.open(QIODevice::ReadOnly))
    {
        LOG_WARNING("Cannot open config file: "+configfile);
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if(jsonDoc.isNull())
    {
        LOG_WARNING("Invalid JSON config file");
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();

    QString serverIP = jsonObj["host_ip"].toString("http://localhost");
    int port = jsonObj["port"].toInt(8080);
    m_serverUrl=serverIP+":"+QString::number(port);
    m_timeout = jsonObj["timeout"].toInt(30000);

    return true;
}

QString ConfigManager::getServerUrl() const
{
    return m_serverUrl;
}

int ConfigManager::getTimeout() const
{
    return m_timeout;
}
