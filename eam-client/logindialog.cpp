#include "logindialog.h"
#include "httpclient.h"
#include "ui_logindialog.h"
#include <QJsonObject>
#include "logger.h"
 #include <QMessageBox>
#include <QRect>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::SplashScreen);
    this->setModal(true);

    this->resize(350,250);
    this->setMinimumSize(350,250);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    this->setWindowOpacity(0.9);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::on_btn_login_clicked()
{
    QString username = this->ui->username->text();
    QString password = this->ui->password->text();

    if(username.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"警告","请输入用户名或密码");
    }

    this->ui->btn_login->setEnabled(false);

    LOG_DEBUG(QString("user: %1 is attempt to login  ").arg(username));

    if(m_client==nullptr)
    {
        LOG_DEBUG("Httpclient is not created!");
        m_client=new HttpClient(this);
        connect(m_client,&HttpClient::loginSuccess,this,&LoginDialog::on_loginSuccess);
    }

    LOG_DEBUG("Prepare to call httpclient login method......");
    m_client->login(username,password);
    LOG_DEBUG("Call httpclient login method success");
    this->ui->btn_login->setEnabled(true);
}

void LoginDialog::on_loginSuccess(const QJsonObject &data)
{
    emit loginSuccess(data);
    accept();
}


void LoginDialog::on_btn_Cancle_clicked()
{
    reject();
}

