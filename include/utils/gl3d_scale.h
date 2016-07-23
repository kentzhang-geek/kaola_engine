#ifndef GL3D_SCALE_H
#define GL3D_SCALE_H

#include <QVector>
#include <QMap>

#include "utils/gl3d_utils.h"

namespace gl3d {
class scale {
public:
    enum length_unit {
        mm = 0,
        cm,
        m,
        skybox,
        wall,
        special
    };
    enum platform {
        windows = 0
    };

    static scale * shared_instance();
    float get_scale_factor(float widget_width);

    template <typename T>
    T pre_scale_vertex(T &in_value, length_unit unit);

//    GL3D_UTILS_PROPERTY(global_scale, float);
    GL3D_UTILS_PROPERTY(current_platform, platform);
    GL3D_UTILS_PROPERTY_GET_POINTER(length_unit_to_scale_factor, QMap<length_unit, float> );
    GL3D_UTILS_PROPERTY_GET_POINTER(platform_to_standard_widget_width,
                                    QMap<platform, float> );

private:
    void init();
    scale();
};
}

#endif // GL3D_SCALE_H
