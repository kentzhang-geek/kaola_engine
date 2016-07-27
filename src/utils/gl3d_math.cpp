#include "utils/gl3d_math.h"

using namespace gl3d;
using namespace gl3d::math;

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

#if 0
#include <QtTest/QtTest>
void main() {
    glm::vec2 p1(0.0, 1.0);
    glm::vec2 p2(1.0, 1.0);
    glm::vec2 p3(0.0, 0.0);
    glm::vec2 p4(0.5, 0.5);
    glm::vec2 p5(0.0, 2.0);

    line l1(p5, p2);
    line l2(p3, p4);
    glm::vec2 res;
    bool ib = gl3d::math::get_cross(l1, l2, res);
}
#endif
