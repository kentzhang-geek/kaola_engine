#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoCommand>
#include <QDateTime>
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "editor/gl3d_window.h"
#include "editor/gl3d_door.h"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_utils.h"
#include "editor/design_scheme.h"
#include "resource_and_network/klm_resource_manager.h"

#include <QVector>

#define KLM_CMD_ID_SET_WALL 1
#define KLM_CMD_ID_SET_FURNITURE 2
#define KLM_CMD_ID_NULL -1


namespace klm {
    namespace command {
        class command_stack : public QUndoStack {
        public:
            static command_stack *shared_instance();

            static void init(gl3d::scene *sc, klm::design::scheme *sch);

        GL3D_UTILS_PROPERTY(main_scene, gl3d::scene *);
        GL3D_UTILS_PROPERTY(sketch, klm::design::scheme *);

        private:
            command_stack(gl3d::scene *sc, klm::design::scheme *sch);
        };

        // should reg command before call sketch add wall
        class add_or_del_wall : public utils::noncopyable{
        public:
            add_or_del_wall(gl3d_wall *w);
            ~add_or_del_wall();
            void combine_wall();
            bool add_wall();
            bool del_wall();

            gl3d_wall *wall;
            int wall_id;
            glm::vec2 start_pos;
            glm::vec2 end_pos;
            float height;
            float thickness;
            bool start_fixed;
            bool end_fixed;
            gl3d_wall_attach start_attach;
            gl3d_wall_attach end_attach;

            bool wall_exist;
        };

        // TODO : it is bug in add wall and del wall
        class add_wall : public QUndoCommand, public add_or_del_wall, public utils::noncopyable {
        public:
            add_wall(gl3d_wall *w);
            ~add_wall() {};

            void undo();

            void redo();
        };

        class del_wall : public QUndoCommand, public add_or_del_wall, public utils::noncopyable {
        public:
            del_wall(gl3d_wall *w);
            ~del_wall() {};

            void undo();

            void redo();
        };

        class set_wall_property : public QUndoCommand, public utils::noncopyable {
        public:
            set_wall_property(gl3d_wall *w);

            void undo();

            void redo();

            bool mergeWith(const QUndoCommand *other);

            virtual int id() const;

            bool auto_call;
            qint64 init_time;
            static const qint64 ONE_SECOND = 1000;

            glm::vec2 ori_start_pos;
            glm::vec2 ori_end_pos;
            float ori_height;
            float ori_thickness;
            gl3d_wall_attach ori_start_attach;
            gl3d_wall_attach ori_end_attach;

            glm::vec2 tar_start_pos;
            glm::vec2 tar_end_pos;
            float tar_height;
            float tar_thickness;
            gl3d_wall_attach tar_start_attach;
            gl3d_wall_attach tar_end_attach;
            int wall_id;
        };

        class add_hole : public QUndoCommand, public utils::noncopyable {
        public:
            add_hole(hole *h, gl3d_wall *w,
                     glm::vec3 center_point, float _width, float _min_height, float _max_height);

            void undo();

            void redo();

            int hole_id;
            int wall_id;
            glm::vec3 center;
            float width;
            float min_height;
            float max_height;
        };

        class del_hole : public QUndoCommand, public utils::noncopyable {
        public:
            del_hole(hole *h, gl3d_wall *w);

            del_hole(int hole_id, gl3d_wall *w);

            void undo();

            void redo();

            int wall_id;
            int hole_id;
            glm::vec3 pa;
            glm::vec3 pb;
        };

        class obj_add_or_del : public utils::noncopyable, public klm::resource::res_loader {
        public:
            obj_add_or_del(gl3d::object * o);
            void add_obj();

            void del_obj();

            void do_work(void *object);

            int obj_id;

            std::string res_id;
            QString obj_file_name;
            gl3d::scale::length_unit s_unit;
            glm::vec3 position;
            glm::mat4 rotate_mat;
            GLuint64 render_code;
            GLuint64 control_code;
        };

        class add_obj : public QUndoCommand, public obj_add_or_del {
        public:
            bool on_create;
            add_obj(gl3d::object * o);
            void undo();

            void redo();
        };

        class del_obj : public QUndoCommand, public obj_add_or_del {
        public:
            bool on_create;
            del_obj(gl3d::object * o);
            void undo();

            void redo();
        };

        // TODO : not process furniture's mtl changes now?
        class set_obj_property : public QUndoCommand, public obj_add_or_del {
        public:
            set_obj_property(gl3d::object * o);
            ~set_obj_property();
            void undo();

            void redo();

            bool mergeWith(const QUndoCommand *other);

            virtual int id() const;

            set_obj_property * target;
            gl3d::object * obj;
        };

        // TODO add or del door command
        class add_or_del_door : public utils::noncopyable {
        public:
            add_or_del_door(gl3d_door * d);
            virtual ~add_or_del_door();
            bool on_sketch;

            int obj_id;
            std::string res_id;
            glm::vec2 start_pt;
            glm::vec2 end_pt;
            glm::vec2 center_pt;
            float height;
            float width;
            float thickness;
            int attached_wall_id;
            int attached_hole_id;
            glm::mat4 pre_translate_mat;
            glm::mat4 pre_scale_mat;
            glm::mat4 trans_mat;
            glm::mat4 rotate_mat;

            void add_door();
            void del_door();
        };

        class add_door : public QUndoCommand, public add_or_del_door {
        public:
            bool on_create;
            add_door(gl3d_door * d);
            void undo();
            void redo();
        };

        class del_door : public QUndoCommand, public add_or_del_door {
        public:
            bool on_create;
            del_door(gl3d_door * d);
            void undo();
            void redo();
        };

        class add_or_del_window : public utils::noncopyable {
        public:
            add_or_del_window(gl3d_window * w);
            virtual ~add_or_del_window();
            bool on_sketch;

            std::string res_id;
            glm::vec2 start_pt;
            glm::vec2 end_pt;
            glm::vec2 center_pt;
            float height_max;
            float height_min;
            float width;
            float thickness;
            int attached_wall_id;
            int attached_hole_id;
            glm::mat4 pre_translate_mat;
            glm::mat4 pre_scale_mat;
            glm::mat4 trans_mat;
            glm::mat4 rotate_mat;
            int obj_id;

            void add_window();
            void del_window();
        };

        class add_window : public QUndoCommand, public add_or_del_window {
        public:
            bool on_create;
            add_window(gl3d_window * win);
            void undo();
            void redo();
        };

        class del_window : public QUndoCommand, public add_or_del_window {
        public:
            bool on_create;
            del_window(gl3d_window * win);
            void undo();
            void redo();
        };
    }
}

#endif // COMMAND_H
