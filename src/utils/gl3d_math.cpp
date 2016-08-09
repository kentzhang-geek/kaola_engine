#include "utils/gl3d_math.h"

using namespace gl3d;
using namespace gl3d::math;

triangle_facet::triangle_facet(glm::vec3 ta, glm::vec3 tb, glm::vec3 tc) :
    a(ta), b(tb), c(tc)
{}

triangle_facet::triangle_facet() :
    a(glm::vec3(0.0f)), b(glm::vec3(0.0f)), c(glm::vec3(0.0f))
{}

triangle_facet::triangle_facet(const triangle_facet &f) : a(f.a), b(f.b), c(f.c)
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

bool triangle_facet::is_point_in_facet(glm::vec3 pt) const {
    if (0.00001 < gl3d::math::point_distance_to_facet(*this, pt)) {
        // 与平面距离不为0,则认为不在三角形内
        return false;
    }
    glm::vec3 ap = pt - this->a;
    glm::vec3 ab = this->b - this->a;
    glm::vec3 ac = this->c - this->a;
    float alpha = glm::acos(glm::dot(glm::normalize(ab), glm::normalize(ac)));

    glm::vec3 tab = gl3d::math::point_distance_to_line(pt, line_3d(this->a, this->c))
            / glm::sin(alpha) * glm::normalize(ab);
    glm::vec3 tac = gl3d::math::point_distance_to_line(pt, line_3d(this->a, this->b))
            / glm::sin(alpha) * glm::normalize(ac);
    if (
            (glm::normalize(tab) == glm::normalize(ab)) &&
            (glm::normalize(tac) == glm::normalize(ac)) &&
            (glm::length(tab) <= glm::length(ab)) &&
            (glm::length(tac) <= glm::length(ac))
            ) {
        return true;
    }
    else {
        return false;
    }
}

bool gl3d::math::get_cross(const line_2d &l1,
                           const line_2d &l2,
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
    if ((p1.x != p2.x) &&
            (p3.x != p4.x)) {
        float d1 = (p2.y - p1.y) / (p2.x - p1.x);
        float d2 = (p4.y - p3.y) / (p4.x - p3.x);
        x = p1.y - d1 * p1.x - p3.y + d2 * p3.x;
        x = x / (d2 - d1);
        y = d1 * x + p1.y - d1 * p1.x;
    }

    cross_point.x = x;
    cross_point.y = y;
    if (!(cross_point == cross_point)) {
        throw std::runtime_error("fuck cross point");
    }

    return true;
}

float gl3d::math::point_distance_to_facet(const triangle_facet &fc, const glm::vec3 &pt) {
    glm::vec3 nor = fc.get_normal();

    return glm::abs(glm::dot(nor, fc.c - pt));
}

bool gl3d::math::line_cross_facet(const triangle_facet &f, const line_3d &ray, glm::vec3 &pt) {
    if (0.000001 > glm::dot(
                f.get_normal(),
                glm::normalize(ray.b - ray.a)
                )) {
        return false;
    }
    float dis = gl3d::math::point_distance_to_facet(f, ray.a);
    float cosx = glm::dot(glm::normalize(ray.b - ray.a), f.get_normal());
    cosx = glm::abs(cosx);
    dis = dis / cosx;
    pt = ray.a + (ray.b - ray.a) * dis;

    return true;
}

float gl3d::math::point_distance_to_line(const glm::vec3 pt, const line_3d l) {
    if (((glm::length(l.a - pt) + glm::length(l.b - pt)) - glm::length(l.b - l.a))
            < 0.0001) {
        return 0.0f;
    }
    float alpha = glm::acos(glm::dot(glm::normalize(pt - l.a), glm::normalize(l.b - l.a)));
    return glm::length(pt - l.a) * glm::sin(alpha);
}

float gl3d::math::point_distance_to_line(const glm::vec2 pt, const line_2d l) {
    if (((glm::length(l.a - pt) + glm::length(l.b - pt)) - glm::length(l.b - l.a))
            < 0.0001) {
        return 0.0f;
    }
    float alpha = glm::acos(glm::dot(glm::normalize(pt - l.a), glm::normalize(l.b - l.a)));
    return glm::length(pt - l.a) * glm::sin(alpha);
}

