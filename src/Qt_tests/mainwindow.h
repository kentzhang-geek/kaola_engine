#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>
#include <iostream>
#include "drawhomewin.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_login_signin_button_clicked();

private slots:
    void on_testtiaozhuan_clicked();

private:
    drawhomewin * dhw;
};

#endif // MAINWINDOW_H
