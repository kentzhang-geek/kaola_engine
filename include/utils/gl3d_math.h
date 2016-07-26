#ifndef GL3D_MATH_H
#define GL3D_MATH_H

#include "kaola_engine/gl3d_out_headers.h"
#include "utils/gl3d_utils.h"

namespace gl3d {
    namespace math {
    class line {
    public:
        glm::vec2 a;
        glm::vec2 b;
        line(glm::vec2 tag_a, glm::vec2 tag_b) : a(tag_a), b(tag_b) {}
    };

    bool get_cross(const line &l1, const line &l2, glm::vec2 &cross_point);

    }
}

#endif // GL3D_MATH_H