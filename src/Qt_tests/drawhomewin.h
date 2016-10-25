#ifndef DRAWHOMEWIN_H
#define DRAWHOMEWIN_H

#include <QWidget>
#include <QShowEvent>
#include <QtWebView>
#include <QtWebView/QtWebView>
#include <QtWebEngineWidgets>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebChannel>
#include <QtWebChannel/QWebChannel>
#include "ui/drawoption.h"

namespace Ui {
class drawhomewin;
}

class drawhomewin : public QWidget
{
    Q_OBJECT

public:
    explicit drawhomewin(QWidget *parent = 0);
    bool on_load_may_need_reload_web;
    ~drawhomewin();

    static void on_draw_clear();

    void load_sketch();

    QWebEngineView * web;
    QWebChannel * channel;

    // save actions
    void new_plan();
    void new_design();
    void change_plan();
    void change_design();

public slots:
    void webload_finished(bool isok);

private slots:
    void on_switch3D_clicked();

    void on_colse_b_clicked();

    void on_p_4_clicked();

    void on_p_1_clicked();

    void on_p_2_clicked();

    void on_restore_clicked();

    void on_undo_clicked();

    void on_p_7_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_p_11_clicked();

    void on_p_12_clicked();

    void on_save_b_clicked();

    void save_ok();

private:
    Ui::drawhomewin *ui;
    DrawOption *dop;

    void clear_bg_color();

protected:
    void showEvent(QShowEvent *ev);
    void closeEvent(QCloseEvent *event);
};

#endif // DRAWHOMEWIN_H
