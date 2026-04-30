#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "configmanager.h"
#include <QDir>
#include "logger.h"
#include <QRect>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include "tokenmanager.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_timer(new QTimer(this))
{
    ui->setupUi(this);
    //初始化日志及配置
    init();
    loadLoginDialog();

    m_stackedWidegt = new QStackedWidget(this);
    this->setCentralWidget(m_stackedWidegt);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->setWindowTitle("欢迎登录 行政资产管理系统");
    this->setFixedSize(1366,768);

    Logger::getInstance().initialize();

    //加载配置
    ConfigManager::getInstance().loadConfig();
    m_serverUrl=ConfigManager::getInstance().getServerUrl();

    //测试日志
    LOG_INFO("Application initialized");
    LOG_INFO("Server URL: " + m_serverUrl);

    this->hide();

    //加载背景图
    m_backGrounds.append(QSharedPointer<QPixmap>::create(":mainWindow/backgroud/image/background/mainBackground.jfif"));
    m_backGrounds.append(QSharedPointer<QPixmap>::create(":/mainWindow/backgroud/image/background/mainBackground2.jfif"));

    m_timer->setInterval(5000);
    connect(m_timer,&QTimer::timeout,this,&MainWindow::on_switchBackGround);
    m_timer->start();
}

void MainWindow::on_loginSuccess(const QJsonObject &data)
{
    LOG_DEBUG("处理数据");
    loadMainContent();
    save_UserInfo(data);
}

void MainWindow::loadMainContent()
{
    this->setWindowTitle("欢迎使用资产管理系统");

    m_showBackGround=false;

    m_mainPage = new MainPageWidget();

    m_mainPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *vLayout = new QVBoxLayout(m_mainPage);

    m_mainPage->setAttribute(Qt::WA_TranslucentBackground);
    m_mainPage->setStyleSheet("background-color: transparent;");

    QLabel *welcomeLabel = new QLabel("欢迎使用资产管理系统", m_mainPage);
    vLayout->addWidget(welcomeLabel);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "font-size: 28px;"
        "color: white;"
        "background-color: rgba(0, 0, 0, 100);"
        "padding: 30px;"
        "border-radius: 15px;"
        "font-weight: bold;"
        );

    m_mainPage->show();
    m_stackedWidegt->addWidget(m_mainPage);
    m_stackedWidegt->setCurrentWidget(m_mainPage);

    m_stackedWidegt->resize(this->size());
    m_stackedWidegt->setGeometry(this->rect());

    this->show();
}

void MainWindow::save_UserInfo(const QJsonObject &data)
{
    QJsonObject dataInfo = data["data"].toObject();
    QString access_token = dataInfo.value("access_token").toString();
    int expires_in = dataInfo["expires_in"].toInt();

    QJsonObject userInfo = dataInfo["user"].toObject();

    int user_id = userInfo.value("user_id").toInt();
    QString username = userInfo.value("username").toString();
    qDebug()<<username;
    QJsonArray jsonArray = userInfo.value("permissions").toArray();
    QList<QString> permissions;
    for (const QJsonValue& perm:jsonArray) {
        QString permission = perm.toString();
        permissions.append(permission);
    }
    TokenManager::getInstance().saveToken(access_token,expires_in,user_id,username,permissions);
}

void MainWindow::loadLoginDialog()
{
    loginDialog = new LoginDialog(this);
    loginDialog->show();

    QRect mainRect = this->geometry();
    QRect loginRect = loginDialog->geometry();

    int x=mainRect.x()+(mainRect.width()-loginRect.width())/2;
    int y =mainRect.y()+(mainRect.height()-loginRect.width())/2;
    loginRect.moveTo(x,y);

    connect(loginDialog,&LoginDialog::loginSuccess,this,&MainWindow::on_loginSuccess);
    connect(loginDialog,&QDialog::rejected,this,&QMainWindow::close);
}

void MainWindow::on_switchBackGround()
{
    if(m_showBackGround && !m_backGrounds.empty()){
        m_backGroundIndex = (m_backGroundIndex+1)%m_backGrounds.size();
    }
    this->update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(m_showBackGround)
    {
        QPainter backGround(this);
        QPixmap courrentBackGround=*m_backGrounds[m_backGroundIndex];
        backGround.drawPixmap(0,0,courrentBackGround.scaled(this->size(),Qt::KeepAspectRatioByExpanding));
    }
}

