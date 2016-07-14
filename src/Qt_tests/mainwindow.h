#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_qqq_clicked();

    void on_sig1();

    void on_pushButton_2_clicked();

signals:
    void sig1();

private:
    Ui::MainWindow *ui;

protected:
    void showEvent(QShowEvent *ev);
};

#endif // MAINWINDOW_H
