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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化日志及配置
    init();

    m_stackedWidegt = new QStackedWidget(this);
    this->setCentralWidget(m_stackedWidegt);
    loginDialog = new LoginDialog(this);

    QRect mainRect = this->geometry();
    QRect loginRect = loginDialog->geometry();

    int x=mainRect.x()+(mainRect.width()-loginRect.width())/2;
    int y =mainRect.y()+(mainRect.height()-loginRect.width())/2;
    loginRect.moveTo(x,y);

    m_tokenManager = new TokenManager();

    loginDialog->show();

    connect(loginDialog,&LoginDialog::loginSuccess,this,&MainWindow::on_loginSuccess);
    connect(loginDialog,&QDialog::rejected,this,&QMainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init()
{
    this->setWindowTitle("欢迎登录 行政资产管理系统");
    this->setFixedSize(1280,900);

    Logger::getInstance().initialize();

    //加载配置
    ConfigManager::getInstance().loadConfig();
    m_serverUrl=ConfigManager::getInstance().getServerUrl();

    //测试日志
    LOG_INFO("Application initialized");
    LOG_INFO("Server URL: " + m_serverUrl);

    this->hide();
}

void MainWindow::on_loginSuccess(const QJsonObject &data)
{
    LOG_DEBUG("处理数据");
    loadMainContent();
    save_UserInfo(data);
    m_tokenManager->saveToken();
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

    // 显示主窗口
    this->show();
}

void MainWindow::save_UserInfo(const QJsonObject &data)
{
    Q_UNUSED(data);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(m_showBackGround)
    {
        QPainter backGround(this);
        backGround.drawPixmap(0,0,QPixmap(":mainWindow/backgroud/image/background/mainBackground.jfif"));
    }
}

