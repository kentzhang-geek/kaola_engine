#ifndef GL3D_DOOR_H
#define GL3D_DOOR_H

#include "utils/gl3d_math.h"
#include "editor/gl3d_wall.h"
#include "utils/gl3d_lock.h"
#include "resource_and_network/global_material.h"

namespace gl3d {
    class gl3d_door : public abstract_object, utils::noncopyable{
    public:
        glm::vec2 start_pt;
        glm::vec2 end_pt;
        glm::vec2 center_pt;
        float height;
        float width;
        int attached_wall_id;
        int attached_hole_id;
        gl3d::object * door_model;
        glm::mat4 pre_translate_mat;
        glm::mat4 trans_mat;
        glm::mat4 rotate_mat;

        static std::string default_model_res_id;

        gl3d_door(std::string model_res = gl3d_door::default_model_res_id);
        virtual ~gl3d_door();
        bool install_to_wall(gl3d_wall *wall, glm::vec2 center_point, float width, float height);
        bool is_valid();

        virtual bool is_data_changed() = 0;
        virtual bool is_visible() = 0;
        virtual glm::mat4 get_translation_mat() = 0;
        virtual glm::mat4 get_rotation_mat() = 0;
        virtual glm::mat4 get_scale_mat() = 0;
        virtual void get_abstract_meshes(QVector<gl3d::mesh *> & ms) = 0;
        virtual void clear_abstract_meshes(QVector<gl3d::mesh *> & ms) = 0;
        virtual void get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) = 0;
        virtual void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) = 0;
        virtual void set_translation_mat(const glm::mat4 & trans) = 0;

    private:
        void init ();
    };
}

#endif // GL3D_DOOR_H
