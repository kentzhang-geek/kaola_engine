//
//  model_manager.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/27.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "model_manager.hpp"

using namespace gl3d;
using namespace std;

static model_manager * global_model_manager = NULL;

model_manager::model_manager() {
//    bzero(this, sizeof(model_manager));
    models.clear();
    global_model_manager = this;
}

model_manager * model_manager::shared_instance() {
    if (NULL == global_model_manager) {
        global_model_manager = new model_manager();
    }
    
    return global_model_manager;
}

void model_manager::release_loaders() {
    auto iter = this->models.begin();
    for (; iter != this->models.end(); iter++) {
        delete *iter;
    }
    this->models.clear();
}

void model_manager::init_objs(gl3d::scene *main_scene) {
    auto iter = this->models.begin();
    this->available_id = 0;
    gl3d::object * obj_tmp;
    for (; iter != this->models.end(); iter++) {
        obj_tmp = new gl3d::object((char *)(gl3d_sandbox_path + "/" + (*iter)->file_name).c_str());
        (*iter)->obj = obj_tmp;
        (*iter)->set_param();
        this->available_id =
        ((*iter)->id > this->available_id) ?
        ((*iter)->id) : this->available_id;
        main_scene->add_obj(pair<int, gl3d::object *>((*iter)->id, obj_tmp));
    }
}

void model_param::set_param() {
    ::gl3d::obj_property * obj_pro = this->obj->get_property();
    // 默认缩放1倍，旋转90度
    obj_pro->scale_range = 1.0;
    this->obj->rotate(glm::vec3(1.0, 0.0, 0.0), 90);
}

model_param::model_param(int i, string fn) : id(i), file_name(fn) , obj(NULL) {
    gl3d::model_manager::shared_instance()->models.push_back(this);
}
model_param::model_param() : id(-1), file_name(string("")), obj(NULL) {
    gl3d::model_manager::shared_instance()->models.push_back(this);
}

void model_manager::add_obj_to_scene(gl3d::scene *scene, char *obj_filename, glm::vec2 coord) {
    // 新建obj
    gl3d::object * obj = new object((char *)(gl3d_sandbox_path + "/" + obj_filename).c_str());
    // 设置位置
    obj->get_property()->position = glm::vec3(coord, 0.0);
    obj->get_property()->scale_range = 0.005;  // KENT TODO : 这里默认缩放比例是否需要可调?
    // 缓存数据
    obj->get_property()->draw_authority |=  GL3D_OBJ_ENABLE_CULLING;
    obj->buffer_data();
    // 添加obj到场景
    this->available_id++;
    scene->add_obj(std::pair<int, gl3d::object *>(this->available_id, obj));
}
