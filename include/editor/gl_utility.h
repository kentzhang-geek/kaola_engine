#ifndef GLUTILITY_H
#define GLUTILITY_H

#include "editor/vertex.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

using namespace std;

namespace klm{

    class GLUtility final{
    public:
        //only coordinate of the vertex will be normalized
        static void normalize(Vertex& vertex) noexcept;

        static vec3 getNormal(const Vertex& p1, const Vertex &p2,
                       const Vertex &p3) noexcept;

        static void getRotation(const vec3 &source, const vec3 &dest, mat4 &matrix) noexcept;
        static vec3 getPlaneNormal(const vector<vec3> &points) noexcept;

        static void positionTransform(Vertex &vertex, const mat4 &matrix) noexcept;
        static void textureTransform(Vertex &vertex, const mat4 &matrix) noexcept;        
    public:
        static const vec3 NON_NORMAL;
        static const vec3 Z_AXIS;
    private:
        GLUtility() = default;
        ~GLUtility() = default;
    };

}
#endif // GLUTILITY_H
