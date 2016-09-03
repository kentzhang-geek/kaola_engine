#ifndef DECORATEWIN_H
#define DECORATEWIN_H

#include <QWidget>

namespace Ui {
class decoratewin;
}

class decoratewin : public QWidget
{
    Q_OBJECT

public:
    explicit decoratewin(QWidget *parent = 0);
    ~decoratewin();

private:
    Ui::decoratewin *ui;
};

#endif // DECORATEWIN_H
