#include "mainpagewidget.h"
#include "ui_mainpagewidget.h"

MainPageWidget::MainPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPageWidget)
{
    ui->setupUi(this);
}

MainPageWidget::~MainPageWidget()
{
    delete ui;
}
