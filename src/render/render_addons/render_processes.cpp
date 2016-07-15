//
//  render_processes.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_post_process.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"

using namespace std;
using namespace gl3d;

bool test_flag_global;

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
    gl3d::scene * one_scene = this->get_attached_scene();
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void normal::render() {
    if (has_drawed)
        return;
    gl3d::scene * one_scene = this->get_attached_scene();
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector_shadow");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->prepare_canvas(false);
    glDisable(GL_CULL_FACE);
    one_scene->draw(false);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    
    // 绘制地面蒙版
    current_shader_param = GL3D_GET_PARAM("multiple_text");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    glDisable(GL_CULL_FACE);
    one_scene->draw_stencil();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 在有模板的情况下绘制地面倒影
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    current_shader_param = GL3D_GET_PARAM("image");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_IMAGE;
    one_scene->get_property()->global_shader = string("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    glDisable(GL_STENCIL_TEST);
    
    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = string("dm");
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 最后绘制天空盒
    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void normal::after_render() {
    has_drawed = true;
}

class moving : public render_process {
public:
    void render();
};
GL3D_ADD_RENDER_PROCESS(moving);

void moving::render() {
    gl3d::scene * one_scene = this->get_attached_scene();
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("multiple_text_vector");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
    one_scene->prepare_canvas(false);
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("dm2");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("dm2");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    return;
}

class test : public render_process {
public:
    void render() {
        gl3d::scene * one_scene = this->get_attached_scene();
        // 输入阴影贴图的参数，然后绘制主图像
        gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("default");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("default");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_SKYBOX);
        one_scene->prepare_canvas(false);
        glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    }
};
GL3D_ADD_RENDER_PROCESS(test);

class editing : public render_process {
public:
    void render() {
        gl3d::scene * one_scene = this->get_attached_scene();
        // 输入阴影贴图的参数，然后绘制主图像
        gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("multiple_text_vector");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
        one_scene->prepare_canvas(false);
        glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("dm2");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("dm2");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
        glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("skybox");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("skybox");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
        glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        return;
    }

    void env_up() {
        this->get_attached_scene()->watcher->set_top_view();
    }

    void env_down() {
        this->get_attached_scene()->watcher->set_normal_view();
    }
};
GL3D_ADD_RENDER_PROCESS(editing);

class has_post : public render_process {
public:
    void pre_render();
    void render() ;
    void after_render();

    // internel calls
    void rend_shadow();
    void rend_main_scene();

    // result
    void rend_result();

    // set a rect
    object * build_rect();

    // set a material
    gl3d_material * build_material();

    // color canvas to present
    gl3d_general_texture * canvas;
};
GL3D_ADD_RENDER_PROCESS(has_post);

void has_post::pre_render() {
//    if (test_flag_global)
//        return;
    this->rend_shadow();
    // create color framebuffer
    this->canvas = new gl3d::gl3d_general_texture(
                gl3d_general_texture::GL3D_RGBA,
                2.0 * this->get_attached_scene()->get_width(),
                2.0 * this->get_attached_scene()->get_height());
    gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                        2.0 * this->get_attached_scene()->get_width(),
                        2.0 * this->get_attached_scene()->get_height());
    fb.attach_color_text(this->canvas->get_text_obj());
    fb.use_this_frame();
    this->rend_main_scene();

    QVector<string> cmd;
    cmd.clear();
    cmd.push_back(string("hdr_test"));
    this->canvas = gl3d::gl3d_post_process_set::shared_instance()->process(cmd, this->get_attached_scene(), this->canvas);
}

void has_post::render() {
//    if (test_flag_global)
//        return;
    this->rend_result();
}

void has_post::after_render() {
    test_flag_global = true;
    this->canvas = NULL;
}

void has_post::rend_shadow() {
    // 先绘制阴影贴图，给阴影贴图shader添加一下参数
    gl3d::scene * one_scene = this->get_attached_scene();
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void has_post::rend_main_scene() {
    gl3d::scene * one_scene = this->get_attached_scene();
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector_shadow");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->prepare_canvas(false);
    glDisable(GL_CULL_FACE);
    one_scene->draw(false);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 绘制地面蒙版
    current_shader_param = GL3D_GET_PARAM("multiple_text");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    glDisable(GL_CULL_FACE);
    one_scene->draw_stencil();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 在有模板的情况下绘制地面倒影
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    current_shader_param = GL3D_GET_PARAM("image");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_IMAGE;
    one_scene->get_property()->global_shader = string("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    glDisable(GL_STENCIL_TEST);

    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = string("dm");
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 最后绘制天空盒
    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void has_post::rend_result() {
    gl3d::scene * one_scene = this->get_attached_scene();
    object * rect = this->build_rect();

    rect->get_property()->authority = GL3D_OBJ_ENABLE_DEL;
    rect->get_meshes()->at(0)->set_material_index(0);
    rect->get_mtls()->insert(0, this->build_material());
    rect->get_property()->draw_authority = GL3D_SCENE_DRAW_RESULT;

    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("post_process_result");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_RESULT;
    one_scene->get_property()->global_shader = string("post_process_result");
    one_scene->add_obj(QPair<int, object *>(222, rect));
    one_scene->prepare_canvas(true);
    glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    one_scene->delete_obj(222);
    delete rect;
}

object *has_post::build_rect() {
    obj_points rect[4];
    memset(rect, 0, sizeof(obj_points) * 4);
    rect[0].vertex_x = -1.0f; // left up
    rect[0].vertex_y = 1.0f;
    rect[0].texture_x = 0.0f;
    rect[0].texture_y = 1.0f;
    rect[1].vertex_x = -1.0f; // left down
    rect[1].vertex_y = -1.0f;
    rect[1].texture_x = 0.0f;
    rect[1].texture_y = 0.0f;
    rect[2].vertex_x = 1.0f; // right up
    rect[2].vertex_y = 1.0f;
    rect[2].texture_x = 1.0f;
    rect[2].texture_y = 1.0f;
    rect[3].vertex_x = 1.0f; // right down
    rect[3].vertex_y = -1.0f;
    rect[3].texture_x = 1.0f;
    rect[3].texture_y = 0.0f;

    GLushort indexes[6] = {
        0, 2, 1,
        1, 2, 3,
    };

    object * ret = new object(rect, 4, indexes, 6);
    ret->get_meshes()->at(0)->set_bounding_value_max(glm::vec3(1.0, 1.0, 0.0));
    ret->get_meshes()->at(0)->set_bounding_value_min(glm::vec3(-1.0, -1.0, 0.0));
    ret->get_meshes()->at(0)->set_material_index(0);

    return ret;
}

gl3d_material * has_post::build_material() {
    gl3d_material * ret = new gl3d_material(this->canvas);
    return ret;
}
