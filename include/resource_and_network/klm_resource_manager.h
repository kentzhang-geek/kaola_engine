#ifndef KLM_RESOURCE_MANAGER_H
#define KLM_RESOURCE_MANAGER_H

#include "kaola_engine/gl3d.hpp"
#include "utils/gl3d_utils.h"
#include "utils/gl3d_path_config.h"
#include <string>
#include <QVector>
#include <QMap>
#include <QByteArray>
#include <QThread>
#include <QMutex>
#include "editor/merchandise.h"

namespace klm {
    namespace resource {
        class item;

        class res_loader : public QThread {
        public:
            virtual void do_work(void *object);

            void run() Q_DECL_OVERRIDE;

        GL3D_UTILS_PROPERTY(obj_res_id, std::string);
        };

        // default model loader for most furnitures
        class default_model_loader : public res_loader {
        public:
            gl3d::scene *main_scene;
            GLuint64 render_code;
            GLuint64 control_code;
            int obj_render_id;

            default_model_loader(gl3d::scene *sc,
                                 int _id = 1,
                                  GLuint64 _render_code = GL3D_SCENE_DRAW_NORMAL |
                                                          GL3D_SCENE_DRAW_IMAGE |
                                                          GL3D_SCENE_DRAW_SHADOW,
                                  GLuint64 _control_code = GL3D_OBJ_ENABLE_ALL &
                                                           (~GL3D_OBJ_ENABLE_CULLING)) :
                    main_scene(sc),
                    obj_render_id(_id),
                    render_code(_render_code),
                    control_code(_control_code) {}

            void do_work(void *object);
        };

        class manager {
        public:
            static manager *shared_instance();

            std::string get_res_item(string id);

            klm::Merchandise *get_merchandise_item(string id);

            // start a thread to load data from disk
            void perform_async_res_load(res_loader *ld, string id);

            // release all data in mem
            void release_all_resources();

            // preload resources like ground and some else
            void preload_resources(gl3d::scene * sc);

            QMap<std::string, std::string> local_resource_map;
            void load_local_databse();
            void save_local_databse();

            // test
            void test_data();

        private:
            manager();

            // resource id to
            QMap<string, item> id_to_item;
            QMap<string, std::string > id_to_resource;
            QMap<string, klm::Merchandise *> id_to_merchandise;
        };

        // internal resource item  class , use to acknowledgement resource property
        class item {
        public:
            enum resource_type {
                res_default_file = 0,
                res_model_3ds,
                res_texture_material,
                res_texture_picture
            };

        GL3D_UTILS_PROPERTY_VIRTUAL(res_id, string);
        GL3D_UTILS_PROPERTY_VIRTUAL(res_type, resource_type);
        GL3D_UTILS_PROPERTY(is_local, bool);
        GL3D_UTILS_PROPERTY(full_file_name, string);

            item() {}
            item(string id, string filename, resource_type tp, bool islocal) :
                    res_id(id), res_type(tp), is_local(islocal), full_file_name(filename) {}

            item(const item &cp) : res_id(cp.res_id), res_type(cp.res_type),
                                   is_local(cp.is_local), full_file_name(cp.full_file_name) {};
        };
    };
}

#endif // KLM_RESOURCE_MANAGER_H
