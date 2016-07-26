#include "utils/gl3d_math.h"

using namespace gl3d;
using namespace gl3d::math;

void gl3d::math::get_cross(const line &l1,
                           const line &l2,
                           glm::vec2 cross_point) {
    if (l1.a.x == l1.b.x) { // special edition is x equals

    }
    if (l2.a.x == l2.b.x) {

    }

    glm::vec2 p1 = l1.a;
    glm::vec2 p2 = l1.b;
    glm::vec2 p3 = l2.a;
    glm::vec2 p4 = l2.b;

    float x = (p2.y - p1.y) / (p2.x - p1.x) - p1.y +
            (p4.y - p3.y) / (p4.x - p3.x) * p3.x + p3.y;
    x = x / ((p2.y - p1.y)/(p2.x - p1.x) - (p4.y - p3.y) / (p4.x - p3.x));
    float y = x * (p2.y - p1.y) / (p2.x - p1.x)
            - (p2.y - p1.y) / (p2.x - p1.x) * p1.x + p1.y;

    cross_point.x = x;
    cross_point.y = y;
}

#if 1
void main() {

}
#endif
