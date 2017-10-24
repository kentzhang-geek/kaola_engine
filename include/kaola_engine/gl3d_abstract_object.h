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
    // 描述Object的具体类型，墙，家居还是其他
    enum tag_obj_type {
        type_default = 0,
        type_wall,
        type_furniture,
        type_door,
        type_window,
        type_ground,
        type_scheme,
        type_special,
        type_loading_object
    };

    // public interfaces
    virtual bool is_data_changed() = 0;
    virtual bool is_visible() = 0;
    virtual glm::vec3 getMaxBoundry() = 0;
    virtual glm::vec3 getMinBoundry() = 0;
    virtual glm::mat4 get_translation_mat() = 0;
    virtual glm::mat4 get_rotation_mat() = 0;
    virtual glm::mat4 get_scale_mat() = 0;
    glm::mat4 getModelMat();
    glm::vec3 getCenterPointInWorldCoord();
    virtual void get_abstract_meshes(QVector<gl3d::mesh *> & ms) = 0;
    virtual void clear_abstract_meshes(QVector<gl3d::mesh *> & ms) = 0;
    virtual void get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) = 0;
    virtual void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) = 0;
    virtual void set_translation_mat(const glm::mat4 & trans) = 0;
    virtual glm::vec3 get_abs_position();
    virtual glm::mat4 get_abs_rotation();

    // constructor and destructor
    abstract_object();
    virtual ~abstract_object();
    // do not overwrite methods below
    GL3D_UTILS_PROPERTY(obj_type, tag_obj_type);
    GL3D_UTILS_PROPERTY(vao, GLuint );
    GL3D_UTILS_PROPERTY(id, GLuint );
    GL3D_UTILS_PROPERTY(data_buffered, bool );
    GL3D_UTILS_PROPERTY(pick_flag, bool );
    GL3D_UTILS_PROPERTY(control_authority, GLuint64 ); //64种控制权限，在gl3d_obj_authority.h中查看
    GL3D_UTILS_PROPERTY(render_authority, GLuint64 ); // 64种绘制权限，在gl3d_obj_authority.h中查看
    enum tag_coordinate_system {
        left_hand = 0,
        right_hand
    } coordinate_system;
    virtual void buffer_data();
    QMap<QString, void *> userData;
};
}

#endif // GL3D_ABSTRACT_OBJECT_H
