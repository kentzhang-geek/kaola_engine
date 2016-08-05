#ifndef GL3D_GLOBAL_PARAM_H
#define GL3D_GLOBAL_PARAM_H

#include <QVector>
#include <QMap>

namespace gl3d {
class gl3d_global_param {
public:
    static gl3d_global_param * shared_instance();
    float canvas_width;
    float canvas_height;
    float wall_thick;
    unsigned int framebuffer;

    enum work_state {
        normal = 0,
        drawwall = 1,
        drawwalling = 2,
        drawwallend = 3,
        pickup = 4,
        drawhome = 5,
        drawhomeing = 6
    } current_work_state ;
private:
    gl3d_global_param();
};
}

#endif // GL3D_GLOBAL_PARAM_H
