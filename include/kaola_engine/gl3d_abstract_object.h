#ifndef GL3D_ABSTRACT_OBJECT_H
#define GL3D_ABSTRACT_OBJECT_H

#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_material.hpp"

#include <QVector>
#include <QMap>

namespace gl3d {
class abstract_object {
public:
    // public interfaces
    virtual bool is_data_changed() = 0;
    virtual bool is_visible() = 0;
    virtual glm::mat4 get_translation_mat() = 0;
    virtual glm::mat4 get_rotation_mat() = 0;
    virtual glm::mat4 get_scale_mat() = 0;
    virtual QVector<gl3d::mesh *> * get_abstract_meshes() = 0;
    virtual QMap<unsigned int, gl3d_material *> * get_abstract_mtls() = 0;

    // constructors and destr...
    abstract_object();
    ~abstract_object();
    GL3D_UTILS_PROPERTY(vao, GLuint );
};
}

#endif // GL3D_ABSTRACT_OBJECT_H
