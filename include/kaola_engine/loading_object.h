#ifndef LOADING_OBJECT_H
#define LOADING_OBJECT_H

#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

// kaolaengine components
#include "utils/gl3d_utils.h"
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_image.hpp"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_abstract_object.h"
#include "editor/gl3d_wall.h"

namespace gl3d {
    class loading_object : public abstract_object, public utils::noncopyable {
    public:
        // constructor and destructor
        loading_object(glm::vec3 xyz);
        ~loading_object();

        // properties
    GL3D_UTILS_PROPERTY_GET_POINTER(sfcs, QVector<klm::Surface *>);
    GL3D_UTILS_PROPERTY(position, glm::vec3);
    GL3D_UTILS_PROPERTY(bounds, glm::vec3);
    GL3D_UTILS_PROPERTY_FIX_BUG(roll_mat, glm::mat4);
    GL3D_UTILS_PROPERTY(real_res_id, std::string);
    GL3D_UTILS_PROPERTY(loading_mtl, gl3d_material *);
    GL3D_UTILS_PROPERTY(is_picked, bool);

        // 继承抽象模型类方法并实现
        virtual bool is_data_changed();

        virtual bool is_visible();

        virtual glm::mat4 get_translation_mat();

        virtual glm::mat4 get_rotation_mat();

        virtual glm::mat4 get_scale_mat();

        virtual void get_abstract_meshes(QVector<gl3d::mesh *> &ms);

        virtual void get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt);

        virtual void set_translation_mat(const glm::mat4 &trans);

        virtual void clear_abstract_meshes(QVector<gl3d::mesh *> &ms);

        virtual void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt);

    private:
        void init();
    };
}


#endif // LOADING_OBJECT_H
