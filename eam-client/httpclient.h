#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QByteArray>
#include <QJsonObject>
#include <functional>


class HttpClient:public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *m_network;
    int m_timeout;
    QString m_baseUrl;

    QTimer *m_timer=nullptr;
    void on_Timeout();

    void sendRequest(const QString& method,const QString& path,const QByteArray& data,std::function<void(bool,const QJsonObject&)> callback);
    void handleResponse(QNetworkReply* reply,std::shared_ptr<std::function<void(bool,const QJsonObject&)>> callback);

public:
    HttpClient(QObject* parent =nullptr);
    ~HttpClient();

    void login(const QString& username,const QString& password);
    void get(const QString& path,std::function<void(bool,const QJsonObject&)> callback);
    void post(const QString& path, const QJsonObject& data,std::function<void(bool,const QJsonObject& )> callback);

signals:
    void loginSuccess(const QJsonObject& data);
    void loginFailed();
};

#endif // HTTPCLIENT_H