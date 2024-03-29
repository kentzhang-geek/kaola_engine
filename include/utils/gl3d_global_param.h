#ifndef GL3D_GLOBAL_PARAM_H
#define GL3D_GLOBAL_PARAM_H

#include <QVector>
#include <QMap>
#include <QString>

namespace gl3d {
class gl3d_global_param {
public:
    enum e_sketch_mode {
        new_design = 0,
        change_plan,
        change_design,
        new_plan
    };
    e_sketch_mode sketch_mode;

    static gl3d_global_param * shared_instance();
    QString * username;
    QString * password;
    float canvas_width;
    float canvas_height;
    float wall_thick;
    float room_height;
    int maxCulledObjNum;
    unsigned int framebuffer;
    void *old_sketch_test;
    void * main_scene;

    enum work_state {
        normal = 0,
        drawwall = 1,
        drawwalling = 2,
        pickup = 3,
        drawhome = 4,
        drawhomeing = 5,
        opendoor = 6,
        movewall = 7,
        movewalling = 8,
        openwindow = 9,
        movefurniture = 10,
        ruling = 11,
        draw_area = 12,
        add_furniture = 13,
        add_texture = 14
    } current_work_state;
private:
    gl3d_global_param();
};
}

#endif // GL3D_GLOBAL_PARAM_H
