#ifndef GL3D_SCALE_H
#define GL3D_SCALE_H

#include <QVector>
#include <QMap>

namespace gl3d {
class scale {
public:
    enum length_unit {
        mm = 0,
        cm,
        m,
        skybox,
        wall
    };
    enum platform {
        windows = 0
    };

    static scale * shared_instance();
    float get_scale_factor(length_unit s, float widget_width);

private:
    void init();
    scale();
    platform current_platform;
    QMap<length_unit, float> length_unit_to_scale_factor;
    QMap<platform, float> platform_to_standard_widget_width;
};
}

#endif // GL3D_SCALE_H
