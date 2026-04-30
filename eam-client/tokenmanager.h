#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QTimer>

struct TokenData{
    QString username;
    int userId;
    QString asscess_token;
    QDateTime expires_at;
    QList<QString> permissions;

    bool isValid() const{
        return !asscess_token.isEmpty() && QDateTime::currentDateTime()<expires_at;
    }
    bool needRefresh() const{
        return isValid() && QDateTime::currentDateTime().addSecs(300)>=expires_at;
    }
    void clean(){
        username.clear();
        userId=0;
        asscess_token.clear();
        permissions.clear();
    }
};

class TokenManager:public QObject
{
    Q_OBJECT

private:
    TokenData m_currentToken;
    QTimer *m_expiryCheckeTimer;

    void checkTokenExpiry();
public:
    TokenManager();
    ~TokenManager();

    static TokenManager& getInstance();
    TokenData loadToken();
    void saveToken(const QString& access_token,int expires_in,int user_id,const QString& username,const QList<QString>& permissions);
    void refreshToken();
    void clearToken();
    bool isLoggedIn();

signals:
    void tokenExpireSoon();
    void tokenExpired();
};

#endif // TOKENMANAGER_H
