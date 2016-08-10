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
#include "editor/merchandise.h"

namespace klm {
    namespace resource {
        class item;

        class res_loader : public QThread {
        public:
            virtual void do_work(void *object);

            void run() Q_DECL_OVERRIDE;

        GL3D_UTILS_PROPERTY(obj_ptr, void *);
        };

        class manager {
        public:
            static manager *shared_instance();

            template<typename T>
            T *get_res_item(string id);

            klm::Merchandise *get_merchandise_item(string id);

            void perform_async_res_load(res_loader *ld, string id);

            void release_all_resources();

        private:
            manager();

            // resource id to
            QMap<string, item> id_to_item;
            QMap<string, void *> id_to_resource;
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
        };
    };
}

#endif // KLM_RESOURCE_MANAGER_H
