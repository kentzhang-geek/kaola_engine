//
//  default_objs.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/27.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>
#include "kaola_engine/model_manager.hpp"

//#if 1
//GL3D_MODEL_PARAM(144, "skybox.obj") {
//    this->obj->set_repeat(false);
//    this->obj->get_property()->scale_unit = gl3d::scale::skybox;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_MOVE | GL3D_OBJ_ENABLE_ROTATE;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_SKYBOX;
//}

// KENT WARN : 注意地面的阴影贴图必须得有！！
GL3D_MODEL_PARAM(168, "floor.3ds") {
    this->obj->get_property()->scale_unit = gl3d::scale::mm;
    auto obj_pro = this->obj->get_property();
    this->obj->set_control_authority(GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING);
    this->obj->set_render_authority(GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND);
    // TODO : need to use const alpha to draw ground
    float * alpha = new float;
    (*alpha) = 0.90;
    this->obj->user_data.clear();
    this->obj->user_data.insert(string("alpha"), alpha);
    this->obj->set_obj_type(this->obj->type_ground);
}

//GL3D_MODEL_PARAM(1, "wall-01.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
//}

//GL3D_MODEL_PARAM(2, "wall-02.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
//}
//GL3D_MODEL_PARAM(3, "wall-03.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
//}

//GL3D_MODEL_PARAM(4, "wood.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
//}

//GL3D_MODEL_PARAM(11, "sidetable.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
//    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW;
//}

////GL3D_MODEL_PARAM(5, "ceiling.3ds") {
////    this->obj->get_property()->scale_unit = gl3d::scale::mm;
////    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
////    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
////}

////GL3D_MODEL_PARAM(10, "main.3ds") {
////    this->obj->get_property()->scale_unit = gl3d::scale::mm;
////    this->obj->get_property()->authority = GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING;
////    this->obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE;
////}

//GL3D_MODEL_PARAM(11, "furniturecombination.3ds") {
//    this->obj->get_property()->scale_unit = gl3d::scale::mm;
//    this->obj->set_control_authority(GL3D_OBJ_ENABLE_NONE | GL3D_OBJ_ENABLE_CULLING);
//    this->obj->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
//}
//#endif
