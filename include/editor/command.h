#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoCommand>

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_utils.h"
#include "editor/design_scheme.h"
#include "resource_and_network/klm_resource_manager.h"

#include <QVector>

#define KLM_CMD_ID_SET_WALL 1
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
        class add_wall : public QUndoCommand, public utils::noncopyable {
        public:
            add_wall(gl3d_wall *w);

            void undo();

            void redo();

            bool mergeWith(const QUndoCommand *other);

            int wall_id;
            glm::vec2 start_pos;
            glm::vec2 end_pos;
            float height;
            float thickness;
            gl3d_wall_attach start_attach;
            gl3d_wall_attach end_attach;
        };

        class del_wall : public QUndoCommand, public utils::noncopyable {
        public:
            del_wall(gl3d_wall *w);

            void undo();

            void redo();

            int wall_id;
            glm::vec2 start_pos;
            glm::vec2 end_pos;
            float height;
            float thickness;
            gl3d_wall_attach start_attach;
            gl3d_wall_attach end_attach;
        };

        class set_wall_property : public QUndoCommand, public utils::noncopyable {
        public:
            set_wall_property(gl3d_wall *w);

            void undo();

            void redo();

            bool mergeWith(const QUndoCommand *other);

            virtual int id() const;

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

    }
}

#endif // COMMAND_H
