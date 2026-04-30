#include "tokenmanager.h"
#include "logger.h"
#include <QSettings>
#include <QString>


void TokenManager::checkTokenExpiry()
{
    if(!m_currentToken.isValid()){
        if(!m_currentToken.asscess_token.isEmpty())
        {
            LOG_WARNING("token is expired");
            emit tokenExpired();
        }
    }

    if(m_currentToken.needRefresh())
    {
        emit tokenExpireSoon();
    }
}

TokenManager::TokenManager():m_expiryCheckeTimer{new QTimer(this)} {
    m_currentToken = loadToken();
    m_expiryCheckeTimer->setInterval(60000); //每分钟检查一次
    connect(m_expiryCheckeTimer,&QTimer::timeout,this,&TokenManager::checkTokenExpiry);
    m_expiryCheckeTimer->start();

    if(m_currentToken.isValid())
    {
        LOG_INFO("Loaded valid token for user: " + m_currentToken.username);
    }else {
        LOG_WARNING("Loaded expired token, clearing...");
        this->clearToken();
    }
    connect(this,&TokenManager::tokenExpireSoon,this,&TokenManager::refreshToken);
}

TokenManager::~TokenManager()
{

}

TokenManager &TokenManager::getInstance()
{
    static TokenManager instance;
    return instance;
}

TokenData TokenManager::loadToken()
{
    TokenData data;
    QSettings settings("Arany","asset_backend");

    QString encryptedAccessToken = settings.value("auth/access_token").toString();
    if (encryptedAccessToken.isEmpty()) {
        return data;
    }

    data.asscess_token = settings.value("auth/access_token").toString();
    data.userId = settings.value("auth/user_id").toInt();
    data.username =settings.value("auth/username").toString();
    data.expires_at = settings.value("auth/expires_at").toDateTime();

    QStringList permList = settings.value("auth/permissions").toStringList();
    for (const auto& perm : permList) {
        data.permissions.append(perm);
    }

    return data;
}

void TokenManager::saveToken(const QString &access_token, int expires_in, int user_id, const QString &username, const QList<QString> &permissions)
{
    QSettings settings("Arany","asset_backend");
    settings.setValue("auth/access_token",access_token);
    settings.setValue("auth/expies_at",QDateTime::currentDateTime().addSecs(expires_in));
    settings.setValue("auth/user_id",user_id);
    settings.setValue("auth/username",username);

    QStringList perms;
    for (const auto&perm:permissions ) {
        perms.append(perm);
    }

    settings.setValue("auth/permissions",perms);

    m_currentToken.asscess_token=access_token;
    m_currentToken.expires_at = QDateTime::currentDateTime().addSecs(expires_in);
    m_currentToken.userId = user_id;
    m_currentToken.username=username;
    m_currentToken.permissions=permissions;

    settings.sync();

    LOG_INFO(QString("Token saved for user: %1, expires at: %2")
                 .arg(username)
                 .arg(m_currentToken.expires_at.toString()));
}

void TokenManager::refreshToken()
{
    qDebug()<<"更新token";
}

void TokenManager::clearToken()
{
    QSettings settings("Arany","asset_backend");
    settings.remove("auth/access_token");
    settings.remove("auth/expies_at");
    settings.remove("auth/user_id");
    settings.remove("auth/username");

    m_currentToken.clean();

    settings.sync();
}

bool TokenManager::isLoggedIn()
{
    if(m_currentToken.isValid())
    {
        return true;
    }
    return false;
}
