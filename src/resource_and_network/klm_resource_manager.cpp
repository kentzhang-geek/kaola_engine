#include "resource_and_network/klm_resource_manager.h"

using namespace std;
using namespace klm;
using namespace klm::resource;
using namespace gl3d;

static resource::manager * local_manager = NULL;

void res_loader::run() {
    this->do_work(this->get_obj_ptr());
    return;
}

void res_loader::do_work(void *object) {
    sleep(10);
    GL3D_UTILS_THROW("default res loader test");
    return;
}

manager * manager::shared_instance() {
    if (NULL == local_manager) {
        local_manager = new manager();
    }

    return local_manager;
}

manager::manager() {
    this->id_to_item.clear();
    this->id_to_merchandise.clear();
    this->id_to_resource.clear();
    return;
}

template  <typename  T>
T * manager::get_res_item(string id) {
    return (T *)this->id_to_resource.value(id);
}

Merchandise * manager::get_merchandise_item(string id) {
    return this->id_to_merchandise.value(id);
}

void manager::perform_async_res_load(res_loader *ld, string id) {
    ld->set_obj_ptr(this->id_to_resource.value(id));
    ld->start();
    return;
}

void manager::release_all_resources() {
    return;
}