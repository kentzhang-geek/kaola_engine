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
    unsigned int framebuffer;

private:
    gl3d_global_param();
};
}

#endif // GL3D_GLOBAL_PARAM_H
