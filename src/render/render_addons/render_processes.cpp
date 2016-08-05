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
#include "utils/gl3d_post_process_template.h"
#include <QVector>

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
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(false);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    
    // 绘制地面蒙版
    current_shader_param = GL3D_GET_PARAM("multiple_text");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw_stencil();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 在有模板的情况下绘制地面倒影
    GL3D_GL()->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
    GL3D_GL()->glEnable(GL_STENCIL_TEST);
    GL3D_GL()->glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    GL3D_GL()->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    current_shader_param = GL3D_GET_PARAM("image");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_IMAGE;
    one_scene->get_property()->global_shader = string("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    GL3D_GL()->glDisable(GL_STENCIL_TEST);
    
    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = string("dm");
    GL3D_GL()->glDisable(GL_CULL_FACE);
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
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("dm2");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("dm2");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = string("skybox");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    return;
}

GL3D_SHADER_PARAM(lines) {
    return true;
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
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    }
};
GL3D_ADD_RENDER_PROCESS(test);

using namespace gl3d;
class editing : public render_process {
public:
    // 暂时没搞定网格
    void draw_coord() {
        QVector<gl3d::mesh *> ms;
        ms.clear();
        obj_points pt[80];
        // generate datas
        GLushort idx[80];
        for (int i = 0; i < 80; i++)
            idx[i] = i;
        float step = 1.0f;
        for (int i = 0; i < 40; i++) {
            pt[i * 2 + 0].vertex_x = step * (float)(i - 20);
            pt[i * 2 + 0].vertex_y = -1.0f;
            pt[i * 2 + 0].vertex_z = -20.0f;
            pt[i * 2 + 1].vertex_x = step * (float)(i - 20);
            pt[i * 2 + 1].vertex_y = -1.0f;
            pt[i * 2 + 1].vertex_z = 20.0f;
        }
        ms.push_back(new gl3d::mesh(pt, 80, idx, 80));
        for (int i = 0; i < 40; i++) {
            pt[i * 2 + 0].vertex_x = -20.0f;
            pt[i * 2 + 0].vertex_y = -1.0f;
            pt[i * 2 + 0].vertex_z = step * (float)(i - 20);
            pt[i * 2 + 1].vertex_x = 20.0f;
            pt[i * 2 + 1].vertex_y = -1.0f;
            pt[i * 2 + 1].vertex_z = step * (float)(i - 20);
        }
        ms.push_back(new gl3d::mesh(pt, 80, idx, 80));
        ms[0]->buffer_data();
        ms[1]->buffer_data();

        // get shader
        Program * use_shader = GL3D_GET_SHADER("lines");
        GL3D_GL()->glUseProgram(use_shader->getProgramID());
        GL3D_GL()->glEnable(GL_LINE_SMOOTH);
        GL3D_GL()->glEnable(GL_DEPTH_TEST);
        GL3D_GL()->glDisable(GL_POLYGON_OFFSET_LINE);

        ::glm::mat4 pvm = *(this->get_attached_scene()->watcher->get_projection_matrix()) *
                *(this->get_attached_scene()->watcher->get_viewing_matrix());
        GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                    gl3d::gl3d_global_param::shared_instance()->canvas_width);
        pvm = pvm * ::glm::scale(glm::mat4(1.0), glm::vec3(s_range));
        GL3D_GL()->glUniformMatrix4fv
                (GL3D_GL()->glGetUniformLocation
                 (use_shader->getProgramID(), "pvmMatrix"),
                 1, GL_FALSE, glm::value_ptr(pvm));

        for (int i = 0; i < 2; i++) {
            // set buffers
            GL3D_GL()->glBindVertexArray(0);
            gl3d::scene::set_attribute(use_shader->getProgramID());
            GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, ms[i]->get_vbo());
            GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms[i]->get_idx());
            gl3d::scene::set_attribute(use_shader->getProgramID());
            GL3D_GL()->glLineWidth(0.1f);
            GL3D_GL()->glDrawElements(GL_LINES,
                                      ms[i]->get_num_idx(),
                                      GL_UNSIGNED_SHORT,
                                      (GLvoid *)NULL);
            GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, 0);
            GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            GL3D_GL()->glBindVertexArray(0);
        }

        // clear data
        delete ms[0];
        delete ms[1];
        ms.clear();
    }

    void render() {
        gl3d::scene * one_scene = this->get_attached_scene();
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());

        gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("multiple_text_vector");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
        one_scene->prepare_canvas(false);
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("dm2");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("dm2");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("skybox");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = string("skybox");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        this->draw_coord();
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
//    cmd.push_back(string("simple_directional_light")); // no light for now
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
    one_scene->get_property()->global_shader = string("multiple_text_vector_shadow");
    one_scene->prepare_canvas(false);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(false);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 绘制地面蒙版
    current_shader_param = GL3D_GET_PARAM("multiple_text");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw_stencil();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 在有模板的情况下绘制地面倒影
    GL3D_GL()->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
    GL3D_GL()->glEnable(GL_STENCIL_TEST);
    GL3D_GL()->glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    GL3D_GL()->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    current_shader_param = GL3D_GET_PARAM("image");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_IMAGE;
    one_scene->get_property()->global_shader = string("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    GL3D_GL()->glDisable(GL_STENCIL_TEST);

    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = string("dm");
    GL3D_GL()->glDisable(GL_CULL_FACE);
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
    special_obj * rect = new special_obj(this->canvas);

    rect->set_control_authority(GL3D_OBJ_ENABLE_DEL);
    rect->set_render_authority(GL3D_SCENE_DRAW_RESULT);

    gl3d::shader_param * current_shader_param = GL3D_GET_PARAM("post_process_result");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_RESULT;
    one_scene->get_property()->global_shader = string("post_process_result");
    one_scene->add_obj(QPair<int, object *>(222, rect));
    GL3D_GL()->glViewport(0, 0,
                          one_scene->get_width(), one_scene->get_height());
    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    one_scene->delete_obj(222);
    delete rect;
}

gl3d_material * has_post::build_material() {
    gl3d_material * ret = new gl3d_material(this->canvas);
    return ret;
}
