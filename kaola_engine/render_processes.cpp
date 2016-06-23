//
//  render_processes.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>
#include "gl3d.hpp"
#include "gl3d_render_process.hpp"

using namespace std;
using namespace gl3d;

class normal : public render_process {
public:
    void pre_render() ;
    void render() ;
    void after_render() ;
    void invoke();
private:
    bool has_drawed;
};
GL3D_ADD_RENDER_PROCESS(normal);

void normal::invoke() {
    has_drawed = false;
}

void normal::pre_render() {
    if (has_drawed)
        return;
    // 先绘制阴影贴图，给阴影贴图shader添加一下参数
    gl3d::scene * one_scene = (gl3d::scene * )this->get_user_object(string("scene"));
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(string("scene"));
}

void normal::render() {
    if (has_drawed)
        return;
    gl3d::scene * one_scene = (gl3d::scene * )this->get_user_object(string("scene"));
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector_shadow");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->prepare_canvas(false);
    glDisable(GL_CULL_FACE);
    one_scene->draw(false);
    current_shader_param->user_data.erase(string("scene"));
    
    // 绘制地面蒙版
    current_shader_param = GL3D_GET_PARAM("multiple_text");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    glDisable(GL_CULL_FACE);
    one_scene->draw_stencil();
    current_shader_param->user_data.erase(string("scene"));
    
    // 在有模板的情况下绘制地面倒影
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    current_shader_param = GL3D_GET_PARAM("image");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_IMAGE;
    one_scene->get_property()->global_shader = string("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
    glDisable(GL_STENCIL_TEST);
    
    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = string("dm");
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
    
    // 最后绘制天空盒
    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
}

void normal::after_render() {
    has_drawed = false;
}

class moving : public render_process {
public:
    void render();
};
GL3D_ADD_RENDER_PROCESS(moving);

void moving::render() {
    gl3d::scene * one_scene = (gl3d::scene * )this->get_user_object(string("scene"));
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("multiple_text_vector");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
    one_scene->prepare_canvas(false);
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
    
    current_shader_param = GL3D_GET_PARAM("dm2");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("dm2");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
    
    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(pair<string, void *>(string("scene"), one_scene));
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(string("scene"));
    return;
}
