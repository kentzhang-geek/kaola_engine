#ifndef GLOBAL_MATERIAL_H
#define GLOBAL_MATERIAL_H

#include "kaola_engine/gl3d_material.hpp"
#include "resource_and_network/klm_resource_manager.h"
#include "utils/gl3d_math.h"
#include "utils/gl3d_lock.h"
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_utils.h"
#include <QVector>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QSet>
#include <QString>

namespace gl3d {
    class global_material_lib {
    public:
        static global_material_lib * shared_instance();

    GL3D_UTILS_PROPERTY_GET_POINTER(res_id_to_mtl_id, QMap<string, int> );
    GL3D_UTILS_PROPERTY_GET_POINTER(mtl_id_to_res_id, QMap<int, string> );
    GL3D_UTILS_PROPERTY_GET_POINTER(mtl_id_to_materials, QMap<int, gl3d::gl3d_material *> );

        typedef void load_mtl_callback(int id, gl3d_material * mtl, void * user_data);
        void request_new_mtl(string res_id, load_mtl_callback * cbk, void * udata);
        bool is_mtl_loaded(string res_id);
        int get_new_mtl_from_res(string res_id);
        int get_mtl_id(string res_id);

        bool insert_new_mtl(string res_id, gl3d_material * mtl);
        gl3d_material * get_mtl(string res_id);
        gl3d_material * get_mtl(int mtl_id);
        void release_all_mtl();

        bool load_from_sketch(string path_to_sketch_file);
        void copy_mtls_pair_info(QMap<unsigned int, gl3d_material *> &mt);

        QSet<QString> pending_mtls;

    private:
        void init();
        global_material_lib();
        static int current_max_mtl_id;
    };
};

#endif // GLOBAL_MATERIAL_H
