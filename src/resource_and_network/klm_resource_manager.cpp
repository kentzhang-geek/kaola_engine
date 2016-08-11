#include "resource_and_network/klm_resource_manager.h"
#include "utils/gl3d_lock.h"
#include "utils/gl3d_global_param.h"
#include <QMutex>

using namespace std;
using namespace klm;
using namespace klm::resource;
using namespace gl3d;

static resource::manager *local_manager = NULL;

static inline std::string get_file_name_by_item(resource::item in) {
    return gl3d_sandbox_path + GL3D_PATH_SEPRATOR + in.get_full_file_name();
}

void res_loader::run() {
    std::string res = manager::shared_instance()->get_res_item(this->get_obj_res_id());
    this->do_work((void *) &res);
    return;
}

void res_loader::do_work(void *object) {
    sleep(10);
    GL3D_UTILS_THROW("default res loader test");
    return;
}

static int render_id = 10086;
void resource::default_model_loader::do_work(void *object) {
    std::string * name = (std::string *)object;
    gl3d::object *obj = new gl3d::object((char *)name->c_str());

    // set property
    obj->get_property()->scale_unit = gl3d::scale::mm;
    obj->set_control_authority(this->control_code);
    obj->set_render_authority(this->render_code);
//    obj->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
//    obj->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    obj->pre_scale();
    obj->merge_meshes();
    obj->recalculate_normals();
    obj->convert_left_hand_to_right_hand();
//    obj->buffer_data();

    // for ground
    if (this->render_code | GL3D_SCENE_DRAW_GROUND) {
        float * alpha = new float;
        (*alpha) = 0.90;
        obj->user_data.clear();
        obj->user_data.insert(string("alpha"), alpha);
    }

    gl3d_lock::shared_instance()->loader_lock.lock();
    gl3d_lock::shared_instance()->render_lock.lock();
    this->main_scene->get_objects()->insert(render_id++, obj);
//    this->main_scene->add_obj(QPair<int, gl3d::abstract_object * >(render_id++, obj));
    this->main_scene->get_objects()->insert(render_id++, obj);
    gl3d_lock::shared_instance()->render_lock.unlock();
    gl3d_lock::shared_instance()->loader_lock.unlock();
}

manager *manager::shared_instance() {
    if (NULL == local_manager) {
        local_manager = new manager();
    }

    return local_manager;
}

manager::manager() {
    this->id_to_item.clear();
    this->id_to_merchandise.clear();
    this->id_to_resource.clear();

    // for test
    this->test_data_init();
    return;
}

string manager::get_res_item(string id) {
    return this->id_to_resource.value(id);
}

Merchandise *manager::get_merchandise_item(string id) {
    return this->id_to_merchandise.value(id);
}

void manager::perform_async_res_load(res_loader * ld, string id) {
    ld->set_obj_res_id(id);
//    ld->start();
    ld->run();
    return;
}

void manager::release_all_resources() {
    for (auto it = this->id_to_merchandise.begin();
            it != this->id_to_merchandise.end();
            it++) {
        delete (*it);
    }
    return;
}

void manager::test_data_init() {
    this->id_to_item.insert("000000", resource::item("000000", "floor.3ds", item::res_model_3ds, true));
    this->id_to_resource.insert("000000",
                                std::string(get_file_name_by_item(this->id_to_item.value("000000"))));
    this->id_to_merchandise.insert("000000", new klm::Furniture("000000"));

    this->id_to_item.insert("000001", resource::item("000001", "diningtable.3ds", item::res_model_3ds, true));
    this->id_to_resource.insert("000001",
                                std::string(get_file_name_by_item(this->id_to_item.value("000001"))));
    this->id_to_merchandise.insert("000001", new klm::Furniture("000001"));

    this->id_to_item.insert("000002", resource::item("000002", "sidetable.3ds", item::res_model_3ds, true));
    this->id_to_resource.insert("000002",
                                std::string(get_file_name_by_item(this->id_to_item.value("000002"))));
    this->id_to_merchandise.insert("000002", new klm::Furniture("000002"));

    this->id_to_item.insert("000003", resource::item("000003", "armchair.3ds", item::res_model_3ds, true));
    this->id_to_resource.insert("000003",
                                std::string(get_file_name_by_item(this->id_to_item.value("000003"))));
    this->id_to_merchandise.insert("000003", new klm::Furniture("000003"));

    this->id_to_item.insert("000004", resource::item("000004", "58.jpg", item::res_texture_picture, true));
    this->id_to_resource.insert("000004",
                                std::string(get_file_name_by_item(this->id_to_item.value("000004"))));
    this->id_to_merchandise.insert("000004", new klm::Surfacing("000004"));

//    this->id_to_item.insert("000005", resource::item("000005", "armchair.3ds", item::res_model_3ds, true));
//    this->id_to_resource.insert("000005",
//                                (void *)std::string(get_file_name_by_item(this->id_to_item.value("000005"))));
//    this->id_to_merchandise.insert("000006", new klm::Furniture("000005"));
//
//    this->id_to_item.insert("000006", resource::item("000006", "armchair.3ds", item::res_model_3ds, true));
//    this->id_to_resource.insert("000006",
//                                (void *)std::string(get_file_name_by_item(this->id_to_item.value("000006"))));
//    this->id_to_merchandise.insert("000006", new klm::Furniture("000006"));
}

// preload resources like ground and some else
void manager::preload_resources(gl3d::scene * sc) {
    this->perform_async_res_load(new default_model_loader(sc,
                                                           GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND,
                                                           GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING),
                                 "000000");
    this->perform_async_res_load(new default_model_loader(sc), "000001");
}
