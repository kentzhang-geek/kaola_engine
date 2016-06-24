//
//  default_objs.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/27.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>
#include "kaola_engine/model_manager.hpp"

GL3D_MODEL_PARAM(144, "skybox.obj") {
    this->obj->set_repeat(false);
    this->obj->get_property()->scale_range = 10.0f;
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_MOVE | GL3D_OBJ_ENABLE_ROTATE;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SKYBOX;
}

#if 0
GL3D_MODEL_PARAM(144, "skybox.obj") {
    this->obj->set_repeat(false);
    this->obj->get_property()->scale_range = 10.0f;
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_MOVE | GL3D_OBJ_ENABLE_ROTATE;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SKYBOX;
}

// KENT WARN : 注意地面的阴影贴图必须得有！！
GL3D_MODEL_PARAM(168, "floor-1.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    auto obj_pro = this->obj->get_property();
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND;
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(pair<string, void *>(string("alpha"), alpha));
}
GL3D_MODEL_PARAM(169, "floor-2.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    auto obj_pro = this->obj->get_property();
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND;
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(pair<string, void *>(string("alpha"), alpha));
}
GL3D_MODEL_PARAM(170, "floor-3.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    auto obj_pro = this->obj->get_property();
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND  ;
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(pair<string, void *>(string("alpha"), alpha));
}
GL3D_MODEL_PARAM(171, "floor-4.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    auto obj_pro = this->obj->get_property();
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND  ;
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(pair<string, void *>(string("alpha"), alpha));
}

GL3D_MODEL_PARAM(172, "diningroomfloor.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    auto obj_pro = this->obj->get_property();
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND  ;
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(pair<string, void *>(string("alpha"), alpha));
}

GL3D_MODEL_PARAM(10, "main.3ds") {
    this->obj->get_property()->scale_range = 0.005;
//    this->obj->get_property()->position = glm::vec3(0.0, 0.0, 0.2);
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
    this->obj->merge_meshes();  // 合并mesh
}
GL3D_MODEL_PARAM(11, "livingroomwall.3ds") {
    this->obj->get_property()->scale_range = 0.005;
    //    this->obj->get_property()->position = glm::vec3(0.0, 0.0, 0.2);
    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
    this->obj->merge_meshes();  // 合并mesh
}


//GL3D_MODEL_PARAM(11, "furniturecombination.3ds") {
//    this->obj->get_property()->scale_range = 0.005;
//    //    this->obj->get_property()->position = glm::vec3(0.0, 0.0, 0.2);
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW;
//    this->obj->merge_meshes();  // 合并mesh
//}
#endif

// 测试
//GL3D_MODEL_PARAM(21, "oushi.3ds") {
//    this->obj->get_property()->scale_range = 0.005;
////    this->obj->get_property()->position = glm::vec3(0.0, 0.0, 0.2);
////    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_PICKING | GL3D_OBJ_ENABLE_ROTATE | GL3D_OBJ_ENABLE_MOVE | GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;
//    this->obj->merge_meshes();  // 合并mesh
//}
