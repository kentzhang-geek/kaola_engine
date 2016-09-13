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

        // save and load
        bool save_to_xml(pugi::xml_node &node);
        static gl3d_door * load_from_xml(pugi::xml_node node);

        virtual bool is_data_changed();
        virtual bool is_visible();
        virtual glm::mat4 get_translation_mat();
        virtual glm::mat4 get_rotation_mat();
        virtual glm::mat4 get_scale_mat();
        virtual void get_abstract_meshes(QVector<gl3d::mesh *> & ms);
        virtual void clear_abstract_meshes(QVector<gl3d::mesh *> & ms);
        virtual void get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt);
        virtual void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt);
        virtual void set_translation_mat(const glm::mat4 & trans);

    private:
        void init ();
    };
}

#endif // GL3D_DOOR_H
