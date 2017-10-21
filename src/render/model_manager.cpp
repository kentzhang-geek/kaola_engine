//
//  model_manager.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/27.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/model_manager.hpp"
#include "utils/gl3d_path_config.h"

using namespace gl3d;
using namespace std;

static model_manager * global_model_manager = NULL;

model_manager::model_manager() {
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
    this->available_id = 0;
    gl3d::object * obj_tmp;
    for (auto param : this->models) {
        obj_tmp = new gl3d::object((char *)(QString(GL3D_PATH_MODELS) + GL3D_PATH_SEPRATOR + param->file_name.c_str()).toStdString().c_str(), GL3D_PATH_MODELS);
        param->obj = obj_tmp;
        // 默认参数
        ::gl3d::obj_property * obj_pro = obj_tmp->get_property();
        obj_pro->scale_unit = gl3d::scale::mm;
        obj_tmp->coordinate_system = abstract_object::left_hand;
        param->set_param();
        // 合并顶点重算法向量
        obj_tmp->merge_meshes();
        obj_tmp->recalculate_normals();
        if (obj_tmp->coordinate_system == abstract_object::left_hand) {
            obj_tmp->convert_left_hand_to_right_hand();
            obj_tmp->coordinate_system = abstract_object::right_hand;
        }
        this->available_id =
                (param->id > this->available_id) ?
                (param->id) : this->available_id;
        main_scene->add_obj(param->id, obj_tmp);
    }
}

void model_param::set_param() {
//    ::gl3d::obj_property * obj_pro = this->obj->get_property();
//    // 默认缩放1倍
//    obj_pro->scale_unit = gl3d::scale::mm;
//    this->obj->rotate(glm::vec3(0.0, 0.0, 1.0), 90);
}

model_param::model_param(int i, string fn) : id(i), file_name(fn) , obj(NULL) {
    gl3d::model_manager::shared_instance()->models.push_back(this);
}
model_param::model_param() : id(-1), file_name(string("")), obj(NULL) {
    gl3d::model_manager::shared_instance()->models.push_back(this);
}

void model_manager::add_obj_to_scene(gl3d::scene *scene, char *obj_filename, glm::vec2 coord) {
    // 新建obj
    gl3d::object * obj = new object(GL3D_PATH_MODELS, obj_filename);new gl3d::object((char *)(QString(GL3D_PATH_MODELS) + GL3D_PATH_SEPRATOR + obj_filename).toStdString().c_str(), GL3D_PATH_MODELS);
    // 默认参数
    ::gl3d::obj_property * obj_pro = obj->get_property();
    obj_pro->scale_unit = gl3d::scale::mm;
    obj->set_control_authority(GL3D_OBJ_ENABLE_CULLING);
    obj->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    obj_pro->scale_unit = gl3d::scale::mm;
    // 合并顶点重算法向量
    obj->merge_meshes();
    obj->recalculate_normals();
    obj->convert_left_hand_to_right_hand();
    // 设置位置
    obj->get_property()->position = glm::vec3(coord.x, 0.0, coord.y);
    obj->get_property()->scale_unit = gl3d::scale::mm;
    // 缓存数据
    obj->buffer_data();
    // 添加obj到场景
    this->available_id++;
    scene->add_obj(this->available_id, obj);
}
