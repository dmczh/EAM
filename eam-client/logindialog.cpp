#include "logindialog.h"
#include "httpclient.h"
#include "ui_logindialog.h"
#include <QJsonObject>
#include "logger.h"
#include <QMessageBox>
#include <QRect>
#include <QSettings>

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
    connect(&TokenManager::getInstance(),&TokenManager::tokenExpired,this,&LoginDialog::on_tokenExpired);

    QSettings settings("eam","userInfo");

    if(!settings.value("username").toString().isEmpty()&&!settings.value("password").toString().isEmpty())
    {
        m_username=settings.value("username").toString();
        m_password =settings.value("password").toString();

        this->ui->username->setText(m_username);
        this->ui->password->setText(m_password);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::on_btn_login_clicked()
{
    QString username;
    QString password;
    if(!m_username.isEmpty()&&!m_password.isEmpty())
    {
        username =m_username;
        password =m_password;
    }else{
        username = this->ui->username->text();
        password = this->ui->password->text();
    }

    if(username.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"警告","请输入用户名或密码");
    }

    QSettings settings("eam","userInfo");

    username = this->ui->username->text();
    password = this->ui->password->text();

    settings.setValue("username",username);
    settings.setValue("password",password);
    settings.sync();

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

void LoginDialog::on_tokenExpired()
{
    logOut();
}


void LoginDialog::on_btn_Cancle_clicked()
{
    reject();
}

void LoginDialog::logOut()
{
    this->show();
}

