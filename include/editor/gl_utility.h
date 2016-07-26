#ifndef GLUTILITY_H
#define GLUTILITY_H

#include "editor/vertex.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QVector>

using namespace std;

namespace klm{

    class GLUtility final{
    public:
        //only coordinate of the vertex will be normalized
        static void normalize(Vertex& vertex) noexcept;

        static glm::vec3 getNormal(const Vertex& p1,
                              const Vertex &p2,
                              const Vertex &p3) noexcept;

        static void getRotation(const glm::vec3 &source,
                                const glm::vec3 &dest,
                                glm::mat4 &matrix) noexcept;
        static glm::vec3 getPlaneNormal(const QVector<glm::vec3> &points) noexcept;

        static void positionTransform(Vertex &vertex,
                                      const glm::mat4 &matrix) noexcept;
        static void textureTransform(Vertex &vertex,
                                     const glm::mat4 &matrix) noexcept;
    public:
        static const glm::vec3 NON_NORMAL;
        static const glm::vec3 Z_AXIS;
    private:
        GLUtility() = default;
        ~GLUtility() = default;
    };

}
#endif // GLUTILITY_H
