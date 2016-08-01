#include "kaola_engine/gl3d_abstract_object.h"

using namespace gl3d;

abstract_object::abstract_object() {
    // gen vao
    GL3D_GL()->glGenVertexArrays(1, &this->vao);
}

abstract_object::~abstract_object() {
    // delete vao
    GL3D_GL()->glDeleteVertexArrays(1, &this->vao);
}
