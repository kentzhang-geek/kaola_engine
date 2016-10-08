#ifndef GL3D_WALL_H
#define GL3D_WALL_H

#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "kaola_engine/Program.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_render_process.hpp"

// OpenGL in windows should use GLEW
#include "kaola_engine/glheaders.h"

// kaolaengine components
#include "utils/gl3d_utils.h"
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_general_light_source.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_object.h"
#include "kaola_engine/gl3d_scene.h"
#include "utils/gl3d_math.h"
#include "utils/gl3d_global_param.h"

using namespace std;
using namespace gl3d;

namespace klm {
    class Surface;
}

namespace gl3d {
    void surface_to_mesh(klm::Surface *sfc, QVector<gl3d::mesh *> &vct, bool picked = false);
    void get_faces_from_surface(klm::Surface *sfc, QVector<math::triangle_facet> &faces);
    klm::Surface * pick_up_surface_in_surface(klm::Surface * sfc, glm::vec3 pt);

    class gl3d_wall;
    class room;

    // 墙的附着状态，用于决定是否需要重算墙角，以及是否可以设置长度
    class gl3d_wall_attach {
    public:
        enum attachment_point {
            start_point = 0,
            end_point
        };
        gl3d_wall *attach;
        attachment_point attach_point;

        gl3d_wall_attach();

        // save and load
        virtual bool save_to_xml(pugi::xml_node &node);
        virtual bool load_from_xml(pugi::xml_node node);
    };

    class hole;

    class gl3d_wall : public object , public utils::noncopyable{
    public:
        gl3d_wall() : start_point(0.0), end_point(0.0) {}

        gl3d_wall(IN glm::vec2 s_pt, IN glm::vec2 e_pt, IN float t_thickness, IN float hight);

        ~gl3d_wall();

        // properties
    GL3D_UTILS_PROPERTY_DECLARE(start_point, glm::vec2);
    GL3D_UTILS_PROPERTY_DECLARE(end_point, glm::vec2);
    GL3D_UTILS_PROPERTY_DECLARE(thickness, float);
    GL3D_UTILS_PROPERTY(hight, float);
    GL3D_UTILS_PROPERTY(start_point_fixed, bool);
    GL3D_UTILS_PROPERTY(end_point_fixed, bool);
    GL3D_UTILS_PROPERTY_GET_POINTER(start_point_attach, gl3d_wall_attach);
    GL3D_UTILS_PROPERTY_GET_POINTER(end_point_attach, gl3d_wall_attach);
    GL3D_UTILS_PROPERTY_GET_POINTER(sfcs, QVector<klm::Surface *>);
    GL3D_UTILS_PROPERTY_GET_POINTER(relate_rooms, QSet<gl3d::room *>);
    GL3D_UTILS_PROPERTY(fixed, bool);

        // pick
        GL3D_UTILS_PROPERTY(is_picked, bool );

        QMap<int, hole *> holes_on_this_wall;

        void calculate_mesh();

        void get_coord_on_screen(IN scene *main_scene, OUT glm::vec2 &start_pos, OUT glm::vec2 &end_pos);

        enum tag_combine_traits {
            combine_wall1_start_to_wall2_start = 0,
            combine_wall1_start_to_wall2_end,
            combine_wall1_end_to_wall2_start,
            combine_wall1_end_to_wall2_end,
        };

        static bool combine(gl3d_wall *wall1, gl3d_wall *wall2, tag_combine_traits combine_traits);

        static bool combine(gl3d_wall *wall1, gl3d_wall *wall2, glm::vec2 combine_point);

        static bool wall_cross(gl3d_wall *wall_cutter, gl3d_wall *wall_target, QSet<gl3d_wall *> & output_walls);

        // remove target if target is covered , return true if removed , false if not removed
        static bool delete_wall_in_wall(gl3d_wall * & wall_target, gl3d_wall * & wall_rmver);

        void seperate(INOUT gl3d::gl3d_wall_attach &attachment);

        gl3d::obj_points bottom_pts[4];

        float get_length();

        bool set_length(IN float len);

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

        // 挖洞相关
        // 获取投射到墙面的点
        bool get_coord_on_wall(gl3d::scene *sce, glm::vec2 coord_on_screen, glm::vec3 &out_point_on_wall,
                               glm::vec3 &out_point_normal);

        // set material of this wall
        bool set_material(std::string res_id);

        // get availble hole id
        int get_availble_hole_id();

        // save and load
        virtual bool save_to_xml(pugi::xml_node &node);
        static gl3d_wall * load_from_xml(pugi::xml_node node);

    private:
        void release_last_data();

        void init();
    };

    class hole : public utils::noncopyable {
    public:
        // two point on world coordinate
    GL3D_UTILS_PROPERTY(pta, glm::vec3);
    GL3D_UTILS_PROPERTY(ptb, glm::vec3);
    GL3D_UTILS_PROPERTY(hole_id, int);
        // on witch wall
    GL3D_UTILS_PROPERTY(on_witch_wall, gl3d_wall *);
        // attach to a surface
    GL3D_UTILS_PROPERTY(sfc, klm::Surface *);

        // do nothing
        hole();

        // should not copy
        hole(const hole &cp);

        // create new hole on wall w
        hole(gl3d_wall *w, glm::vec3 point_a, glm::vec3 point_b, int hole_id);
        hole(gl3d_wall *w, glm::vec3 center_point, float width, float min_height, float max_height, int hole_id);

        // check is this hole valid to the wall
        bool is_valid();

        static int global_hole_id;

        // save and load
        virtual bool save_to_xml(pugi::xml_node &node);
        static hole * load_from_xml(pugi::xml_node node);
        static hole * load_from_xml(pugi::xml_node node, gl3d::gl3d_wall * wall);

    private:
        void init();
    };

    class room : public utils::noncopyable {
    public:
        room(QVector<glm::vec3> edge_points);
        ~room();
        klm::Surface * ground;
        QVector<glm::vec3> edge_points;
        QSet<gl3d::gl3d_wall *> relate_walls;
        std::string name;
        void set_picked(bool on_off);
        bool get_picked();

        void get_relate_surfaces(QSet<klm::Surface *> &surfaces);
        void clear_relate_surfaces(QSet<klm::Surface *> &surfaces);
        void set_material(std::string res_id);

        virtual bool save_to_xml(pugi::xml_node & node);
        static room * load_from_xml(pugi::xml_node & node);

    private:
        bool picked;
    };
}

#endif // GL3D_WALL_H
