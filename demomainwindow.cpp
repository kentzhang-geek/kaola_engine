#include "demomainwindow.h"
#include "ui_demomainwindow.h"

DemoMainWindow::DemoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DemoMainWindow)
{
    ui->setupUi(this);
}

DemoMainWindow::~DemoMainWindow()
{
    delete ui;
}
