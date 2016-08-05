#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/utils/qui/myhelper.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myHelper::FormInCenter(this);
    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_testtiaozhuan_clicked()
{
    dhw = new drawhomewin(this->parentWidget());
    dhw->show();
}
