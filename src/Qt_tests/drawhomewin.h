#ifndef DRAWHOMEWIN_H
#define DRAWHOMEWIN_H

#include <QWidget>
#include <QShowEvent>
#include "ui/drawoption.h"

namespace Ui {
class drawhomewin;
}

class drawhomewin : public QWidget
{
    Q_OBJECT

public:
    explicit drawhomewin(QWidget *parent = 0);
    ~drawhomewin();

    static void on_draw_clear();

private slots:
    void on_switch_2_clicked();

    void on_tempdraw_clicked();

    void on_drawwall_b_stateChanged(int arg1);

private:
    Ui::drawhomewin *ui;
    DrawOption *dop;

protected:
    void showEvent(QShowEvent *ev);
};

#endif // DRAWHOMEWIN_H
