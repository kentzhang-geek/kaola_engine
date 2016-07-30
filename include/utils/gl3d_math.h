#ifndef GL3D_MATH_H
#define GL3D_MATH_H

#include "kaola_engine/gl3d_out_headers.h"
#include "utils/gl3d_utils.h"

namespace gl3d {
    namespace math {
    template <class T>
    class line {
    public:
        T a;
        T b;
        line(T tag_a, T tag_b) : a(tag_a), b(tag_b) {}
    };
    typedef line<glm::vec2> line_2d;
    typedef line<glm::vec3> line_3d;

    class triangle_facet {
    public:

        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;

        bool is_valid_facet() const;
        bool is_point_in_facet(glm::vec3 pt) const;
        triangle_facet(glm::vec3 ta, glm::vec3 tb, glm::vec3 tc);
        glm::vec3 get_normal() const;

    };

    bool get_cross(const line_2d &l1, const line_2d &l2, glm::vec2 &cross_point);

    float point_distance_to_line(const glm::vec3 pt, const line_3d l);
    float point_distance_to_facet(const triangle_facet & fc, const glm::vec3 & pt);

    void line_cross_facet(const triangle_facet &f, const line_3d &ray, glm::vec3 & pt);

    }
}

#endif // GL3D_MATH_H
