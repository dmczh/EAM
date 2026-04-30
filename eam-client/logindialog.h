#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "httpclient.h"
#include "tokenmanager.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_btn_login_clicked();
    void on_btn_Cancle_clicked();
    void logOut();

private:
    Ui::LoginDialog *ui;
    HttpClient *m_client=nullptr;

    QString m_username;
    QString m_password;

    void on_loginSuccess(const QJsonObject& data);
    void on_tokenExpired();

signals:
   void loginSuccess(const QJsonObject& data);
};

#endif // LOGINDIALOG_H
