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
    bool on_load_reload;
    ~MainWindow();
    void resizeEvent(QResizeEvent *ev);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void on_login_signin_button_clicked();
    void process_login(bool isok);
    void test_text(const QString &text);
    void web_loaded(bool ib);
    void login(const QString & uname, const QString & pwd);
    void open_sketch(const QString & plan_id, const QString & design_id, const QString & path, const QString & pdn);
    void open_sketch_change_plan(const QString & plan_id, const QString & design_id, const QString & path, const QString & pdn);
    void reg_info(const QString & housename, const QString & tag);
    void on_catch_url(QString url);

private slots:
    void on_testtiaozhuan_clicked();

private:
    drawhomewin * dhw;
};

#endif // MAINWINDOW_H
