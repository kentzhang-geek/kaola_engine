#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>
#include <iostream>
#include "drawhomewin.h"
#include "resource_and_network/network_tool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    klm::network::login_tool * lg;

public slots:
    void on_login_signin_button_clicked();
    void process_login(bool isok);

private slots:
    void on_testtiaozhuan_clicked();

private:
    drawhomewin * dhw;
};

#endif // MAINWINDOW_H
