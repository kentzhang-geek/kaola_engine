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
#include "editor/design_scheme.h"

class PickupDig:public QDialog
{
    Q_OBJECT
public:
    PickupDig(QWidget *parent, int x, int y, int pickUpObjID, gl3d::scene *sc, klm::design::scheme *sch,
              glm::vec2 cd_scr);
    ~PickupDig();

private slots:
    void slotDoubleSpinbox_Slider();
    void slotSlider_DoubleSpinbox();
    void slotDoubleSpinbox_Slider2();
    void slotSlider_DoubleSpinbox2();
    void slotDoubleSpinbox_Slider3();
    void slotSlider_DoubleSpinbox3();
    void on_delete_obj();
    void on_del_window_or_wall_clicked();
    void on_rotate_btn_valueChanged(int value);
    void on_delete_btn_clicked();
    void on_rot_angle_editingFinished();
    void on_cpos_x_editingFinished();
    void on_cpos_y_editingFinished();
    void on_trans_x_editingFinished();
    void on_trans_y_editingFinished();
    void on_test_window_or_door_clicked();
    void on_rm_area_clicked();

private:
    void initBasicInfo();
    void initBasicSchemeInfo();
    void initNormalLayout();

    QWidget* baseWidget;
    QSlider *slider;
    QSlider *slider2;
    QSlider *slider3;
    QDoubleSpinBox *spinBox;
    QDoubleSpinBox *spinBox2;
    QDoubleSpinBox *spinBox3;

    int pickUpObjID;
    gl3d::object *pickUpObj;
    gl3d::scene *main_scene;
    gl3d::gl3d_wall *wall;
    klm::design::scheme * sketch;
    QComboBox *cbo;
    glm::vec2 coord_on_screen;
};

#endif // PICKUPDIG_H
