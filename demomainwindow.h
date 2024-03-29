#ifndef DEMOMAINWINDOW_H
#define DEMOMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class DemoMainWindow;
}

class DemoMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DemoMainWindow(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *event) override;

public:
    ~DemoMainWindow();

private:
    Ui::DemoMainWindow *ui;
};

#endif // DEMOMAINWINDOW_H
