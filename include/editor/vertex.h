#ifndef VERTEX_H
#define VERTEX_H

#include "kaola_engine/glheaders.h"
#include "kaola_engine/gl3d_out_headers.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>

using namespace glm;

namespace klm{

    class Vertex final{
    public:
        Vertex(const GLdouble &setX, const GLdouble &getY, const GLdouble &z,
               const GLdouble w =0, const GLdouble h = 0) noexcept;
        Vertex(const vec3 &position, const vec2 = vec2(0.0f, 0.0f)) noexcept;
        Vertex(const Vertex &vertex) noexcept;
        ~Vertex() noexcept;

        bool equals(const Vertex& another) const noexcept;
        GLdouble* getData() const noexcept;

        void getPosition(vec3 &position) const noexcept;
        void setPosition(const vec3 &position) noexcept;
        void getTexture(vec2 &texture) const noexcept;
        void setTexture(const vec2 &texture) noexcept;


        void setX(GLdouble x);
        void setY(GLdouble y);
        void setZ(GLdouble z);
        void setW(GLdouble w);
        void setH(GLdouble h);

        GLdouble getX() const;
        GLdouble getY() const;
        GLdouble getZ() const;
        GLdouble getW() const;
        GLdouble getH() const;

        void print() const;

    private:
        GLdouble *data;

    public:
        static const int X;
        static const int Y;
        static const int Z;
        static const int W;
        static const int H;
        static const int VERTEX_SIZE;
    };

}

#endif // VERTEX_H
