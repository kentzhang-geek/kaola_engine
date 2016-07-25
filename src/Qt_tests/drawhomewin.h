#ifndef DRAWHOMEWIN_H
#define DRAWHOMEWIN_H

#include <QWidget>
#include <QShowEvent>

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

    void on_drawwall_b_clicked(bool checked);
private:
    Ui::drawhomewin *ui;

protected:
    void showEvent(QShowEvent *ev);
};

#endif // DRAWHOMEWIN_H
