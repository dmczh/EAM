#include "httpclient.h"
#include "configmanager.h"
#include "logger.h"
#include <QUrl>
#include <QJsonDocument>
#include <QDebug>
#include <QPointer>

HttpClient::HttpClient(QObject *parent)
    :QObject(parent)
    ,m_network(new QNetworkAccessManager(this))
    ,m_timeout(ConfigManager::getInstance().getTimeout())
    ,m_baseUrl(ConfigManager::getInstance().getServerUrl())
    ,m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    connect(m_timer,&QTimer::timeout,this,&HttpClient::on_Timeout);
}

HttpClient::~HttpClient()
{

}

void HttpClient::on_Timeout()
{
    LOG_WARNING("Request is timeout!");
}

void HttpClient::sendRequest(const QString &method, const QString &path, const QByteArray &data, std::function<void (bool, const QJsonObject &)> callback)
{
    QUrl url(m_baseUrl+path);
    qDebug()<<url.path();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QNetworkReply *reply = nullptr;

    QString lowermethod = method.toLower();
    if(lowermethod =="get")
        reply = m_network->get(request);
    else if(lowermethod == "post")
        reply= m_network->post(request,data);
    else if(lowermethod == "put")
        reply = m_network->put(request,data);
    else if(lowermethod == "delete")
        reply = m_network->deleteResource(request);

    QPointer<QNetworkReply> safeReply = reply;

    auto callbackPtr = std::make_shared<std::function<void(bool,const QJsonObject&)>>(callback);

    connect(reply,&QNetworkReply::finished,[this,safeReply,callbackPtr](){
        m_timer->stop();
        LOG_INFO("Process http callback");
        //处理响应及回调
        if(!safeReply.isNull()){
            handleResponse(safeReply,callbackPtr);
            safeReply->deleteLater();
        }else {
            LOG_WARNING("Reply was destroyed before finished");
            if (callbackPtr && *callbackPtr) {
                QJsonObject errorJson;
                errorJson["error"] = "Request cancelled";
                (*callbackPtr)(false, errorJson);
            }
        }
    });
}

void HttpClient::handleResponse(QNetworkReply *reply,std::shared_ptr<std::function<void(bool,const QJsonObject&)>> callbackPtr)
{
    bool httpOk = false;
    QJsonObject responseJson;
    QVariant statusCodeVariant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int httpStatusCode = statusCodeVariant.toInt();

    if(reply->error()==QNetworkReply::NoError)
    {
        if(httpStatusCode>=200 && httpStatusCode<300)
            httpOk = true;
        else
        {
            httpOk = false;
            LOG_WARNING("Http error"+QString::number(httpStatusCode));
        }
        QByteArray responseData = reply->readAll();
        if(responseData.isEmpty())
        {
            LOG_ERROR("Data is Empty");
            return;
        }
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(!jsonDoc.isNull()&&jsonDoc.isObject()){
            responseJson = jsonDoc.object();
        }
    }else{
        httpOk = false;
        LOG_WARNING("Network Error"+reply->errorString());
    }
    if(*callbackPtr)
    {
        LOG_DEBUG("Process programmer callback");
        (*callbackPtr)(httpOk,responseJson);
    }
}

void HttpClient::login(const QString &username, const QString &password)
{
    QJsonObject json;
    json["username"]=username;
    json["password"]=password;

    post("/api/login",json,[this, username](bool httpOk,const QJsonObject& data){
        if(!httpOk){
            LOG_WARNING(QString("HTTP request failed for user: %1").arg(username));
            return;
        }

        bool bizSuccess = data.value("success").toBool();
        if(bizSuccess){
            LOG_DEBUG("Callback success, process data next!");
            emit loginSuccess(data);
        }else
        {
            int code = data.value("code").toInt();
            QString message = data.value("message").toString();
            LOG_WARNING(QString("user: %1 login failed, code: %2,message: %3").arg(username).arg(code).arg(message));
        }
    });
}

void HttpClient::get(const QString &path, std::function<void (bool, const QJsonObject &)> callback)
{
    sendRequest("GET",path,QByteArray(),callback);
}

void HttpClient::post(const QString &path, const QJsonObject &data, std::function<void (bool, const QJsonObject &)> callback)
{
    QByteArray jsondata = QJsonDocument(data).toJson();
    sendRequest("POST",path,jsondata,callback);
}
