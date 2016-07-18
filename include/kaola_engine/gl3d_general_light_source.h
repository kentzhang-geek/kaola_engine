#ifndef GL3D_GENERAL_LIGHT_SOURCE_H
#define GL3D_GENERAL_LIGHT_SOURCE_H

#include "kaola_engine/gl3d_out_headers.h"
#include "utils/gl3d_utils.h"

// 光源抽象定义，暂时只支持白色光源（纯亮度）

namespace gl3d {
class general_light_source {
    enum gl3d_light_type {
        point_light = 0,
        directional_point_light,
        vector_light
    };

    GL3D_UTILS_PROPERTY(location, glm::vec3);
    GL3D_UTILS_PROPERTY(direction, glm::vec3);
    GL3D_UTILS_PROPERTY(light_type, gl3d_light_type);
    GL3D_UTILS_PROPERTY(is_attached_to_object, bool);
    GL3D_UTILS_PROPERTY(attached_object_id, int);

public:
    general_light_source();
    void attach_to_object(int id);
    void disattach_object();
};
}

#endif // GL3D_GENERAL_LIGHT_SOURCE_H
