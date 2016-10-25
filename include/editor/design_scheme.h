#ifndef DESIGN_SCHEME_H
#define DESIGN_SCHEME_H

#include <iostream>
#include <QImage>
#include <QPainter>
#include <QtCore>
#include <QtGui>
#include "kaola_engine/gl3d_scene.h"
#include "kaola_engine/gl3d_abstract_object.h"
#include "editor/gl3d_wall.h"
#include "editor/gl3d_door.h"
#include "resource_and_network/global_material.h"
#include "utils/gl3d_utils.h"
#include "kaola_engine/gl3d_general_light_source.h"
#include "editor/gl3d_window.h"
#include "pugiconfig.hpp"
#include "pugixml.hpp"

#define KLM_SCHEME_ID 65535

namespace klm {
    namespace design {
        class scheme : public gl3d::abstract_object , public utils::noncopyable {
        public:
            scheme(gl3d::scene * sc);
            ~scheme();
            // get mesh for scheme
            bool is_data_changed() ;
            bool is_visible() ;
            glm::mat4 get_translation_mat() ;
            glm::mat4 get_rotation_mat() ;
            glm::mat4 get_scale_mat() ;
            void get_abstract_meshes(QVector<gl3d::mesh *> & ms) ;
            void clear_abstract_meshes(QVector<gl3d::mesh *> & ms) ;
            void get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) ;
            void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) ;
            void set_translation_mat(const glm::mat4 & trans) ;

            GL3D_UTILS_PROPERTY(attached_scene, gl3d::scene *);

            GL3D_UTILS_PROPERTY_GET_POINTER(rooms, QSet<room *> );
            GL3D_UTILS_PROPERTY_GET_POINTER(walls, QSet<gl3d::gl3d_wall *> );
            GL3D_UTILS_PROPERTY_GET_POINTER(doors, QSet<gl3d::gl3d_door *> );
            GL3D_UTILS_PROPERTY_GET_POINTER(windows, QSet<gl3d::gl3d_window *> );
            GL3D_UTILS_PROPERTY_GET_POINTER(drawing_walls, QSet<gl3d::gl3d_wall *> );
            GL3D_UTILS_PROPERTY(wall_id_start, int);

            int find_available_id();

            bool add_wall(gl3d::gl3d_wall * w, gl3d::gl3d_wall * & wall_to_end);
            void del_wal(gl3d::gl3d_wall * w);

            bool add_door(gl3d_wall *w, glm::vec2 center_pt, float width, float height,
                          std::string resource_id = gl3d_door::default_model_res_id);
            void del_door(gl3d_door *w);

            bool add_window(gl3d_wall *w, glm::vec2 center_pt, float width, float height_min, float height_max,
                            std::string resource_id = gl3d_window::default_model_res_id);
            void del_window(gl3d_window *wdw);

            void release_rooms();
            void recalculate_rooms();

            // add or del furniture
            bool add_furniture(std::string fur_res, glm::vec3 pos = glm::vec3(0.0f));
            void del_furniture(gl3d::abstract_object * obj);

            void delete_room(gl3d::room * r);
            gl3d::room * get_room(glm::vec2 coord_on_screen);

            // objects in sketch
            GL3D_UTILS_PROPERTY_GET_POINTER(objects, QMap<int, gl3d::abstract_object *> );
            // light source in sketch
            GL3D_UTILS_PROPERTY_GET_POINTER(light_srcs, QMap<int, general_light_source *>);
            bool add_obj(int id, abstract_object * obj);
            bool del_obj(int id);
            gl3d::abstract_object * get_obj(int id);

            // draw assistant image
            bool draw_assistant_image(QImage * img);

            // save and load
            virtual bool save_to_xml(pugi::xml_node & node);
            virtual bool load_from_xml(pugi::xml_node node);

            // get wall's related room
            gl3d::room * get_related_room_of_wall(gl3d::gl3d_wall * wall, glm::vec2 coord_scr);
            klm::Surface * get_related_surface_of_wall(gl3d::gl3d_wall * wall, glm::vec2 coord_scr);

            void add_area_on_ground_of_room(QVector<glm::vec3> apts);

            // clear furnitures
            void clear_sketch();

        private:
            void init();
        };
    }
}

#endif // DESIGN_SCHEME_H
