#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "kaola_engine/glheaders.h"
#include "kaola_engine/gl3d_out_headers.h"
#include <QVector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "editor/vertex.h"

using namespace std;

namespace klm{
    class BoundingBox{
    public:
        BoundingBox(const QVector<glm::vec3> &points);
        BoundingBox(const QVector<Vertex*> &vertecies);
        ~BoundingBox() = default;

        GLfloat getLength() const noexcept;
        GLfloat getHeight() const noexcept;
        GLfloat getWidth() const noexcept;

        GLfloat getXMin() const noexcept;
        GLfloat getXMax() const noexcept;
        GLfloat getYMin() const noexcept;
        GLfloat getYMax() const noexcept;
        GLfloat getZMin() const noexcept;
        GLfloat getZMax() const noexcept;

        void genTexture(Vertex &vertex) const noexcept;
        void genTexture(QVector<Vertex*> &verteces) const noexcept;

        glm::vec3 getCenter() const noexcept ;
    private:
        void initValues();
        void swapIfLess(const glm::vec3 &vector,
                        GLfloat &x, GLfloat &y, GLfloat &z) const noexcept;
        void swapIfGreater(const glm::vec3 &vector,
                           GLfloat &x, GLfloat &y, GLfloat &z) const noexcept;
    private:
        GLfloat xMin;
        GLfloat xMax;
        GLfloat yMin;
        GLfloat yMax;
        GLfloat zMin;
        GLfloat zMax;
    };
}

#endif // BOUNDINGBOX_H
