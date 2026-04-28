#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include "logindialog.h"
#include <QPaintEvent>
#include <QStackedWidget>
#include "mainpagewidget.h"
#include "tokenmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void init();

private:
    Ui::MainWindow *ui;
    QString m_serverUrl;
    LoginDialog *loginDialog=nullptr;
    QStackedWidget *m_stackedWidegt=nullptr;
    MainPageWidget *m_mainPage=nullptr;
    bool m_showBackGround=true;
    TokenManager *m_tokenManager=nullptr;

    void on_loginSuccess(const QJsonObject& data);
    void loadMainContent();
    void save_UserInfo(const QJsonObject& data);

};
#endif // MAINWINDOW_H