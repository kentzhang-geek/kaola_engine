#include "utils/gl3d_math.h"

using namespace gl3d;
using namespace gl3d::math;

triangle_facet::triangle_facet(glm::vec3 ta, glm::vec3 tb, glm::vec3 tc) :
    a(ta), b(tb), c(tc)
{}

bool triangle_facet::is_valid_facet() const {
    glm::vec3 td1 = glm::normalize(a - b);
    glm::vec3 td2 = glm::normalize(a - c);
    if ((td1 == td2) || (td1 == (-td2))) {
        return false;
    }
    else {
        return true;
    }
}

glm::vec3 triangle_facet::get_normal() const {
    glm::vec3 nor = glm::cross((this->b - this->a), (this->c - this->a));
    nor = glm::normalize(nor);

    return nor;
}

bool gl3d::math::get_cross(const line &l1,
                           const line &l2,
                           glm::vec2 & cross_point) {
    glm::vec2 p1 = l1.a;
    glm::vec2 p2 = l1.b;
    glm::vec2 p3 = l2.a;
    glm::vec2 p4 = l2.b;

    float x = 0.0;
    float y = 0.0;
    if (glm::normalize(p2 - p1) == glm::normalize(p4 - p3)) {
        return false;
    }
    if (glm::normalize(p2 - p1) == glm::normalize(p3 - p4)) {
        return false;
    }

    if (p1.x == p2.x) { // special edition is x equals
        x = p1.x;
        y = x * (p4.y - p3.y) / (p4.x - p3.x)
                - (p4.y - p3.y) / (p4.x - p3.x) * p3.x + p3.y;
    }
    if (p3.x == p4.x) {
        x = p3.x;
        y = x * (p2.y - p1.y) / (p2.x - p1.x)
                - (p2.y - p1.y) / (p2.x - p1.x) * p1.x + p1.y;
    }
    else {
        float d1 = (p2.y - p1.y) / (p2.x - p1.x);
        float d2 = (p4.y - p3.y) / (p4.x - p3.x);
        x = p1.y - d1 * p1.x - p3.y + d2 * p3.x;
        x = x / (d2 - d1);
        y = d1 * x + p1.y - d1 * p1.x;
    }

    cross_point.x = x;
    cross_point.y = y;
    return true;
}

float gl3d::math::point_distance_to_facet(const triangle_facet &fc, const glm::vec3 &pt) {
    glm::vec3 nor = fc.get_normal();

    return glm::dot(nor, fc.c - pt);
}

void gl3d::math::line_cross_facet(const triangle_facet &f, const line &ray, glm::vec3 &pt) {
    float dis = gl3d::math::point_distance_to_facet(f, ray.a);
    float cosx = glm::dot(glm::normalize(ray.b - ray.a), f.get_normal());
    cosx = glm::abs(cosx);
    dis = dis / cosx;
    pt = ray.a + (ray.b - ray.a) * dis;

    return;
}

#if 0
#include <QString>
#define GL3D_TEST(...) if (!(__VA_ARGS__)) { \
    QString throw_out = QString::asprintf("test failed at %s : %d \n", __FILE__, __LINE__);\
    throw_out += QString::asprintf("expression is %s", ##__VA_ARGS__); \
    throw throw_out; \
    }

#include <QtTest/QtTest>
void main() {
    glm::vec2 p1(0.0, 1.0);
    glm::vec2 p2(1.0, 1.0);
    glm::vec2 p3(0.0, 0.0);
    glm::vec2 p4(0.5, 0.5);
    glm::vec2 p5(0.0, 2.0);
    glm::vec2 p6(3.0, 3.0);
    glm::vec2 p7(3.0, 0.0);

    glm::vec2 p8(4.0, 1.0);
    glm::vec2 p9(5.0, 1.0);

    glm::vec2 p10(1.0, 2.0);

    line l1(p5, p2);
    line l2(p3, p4);
    line l3(p1, p2);
    line l4(p6, p7);
    line l5(p8, p9);

    line l6(p1, p3);
    line l7(p6, p7);

    line l8(p1, p2);
    line l9(p8, p9);

    line l10(p2, p3);
    line l11(p1, p10);
    glm::vec2 res;
    bool ib = gl3d::math::get_cross(l2, l1, res);
    GL3D_TEST(ib == true);
    ib = gl3d::math::get_cross(l3, l4, res);
    GL3D_TEST(ib == true);
    ib = gl3d::math::get_cross(l5, l4, res);
    GL3D_TEST(ib == true);
    ib = gl3d::math::get_cross(l6, l7, res);
    GL3D_TEST(ib == false);
    ib = gl3d::math::get_cross(l8, l9, res);
    GL3D_TEST(ib == false);
    ib = gl3d::math::get_cross(l10, l11, res);
    GL3D_TEST(ib == false);
}
#endif
