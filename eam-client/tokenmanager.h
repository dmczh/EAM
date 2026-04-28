#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <QObject>

class TokenManager:public QObject
{
    Q_OBJECT
public:
    TokenManager();
    void saveToken();
    void refreshToken();
    void verifyToken();
};

#endif // TOKENMANAGER_H
