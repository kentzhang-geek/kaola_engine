#ifndef PAINTEDWIDGET_H
#define PAINTEDWIDGET_H

#include <QWidget>
#include <QPainter>

class PaintedWidget : public QWidget {

public:
    PaintedWidget();

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // PAINTEDWIDGET_H
