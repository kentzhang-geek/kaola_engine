#ifndef PICKUPDIG_H
#define PICKUPDIG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSlider>

class PickupDig:public QDialog
{
    Q_OBJECT
public:
    PickupDig(QWidget* parent, int x, int y);
    void initBasicInfo();
private:
    QWidget* baseWidget;
};

#endif // PICKUPDIG_H
