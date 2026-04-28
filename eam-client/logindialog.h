#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "httpclient.h"

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

private:
    Ui::LoginDialog *ui;
    HttpClient *m_client=nullptr;
    void on_loginSuccess(const QJsonObject& data);

signals:
   void loginSuccess(const QJsonObject& data);
};

#endif // LOGINDIALOG_H
