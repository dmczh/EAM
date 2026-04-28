#ifndef MAINPAGEWIDGET_H
#define MAINPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class MainPageWidget;
}

class MainPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainPageWidget(QWidget *parent = nullptr);
    ~MainPageWidget();

private:
    Ui::MainPageWidget *ui;
};

#endif // MAINPAGEWIDGET_H
