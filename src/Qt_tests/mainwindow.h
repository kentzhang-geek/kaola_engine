#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>
#include <iostream>
#include <QtWebView>
#include <QtWebView/QtWebView>
#include <QtWebEngineWidgets>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebChannel>
#include <QtWebChannel/QWebChannel>
#include "drawhomewin.h"
#include "resource_and_network/network_tool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString text)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QWebEngineView * web;
    void resizeEvent(QResizeEvent *ev);
    QWebChannel * channel;

    klm::network::login_tool * lg;
    void keyPressEvent(QKeyEvent *event);

public slots:
    void on_login_signin_button_clicked();
    void process_login(bool isok);
    void test_text(const QString &text);
    void web_loaded(bool ib);
    void login(const QString & uname, const QString & pwd);
    void open_sketch(const QString & plan_id, const QString & design_id, const QString & path);

private slots:
    void on_testtiaozhuan_clicked();

private:
    drawhomewin * dhw;
};

#endif // MAINWINDOW_H
