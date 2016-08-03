#include "kaola_engine/gl3d_abstract_object.h"

using namespace gl3d;

abstract_object::abstract_object() {
    // gen vao
    GL3D_GL()->glGenVertexArrays(1, &this->vao);
    this->pick_flag = false;
}

abstract_object::~abstract_object() {
    // delete vao
    GL3D_GL()->glDeleteVertexArrays(1, &this->vao);
}

void abstract_object::buffer_data() {
    // KENT TODO : 已缓存过顶点就不再缓存顶点，目前暂不考虑改变顶点数据的情况
    if (data_buffered) {
        return;
    }

    // bind vao
    GL3D_GL()->glBindVertexArray(this->get_vao());

    gl3d::mesh * p_mesh;
    QVector<mesh *> mss;
    this->get_abstract_meshes(mss);
    auto iter = mss.begin();
    while (iter != mss.end()) {
        p_mesh = *iter;
        p_mesh->buffer_data();
        iter++;
    }

    GL3D_GL()->glBindVertexArray(0);

    data_buffered = true;
}