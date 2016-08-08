#ifndef GL3D_MATH_H
#define GL3D_MATH_H

#include "kaola_engine/gl3d_out_headers.h"
#include "utils/gl3d_utils.h"
#include <QVector>

namespace gl3d {
    namespace math {
    template <class T>
    class line {
    public:
        T a;
        T b;
        line() : a(glm::vec2(0.0f)), b(glm::vec2(0.0f)) {}
        line(T tag_a, T tag_b) : a(tag_a), b(tag_b) {}
        line(const line<T> &l) : a(l.a), b(l.b) {}
        line(const line<T> *l) : a(l->a), b(l->b) {}
        void swap_ab() {T tmp = this->b;
                       this->b = this->a;
                       this->a = tmp;}
        float length() {return glm::length(b - a);}
    };
    typedef line<glm::vec2> line_2d;
    typedef line<glm::vec3> line_3d;

    template <typename T>
    bool equal(T v1, T v2) {
        return (glm::length(v1 - v2) < 0.0001);
    }

    class triangle_facet {
    public:

        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;

        bool is_valid_facet() const;
        bool is_point_in_facet(glm::vec3 pt) const;
        triangle_facet();
        triangle_facet(const triangle_facet &f);
        triangle_facet(glm::vec3 ta, glm::vec3 tb, glm::vec3 tc);
        glm::vec3 get_normal() const;

    };

    bool get_cross(const line_2d &l1, const line_2d &l2, glm::vec2 &cross_point);

    float point_distance_to_line(const glm::vec3 pt, const line_3d l);
    float point_distance_to_line(const glm::vec2 pt, const line_2d l);
    float point_distance_to_facet(const triangle_facet & fc, const glm::vec3 & pt);

    bool point_project_to_line(const line_2d & l, const glm::vec2 & pt, glm::vec2 &out_pt);

    bool line_cross_facet(const triangle_facet &f, const line_3d &ray, glm::vec3 & pt);

    void math_local_sort_line(QVector<line_2d> &lines);
    QVector<glm::vec3> generate_area(QVector<line_2d> &lines);

    }
}

#endif // GL3D_MATH_H
