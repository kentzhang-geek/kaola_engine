#include "mainwindow.h"
#include <iostream>
#include "ui_mainwindow.h"
using namespace std;

extern MOpenGLView * one_view;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(sig1()), this, SLOT(on_sig1()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    exit(0);
}

void MainWindow::on_sig1() {
    ui->tout->setText(ui->tout->toPlainText() + "qqq");
    ui->lcdn->setHexMode();
    ui->lcdn->display(ui->lcdn->intValue() + 1);
}

void MainWindow::on_qqq_clicked()
{
    static bool kai = true;
    one_view->setCon(kai);
    kai = !kai;
    emit sig1();
}
