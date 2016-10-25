#include "resource_and_network/klm_resource_manager.h"
#include "utils/gl3d_lock.h"
#include "utils/gl3d_global_param.h"
#include <QMutex>
#include <include/editor/command.h>
#include "resource_and_network/network_tool.h"
#include "resource_and_network/pack_tool.h"
#include "resource_and_network/global_info.h"

using namespace std;
using namespace klm;
using namespace klm::resource;
using namespace gl3d;

static resource::manager *local_manager = NULL;

static inline std::string get_file_name_by_item(resource::item in) {
    string ret;
    ret = string(KLM_DOWNLOAD_TMP_PATH) + GL3D_PATH_SEPRATOR + in.get_res_id() + GL3D_PATH_SEPRATOR + in.get_full_file_name();
    return ret;
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

void resource::default_model_loader::do_work(void *object) {
    // download furniture
    if (!klm::resource::manager::shared_instance()->local_resource_map.contains(this->get_obj_res_id())) {
        klm::network::call_web_download(this->url, "tmp\\tmpfile.7z");
    }
    // unpack
    klm::pack_tool packer;
    QEventLoop loop;
    packer.unpack("tmp\\tmpfile.7z", klm::resource::manager::get_base_path_by_resid(this->get_obj_res_id()).c_str());
    connect(&packer, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QFile::remove("tmp\\tmpfile.7z");
    // load obj
    gl3d::object *obj = new gl3d::object((char *)
            (klm::resource::manager::get_base_path_by_resid(this->get_obj_res_id()) + GL3D_PATH_SEPRATOR +
             KLM_FURNITURE_FILENAME).c_str(),
                                         klm::resource::manager::get_base_path_by_resid(this->get_obj_res_id()));
    obj->set_res_id(this->get_obj_res_id());

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

    gl3d_lock::shared_instance()->render_lock.lock();
    if (this->main_scene->get_attached_sketch()->get_objects()->contains(this->obj_render_id)){
        abstract_object * o = this->main_scene->get_obj(this->obj_render_id);
        if ((o->get_obj_type() == o->type_furniture) || (o->get_obj_type() == o->type_loading_object)) {
            obj->get_property()->position = o->get_abs_position();
            obj->get_property()->rotate_mat = o->get_abs_rotation();
            obj->set_obj_type(obj->type_furniture);
            this->main_scene->delete_obj(this->obj_render_id);
            delete o;
            this->main_scene->get_attached_sketch()->add_obj(this->obj_render_id, obj);
            klm::command::command_stack::shared_instance()->push(new klm::command::add_obj(obj)); // change : add command
        }
    }
    else {
        this->main_scene->get_attached_sketch()->add_obj(this->obj_render_id, obj);
        command::command_stack::shared_instance()->push(new command::add_obj(obj));  // add obj undo
    }
    gl3d_lock::shared_instance()->render_lock.unlock();

    this->terminate();
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
    this->local_resource_map.clear();
    this->load_local_databse();

    // make tmp dir
    QDir tmp("tmp");
    if (!tmp.exists()) {
        QDir::current().mkdir("tmp");
    }
    local_dir = QDir::currentPath();

    return;
}

string manager::get_base_path_by_resid(string resid) {
    return string(KLM_DOWNLOAD_TMP_PATH) + GL3D_PATH_SEPRATOR + resid + GL3D_PATH_SEPRATOR;
}

string manager::get_res_item(string id) {
    return this->id_to_resource.value(id);
}

Merchandise *manager::get_merchandise_item(string id) {
    return this->id_to_merchandise.value(id);
}

void manager::perform_async_res_load(res_loader * ld, string id) {
    ld->set_obj_res_id(id);
    ld->start();
//    ld->run();
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

void manager::load_local_databse() {
    pugi::xml_document doc;
    doc.load_file(GL3D_LOCAL_DATABASE);
    pugi::xml_node rt = doc.root().child("local_res");
    auto reses = rt.children("res");
    for (auto rit = reses.begin();
         rit != reses.end();
         rit++) {
        string resid = string(rit->attribute("id").as_string());
        string res_value = string(rit->attribute("value").as_string());
        int type = rit->attribute("item_type").as_int();
        QFile f(QString::fromStdString(
                get_file_name_by_item(resource::item(resid, res_value, (item::resource_type) type, true))));
        if (f.exists()) {
            this->id_to_item.insert(resid, resource::item(resid, res_value, (item::resource_type) type, true));
            this->id_to_resource.insert(resid, get_file_name_by_item(this->id_to_item.value(resid)));
            if (type == item::resource_type::res_model_3ds) {
                this->id_to_merchandise.insert(resid, new klm::Furniture(resid));
            }
            else if (type == item::resource_type::res_texture_picture) {
                this->id_to_merchandise.insert(resid, new klm::Surfacing(resid));
            }
        }
        else {
            qDebug("local data %s not exist", f.fileName().toStdString().c_str());
        }
    }
}

void manager::save_local_databse() {
    pugi::xml_document doc;
    pugi::xml_node rt = doc.root().append_child("local_res");
    Q_FOREACH(auto iit, this->id_to_item) {
            auto nd = rt.append_child("res");
            nd.append_attribute("id").set_value(iit.get_res_id().c_str());
            nd.append_attribute("value").set_value(iit.get_full_file_name().c_str());
            nd.append_attribute("item_type").set_value((int) iit.get_res_type());
        }
    doc.save_file(GL3D_LOCAL_DATABASE);

    return;
}

#if 1
void manager::test_data() {
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

    this->id_to_item.insert("mtl000000", resource::item("mtl000000", "__2.jpg", item::res_texture_picture, true));
    this->id_to_resource.insert("mtl000000",
                                std::string(get_file_name_by_item(this->id_to_item.value("mtl000000"))));
    this->id_to_merchandise.insert("mtl000000", new klm::Surfacing("mtl000000"));

    this->id_to_item.insert("mtl000001", resource::item("mtl000001", "_35.jpg", item::res_texture_picture, true));
    this->id_to_resource.insert("mtl000001",
                                std::string(get_file_name_by_item(this->id_to_item.value("mtl000001"))));
    this->id_to_merchandise.insert("mtl000001", new klm::Surfacing("mtl000001"));

    this->id_to_item.insert("mtl000002", resource::item("mtl000002", "__2.jpg", item::res_texture_picture, true));
    this->id_to_resource.insert("mtl000002",
                                std::string(get_file_name_by_item(this->id_to_item.value("mtl000002"))));
    this->id_to_merchandise.insert("mtl000002", new klm::Surfacing("mtl000002"));

    this->id_to_item.insert("door000000", resource::item("door000000", "door.3ds", item::res_texture_picture, true));
    this->id_to_resource.insert("door000000",
                                std::string(get_file_name_by_item(this->id_to_item.value("door000000"))));
    this->id_to_merchandise.insert("door000000", new klm::Furniture("door000000"));

    this->id_to_item.insert("window000000", resource::item("window000000", "door.3ds", item::res_texture_picture, true));
    this->id_to_resource.insert("window000000",
                                std::string(get_file_name_by_item(this->id_to_item.value("window000000"))));
    this->id_to_merchandise.insert("window000000", new klm::Furniture("window000000"));

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
#endif

// preload resources like ground and some else
void manager::preload_resources(gl3d::scene * sc) {
//    this->perform_async_res_load(new default_model_loader(sc,
//                                                          GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND,
//                                                          GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING),
//                                 "000000");
}

void manager::downlaod_res(QString url, QString res_id, QString filename, resource::item::resource_type rtype) {
    klm::network::call_web_download(url, "tmp/stmp.7z");
    klm::pack_tool pkt;
    pkt.unpack("tmp/stmp.7z", QString("tmp\\") + res_id + GL3D_PATH_SEPRATOR);
    QFile::remove("tmp/stmp.7z");

    // insert resource item to localdatabase
    this->id_to_item.insert(res_id.toStdString(),
                            resource::item(res_id.toStdString(), filename.toStdString(), (item::resource_type) rtype,
                                           true));
    this->id_to_resource.insert(res_id.toStdString(),
                                get_file_name_by_item(this->id_to_item.value(res_id.toStdString())));
    if (rtype == item::resource_type::res_model_3ds) {
        this->id_to_merchandise.insert(res_id.toStdString(), new klm::Furniture(res_id.toStdString()));
    }
    else if (rtype == item::resource_type::res_texture_picture) {
        this->id_to_merchandise.insert(res_id.toStdString(), new klm::Surfacing(res_id.toStdString()));
    }

    return;
}

static klm::info * one_info = NULL;

klm::info::info() :
is_login(false),
username(),
password(),
design_id(),
plan_id(),
d_p_path(),
housename("housename"),
house_url("http://www.baidu.com/")
{

}

info* klm::info::shared_instance() {
    if (NULL == one_info) {
        one_info = new klm::info();
    }

    return one_info;
}