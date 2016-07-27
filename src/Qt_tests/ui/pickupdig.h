#ifndef PICKUPDIG_H
#define PICKUPDIG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d.hpp"
#include "editor/gl3d_wall.h"

class PickupDig:public QDialog
{
    Q_OBJECT
public:
    PickupDig(QWidget* parent, int x, int y, int pickUpObjID, gl3d::scene * sc);
    void initBasicInfo();
private slots:
    void slotDoubleSpinbox_Slider2();
    void slotSlider_DoubleSpinbox2();
    void slotDoubleSpinbox_Slider3();
    void slotSlider_DoubleSpinbox3();

    void on_delete_obj();
private:
    QWidget* baseWidget;
    QSlider *slider2;
    QSlider *slider3;
    QDoubleSpinBox *spinBox2;
    QDoubleSpinBox *spinBox3;

    int pickUpObjID;
    gl3d::object *pickUpObj;
    gl3d::scene *main_scene;
    gl3d::gl3d_wall *wall;
};

#endif // PICKUPDIG_H