bool gl3d::math::point_project_to_line(const line_2d & l, const glm::vec2 & pt, glm::vec2 out_pt) {
    glm::vec3 ptt(pt, 0.0f);
    line_3d tmpl(glm::vec3(l.a, 0.0f),
                 glm::vec3(l.b, 0.0f));
    if (gl3d::math::point_distance_to_line(ptt, tmpl) <= 0.0001) {
        return false;
    }

    glm::vec2 dir = l.b - l.a;
    float tmp = dir.y;
    dir.y = dir.x;
    dir.x = -tmp;

    line_2d l2(pt, pt + dir);
    gl3d::math::get_cross(l, l2, out_pt);

    return true;
}

static line_2d math_local_get_line(QVector<line_2d> & lines, glm::vec2 pt) {
    for (QVector<line_2d>::iterator it = lines.begin();
         it != lines.end();
         it++) {
        if ((it->a == pt) || (it->b == pt)) {\
            line_2d ret(it->a, it->b);
            lines.erase(it);
            return ret;
        }
    }

    return line_2d(glm::vec2(0.0f), glm::vec2(0.0f));
}

void gl3d::math::math_local_sort_line(QVector<line_2d> &lines) {
    glm::vec2 pt;
    pt = lines[0].b;
    QVector<line_2d> target;
    target.push_back(math_local_get_line(lines, pt));
    while(lines.size() > 0) {
        line_2d t = math_local_get_line(lines, pt);
        if (t.a == t.b) {
            // this is not a line!
            GL3D_UTILS_THROW("We have bad line in area edge loop");
        }
        if (t.b == pt) {
            t.swap_ab();
        }
        target.push_back(t);
        pt = t.b;
    }

    for (auto it = target.begin();
         it != target.end();
         it++) {
        lines.push_back(*it);
    }
}

glm::vec3 math_local_cal_area(QVector<line_2d> &poly) {
    glm::vec3 ret = glm::vec3(0.0f);
    for (auto it = poly.begin();
         it != poly.end();
         it++) {
        ret += glm::cross(glm::vec3(it->a.x, it->a.y, 0.0f),
                          glm::vec3(it->b.x, it->b.y, 0.0f));
    }

    return ret;
}

static glm::vec3 math_local_cvt_glm2_to_glm3(glm::vec2 a) {
    return glm::vec3(a.x, 0.0f, a.y);
}

QVector<glm::vec3> gl3d::math::generate_area(QVector<line_2d> & lines) {
    // 先做排序得到所有顶点组成的环，然后对环求向量面积，为正则不处理，为负则反向环
    QVector<glm::vec3> ret;
    QVector<line_2d> tmp;
    for (auto it = lines.begin();
         it != lines.end();
         it++) {
        tmp.push_back(*it);
    }
    math_local_sort_line(tmp);
    glm::vec3 tt = math_local_cal_area(tmp);

    if (tt.z > 0) {
        // 正向逆时针
        for (auto it = tmp.begin();
             it != tmp.end();
             it++) {
            ret.push_back(math_local_cvt_glm2_to_glm3(it->a));
        }
    }
    else if (tt.z < 0) {
        // 正向顺时针需反向
        for (int i = tmp.size() - 1;
             i >= 0;
             i--) {
            ret.push_back(math_local_cvt_glm2_to_glm3(tmp[i].b));
        }
    }
    else
        GL3D_UTILS_THROW("get area of polygon failed");
    return ret;
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
    QVector<line_2d> lss;
    lss.push_back(line_2d(glm::vec2(1.0f, 2.0f),glm::vec2(2.0f, 1.0f)));
    lss.push_back(line_2d(glm::vec2(-1.0f, 1.0f),glm::vec2(0.0f, 2.0f)));
    lss.push_back(line_2d(glm::vec2(1.0f, 0.0f),glm::vec2(2.0f, 1.0f)));
    lss.push_back(line_2d(glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f)));
    lss.push_back(line_2d(glm::vec2(0.0f, 2.0f),glm::vec2(1.0f, 2.0f)));
    lss.push_back(line_2d(glm::vec2(0.0f, 0.0f),glm::vec2(-1.0f, 1.0f)));
    gl3d::math::generate_area(lss);

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

    line_2d l1(p5, p2);
    line_2d l2(p3, p4);
    line_2d l3(p1, p2);
    line_2d l4(p6, p7);
    line_2d l5(p8, p9);

    line_2d l6(p1, p3);
    line_2d l7(p6, p7);

    line_2d l8(p1, p2);
    line_2d l9(p8, p9);

    line_2d l10(p2, p3);
    line_2d l11(p1, p10);
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

    line_3d lll(glm::vec3(0.0), glm::vec3(1.0));
    glm::vec3 pt(0.1, 0.5, 0.25);
    float xxqqq = gl3d::math::point_distance_to_line(pt, lll);
}
#endif
