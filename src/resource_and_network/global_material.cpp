#include "resource_and_network/global_material.h"

using namespace gl3d;
using namespace klm;
using namespace std;

static global_material_lib *g_mtl_lib = NULL;

int global_material_lib::current_max_mtl_id = 1;

static QMutex mtl_loader_lock;

void global_material_lib::init() {
    this->res_id_to_mtl_id.clear();
    this->mtl_id_to_res_id.clear();
    this->mtl_id_to_materials.clear();
    this->current_max_mtl_id = 1;
    this->res_id_to_mtl_id.insert("", 0);
    this->mtl_id_to_res_id.insert(0, "");
    this->mtl_id_to_materials.insert(0, new gl3d_material());
    this->pending_mtls.clear();
}

global_material_lib::global_material_lib() {
    this->init();
}

global_material_lib *global_material_lib::shared_instance() {
    if (NULL == g_mtl_lib) {
        g_mtl_lib = new global_material_lib();
    }

    return g_mtl_lib;
}

int global_material_lib::get_new_mtl_from_res(string res_id) {
    if (!this->res_id_to_mtl_id.contains(res_id)) {
        mtl_loader_lock.lock();
        string path = klm::resource::manager::shared_instance()->get_res_item(res_id);
        gl3d_material *mtl = new gl3d_material(path.c_str());
        this->res_id_to_mtl_id.insert(res_id, this->current_max_mtl_id);
        this->mtl_id_to_res_id.insert(this->current_max_mtl_id, res_id);
        this->mtl_id_to_materials.insert(this->current_max_mtl_id, mtl);
        this->current_max_mtl_id++;
        mtl_loader_lock.unlock();
    }

    return this->res_id_to_mtl_id.value(res_id);
}

int global_material_lib::get_mtl_id(string res_id) {
    if (this->is_mtl_loaded(res_id)) {
        return this->res_id_to_mtl_id.value(res_id);
    }

    return 0;
}

class mtl_loader : public klm::resource::res_loader {
public:
    string res_id;
    global_material_lib::load_mtl_callback *callback;
    void *user_data;

    mtl_loader(string id, global_material_lib::load_mtl_callback *cbk, void *udata) :
            klm::resource::res_loader(),
            res_id(id),
            callback(cbk),
            user_data(udata) {}

    void do_work(void *object) {
        string *file = (string *) object;
        if (!global_material_lib::shared_instance()->is_mtl_loaded(this->res_id)) {
            gl3d_material *mtl = new gl3d_material(file->c_str());
            if (!global_material_lib::shared_instance()->insert_new_mtl(this->res_id, mtl)) {
                delete mtl;
            }
            global_material_lib::shared_instance()->pending_mtls.remove(QString::fromStdString(this->res_id));
        }
        if (NULL != this->callback)
            this->callback(global_material_lib::shared_instance()->get_res_id_to_mtl_id()->value(this->res_id),
                           global_material_lib::shared_instance()->get_mtl(this->res_id),
                           this->user_data);
        this->terminate();
    }
};

void global_material_lib::request_new_mtl(string res_id, load_mtl_callback *cbk, void *udata) {
    if (this->pending_mtls.contains(QString::fromStdString(res_id))) {
        return;
    }
    mtl_loader *l = new mtl_loader(res_id, cbk, udata);
    this->pending_mtls.insert(QString::fromStdString(res_id));
    klm::resource::manager::shared_instance()->perform_async_res_load(l, res_id);
    return;
}

bool global_material_lib::is_mtl_loaded(string res_id) {
    if (this->res_id_to_mtl_id.contains(res_id)) {
        if (this->mtl_id_to_materials.contains(this->res_id_to_mtl_id.value(res_id))) {
            return true;
        }
    }

    return false;
}

bool global_material_lib::insert_new_mtl(string res_id, gl3d_material *mtl) {
    mtl_loader_lock.lock();
    if (this->is_mtl_loaded(res_id)) {
        mtl_loader_lock.unlock();
        return false;
    }
    this->res_id_to_mtl_id.insert(res_id, this->current_max_mtl_id);
    this->mtl_id_to_res_id.insert(this->current_max_mtl_id, res_id);
    this->mtl_id_to_materials.insert(this->current_max_mtl_id, mtl);
    this->current_max_mtl_id++;
    mtl_loader_lock.unlock();
    return true;
}

void global_material_lib::release_all_mtl() {
    mtl_loader_lock.lock();
    this->res_id_to_mtl_id.clear();
    this->mtl_id_to_res_id.clear();
    for (auto it = this->mtl_id_to_materials.begin();
         it != this->mtl_id_to_materials.end();
         it++) {
        delete it.value();
    }
    this->mtl_id_to_materials.clear();
    this->current_max_mtl_id = 1;
    this->init();
    mtl_loader_lock.unlock();
}

gl3d_material *global_material_lib::get_mtl(string res_id) {
    if (this->is_mtl_loaded(res_id)) {
        return this->mtl_id_to_materials.value(this->res_id_to_mtl_id.value(res_id));
    }
    else {
        return NULL;
    }
}

gl3d_material *global_material_lib::get_mtl(int mtl_id) {
    if (this->mtl_id_to_materials.contains(mtl_id)) {
        return this->mtl_id_to_materials.value(mtl_id);
    }
    else {
        return NULL;
    }
}

bool global_material_lib::load_from_sketch(string path_to_sketch_file) {
    GL3D_UTILS_THROW("does not implement load mtl from sketch now");
    return false;
}

void global_material_lib::copy_mtls_pair_info(QMap<unsigned int, gl3d_material *> &mt) {
    for (auto it = this->mtl_id_to_materials.begin();
         it != this->mtl_id_to_materials.end();
         it++) {
        mt.insert(it.key(), it.value());
    }
}
