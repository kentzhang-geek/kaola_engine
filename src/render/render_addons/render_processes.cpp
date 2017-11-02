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
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_post_process_template.h"

using namespace std;
using namespace gl3d;

bool test_flag_global;

class normal : public render_process {
public:
    void pre_render();

    void render();

    void after_render();

    void invoke();

private:
    bool has_drawed;
};

GL3D_ADD_RENDER_PROCESS(normal);

void normal::invoke() {
//    has_drawed = false;
}

void normal::pre_render() {
//    if (has_drawed)
//        return;
    // 先绘制阴影贴图，给阴影贴图shader添加一下参数
    gl3d::scene *one_scene = this->get_attached_scene();
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void normal::render() {
    if (has_drawed)
        return;
    gl3d::scene *one_scene = this->get_attached_scene();
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("multiple_text_vector_shadow");
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
    one_scene->get_property()->global_shader = QString("image");
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    GL3D_GL()->glDisable(GL_STENCIL_TEST);

    // 绘制地面
    current_shader_param = GL3D_GET_PARAM("dm");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    one_scene->get_property()->global_shader = QString("dm");
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    // 最后绘制天空盒
    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    one_scene->get_property()->global_shader = QString("skybox");
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
    gl3d::scene *one_scene = this->get_attached_scene();
    // 输入阴影贴图的参数，然后绘制主图像
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = QString("multiple_text_vector");
    one_scene->get_property()->current_draw_authority =
            GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
    one_scene->prepare_canvas(false);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("dm2");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = QString("dm2");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    current_shader_param = GL3D_GET_PARAM("skybox");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    // 选择全局渲染器
    one_scene->get_property()->global_shader = QString("skybox");
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    return;
}

class test : public render_process {
public:
    void render() {
        gl3d::scene *one_scene = this->get_attached_scene();
        // 输入阴影贴图的参数，然后绘制主图像
        gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("default");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = QString("default");
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
    object *draw_coord() {
        gl3d::obj_points pt[4] = {
                {-100.0, -1.0, 100.0,  // vt
                        0.0, 0.0, 0.0,       // normal
                        0.0, 0.0, 0.0, 0.0,  // color
                        0.0,   400.0,     // text
                        0},
                {-100.0, -1.0, -100.0,  // vt
                        0.0, 0.0, 0.0,   // normal
                        0.0, 0.0, 0.0, 0.0, // color
                        0.0,   0.0,     // text
                        0},
                {100.0,  -1.0, 100.0,   // vt
                        0.0, 0.0, 0.0,   // normal
                        0.0, 0.0, 0.0, 0.0, // color
                        400.0, 400.0,     // text
                        0},
                {100.0,  -1.0, -100.0,  // vt
                        0.0, 0.0, 0.0,   // normal
                        0.0, 0.0, 0.0, 0.0, // color
                        400.0, 0.0,     // text
                        0},
        };

        static GLushort idx[6] = {
                0, 2, 1,
                1, 2, 3,
        };

        gl3d::object *obj = new object(pt, 4, idx, 6);
        obj->set_repeat(true);
        obj->get_meshes()->at(0)->set_texture_repeat(true);
        obj->get_meshes()->at(0)->set_material_index(0);
        obj->get_mtls()->insert(0, new gl3d_material(string(KLM_RES_PATH) + GL3D_PATH_SEPRATOR + "net.jpg"));
        obj->get_property()->scale_unit = gl3d::scale::m;
        obj->set_render_authority(GL3D_SCENE_DRAW_NET);
        obj->set_control_authority(GL3D_OBJ_ENABLE_DEL);

//        shader_param * param = GL3D_GET_PARAM("lines");
//        gl3d::scene::set_attribute(GL3D_GET_SHADER("lines")->getProgramID());
//        param->user_data.insert(string("object"), (void *)obj);
//        param->user_data.insert(string("scene"), this->get_attached_scene());
//        param->set_param();
//        // then delete param
//        param->user_data.erase(param->user_data.find(string("object")));
//        GL3D_GL()->glBindFramebuffer(GL_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
//        GL3D_GL()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
//        this->get_attached_scene()->draw_object(obj,
//                                                GL3D_GET_SHADER("lines")->getProgramID());

        // send data
        return obj;
    }

    void render() {
        gl3d::scene *one_scene = this->get_attached_scene();
//        one_scene->get_attached_sketch()->draw_assistant_image(one_scene->get_assistant_image());
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());

        // 选择全局渲染器渲染地面网格
        gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("lines");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        one_scene->get_property()->global_shader = QString("lines");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NET;
        one_scene->prepare_canvas(false);
        object *oo = this->draw_coord();
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());
        one_scene->drawSpecialObject(oo, true);
        delete oo;
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("multiple_text_vector");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = QString("multiple_text_vector");
        one_scene->get_property()->current_draw_authority =
                GL3D_SCENE_DRAW_ALL & (~GL3D_SCENE_DRAW_GROUND) & (~GL3D_SCENE_DRAW_SKYBOX);
//        object * oo = this->draw_coord();
//        one_scene->add_obj(QPair<int, abstract_object *>(23123, oo));
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());
        one_scene->draw(true);
//        one_scene->delete_obj(23123);
//        delete oo;
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("dm2");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = QString("dm2");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_GROUND;
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        current_shader_param = GL3D_GET_PARAM("skybox");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        one_scene->get_property()->global_shader = QString("skybox");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SKYBOX;
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        // TODO : draw assistant
        current_shader_param = GL3D_GET_PARAM("post_process_result");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        one_scene->get_property()->global_shader = QString("post_process_result");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SPECIAL;
        gl3d_general_texture *txt =
                new gl3d_general_texture(gl3d_general_texture::GL3D_RGBA,
                                         (GLuint) this->get_attached_scene()->get_assistant_image()->width(),
                                         (GLuint) this->get_attached_scene()->get_assistant_image()->height());
        txt->buffer_data(this->get_attached_scene()->get_assistant_image()->bits());
        oo = (object *)new special_obj(txt);
        obj_points * opt = oo->get_meshes()->at(0)->get_points_data();
        for (int ii = 0; ii < oo->get_meshes()->at(0)->get_num_pts(); ii++) {
            opt[ii].texture_y = 1.0f - opt[ii].texture_y;
        }
        oo->set_render_authority(GL3D_SCENE_DRAW_SPECIAL);
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glDisable(GL_DEPTH_TEST);
        GL3D_GL()->glViewport(0, 0, one_scene->get_width(), one_scene->get_height());
        one_scene->drawSpecialObject(oo, true);
        GL3D_GL()->glEnable(GL_DEPTH_TEST);
        delete oo;
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

//        one_scene->get_assistant_image()->fill(0);
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

class day : public render_process {
public:
    void pre_render();

    void render();

    void after_render();

    // internel calls
    void rend_shadow();

    void rend_main();

    // result
    void rend_result();

    // set a material
    gl3d_material *build_material();

    // color canvas to present
    gl3d_general_texture *canvas;
};

GL3D_ADD_RENDER_PROCESS(day);

void day::pre_render() {
//    this->rend_shadow();
    // render g-buffer
    int gbufWidth = 2 * (int)this->get_attached_scene()->get_width();
    int gbufHeight = 2 * (int)this->get_attached_scene()->get_height();

    gl3d::scene *one_scene = this->get_attached_scene();
    bool drawSunshine = false;
    glm::vec3 sunPos(300.0f, 300.0f, 300.0f);
    glm::vec3 lookdir = one_scene->watcher->get_look_direction();
    glm::vec3 backClr = glm::vec3(one_scene->get_property()->background_color);
    gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                        gbufWidth, gbufHeight);
    GLuint gBuffer = fb.get_frame_obj();
    GLuint gPosition, gNormal, gColorSpec;

// - 颜色缓冲
    gl3d_general_texture * gColor = new gl3d::gl3d_general_texture(
            gl3d_general_texture::GL3D_RGBA,
            2.0 * this->get_attached_scene()->get_width(),
            2.0 * this->get_attached_scene()->get_height());
    fb.attach_color_text(gColor->get_text_obj());

    // 位置缓冲
    GL3D_GL()->glGenTextures(1, &gPosition);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gPosition);
    GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, gbufWidth, gbufHeight, 0, GL_RGB, GL_FLOAT, NULL);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT1,
                                      GL_TEXTURE_2D, gPosition, 0);

    // - 法线颜色缓冲
    GL3D_GL()->glGenTextures(1, &gNormal);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gNormal);
    GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, gbufWidth, gbufHeight, 0, GL_RGB, GL_FLOAT, NULL);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

//// - 漫反射 + Specular颜色缓冲
//    glGenTextures(1, &gAlbedoSpec);
//    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

// - 告诉OpenGL我们要使用哪个颜色附件来渲染
    fb.use_this_frame();
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    GL3D_GL()->glDrawBuffers(3, attachments);
    this->rend_main();

    // checkwith stencil
    if (lookdir.y > 0) {
        fb.use_this_frame();
        glm::vec2 sunpt = one_scene->project_point_to_screen(glm::vec3(300.0f));
        sunpt = sunpt * 2.0f;
        sunpt.y = gbufHeight - sunpt.y;
        glm::vec2 sunTan(sunpt.x - gbufWidth / 2.0, sunpt.y - gbufHeight / 2.0f);
        float angleTmp = glm::atan(glm::abs(sunTan.y) / glm::abs(sunTan.x));
        if (math::point_in_range(sunpt, glm::vec2(0.0), glm::vec2(gbufWidth, gbufHeight))) {
            if ((sunTan.y > 0.0) &&
                (glm::abs(sunTan.x) > 0.05f) &&
                (angleTmp > (glm::pi<float>() / 12.0)) &&
                (angleTmp < (glm::pi<float>() / 4.0))) {
                GLubyte tmpdata[4];
                // get pixel
                GL3D_GL()->glReadPixels(sunpt.x, sunpt.y, 1, 1, GL_RGBA,  GL_UNSIGNED_BYTE, tmpdata);
                // now can check if sunshine should draw
                if ((tmpdata[0] || tmpdata[1] || tmpdata[2] || tmpdata[3]) == 0) {
                    drawSunshine = true;
                }
            }
        }
    }

    // now render with light
    this->canvas = new gl3d::gl3d_general_texture(
            gl3d_general_texture::GL3D_RGBA,
            2.0 * this->get_attached_scene()->get_width(),
            2.0 * this->get_attached_scene()->get_height());
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0); // detach
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0); // detach
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0); // detach
    fb.attach_color_text(this->canvas->get_text_obj());

    // set g buffer and its texture
    Program * lightShader = GL3D_GET_SHADER("day");
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("day");
    GL3D_GL()->glUseProgram(lightShader->getProgramID());
    auto posText = new gl3d_general_texture(gPosition, gbufWidth, gbufHeight);
    auto normalText = new gl3d_general_texture(gNormal, gbufWidth, gbufHeight);
    special_obj *rect = new special_obj(gColor, posText, normalText);
    rect->buffer_data();
    // set shader
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->get_property()->global_shader = QString("day");
    current_shader_param->set_param();
//    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    GL3D_GL()->glDrawBuffers(1, attachments);
    auto location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "sunPos");
    GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(sunPos));
    location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "lookto");
    GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(lookdir));
    location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "backColor");
    GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(backClr));
    location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "drawSun");
    GL3D_GL()->glUniform1i(location, 0);
//    GL3D_GL()->glEnable(GL_BLEND);
//    GL3D_GL()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // set stencil
    GL3D_GL()->glEnable(GL_STENCIL_TEST);
    GL3D_GL()->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    GL3D_GL()->glStencilFunc(GL_NOTEQUAL, 0, 0xffff);
    GL3D_GL()->glClear(GL_COLOR_BUFFER_BIT);
    one_scene->drawSpecialObject(rect, true);       // draw peoples
    GL3D_GL()->glUniform1i(location, 1);
    location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "backColor");
    GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(backClr));
    GL3D_GL()->glStencilFunc(GL_EQUAL, 0, 0xffff);        // draw sun
    one_scene->drawSpecialObject(rect, true);
    GL3D_GL()->glDisable(GL_STENCIL_TEST);
//        one_scene->drawObjectInstanced(lightShader->getProgramID(), true, i, rect);
    // draw sun shine
    if (drawSunshine) {
        Program * shineShader = GL3D_GET_SHADER("sunshine");
        current_shader_param = GL3D_GET_PARAM("sunshine");
        GL3D_GL()->glUseProgram(shineShader->getProgramID());
        current_shader_param->user_data.insert(string("scene"), one_scene);
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
        one_scene->get_property()->global_shader = QString("sunshine");
        current_shader_param->set_param();
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glDrawBuffers(1, attachments);
        GL3D_GL()->glDisable(GL_DEPTH_TEST);
        GL3D_GL()->glDisable(GL_STENCIL_TEST);
        GL3D_GL()->glEnable(GL_BLEND);
        GL3D_GL()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//        glm::vec3 sunRay = one_scene->watcher->get_current_position() + one_scene->watcher->get_look_direction() - sunPos;
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shinePoint");
        GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(sunPos));

        glm::vec4 shineColor;
        // shine pot 1
        shineColor = glm::vec4(255.0f/255.0f, 225.0f/255.0f, 125.0f/255.0, 0.4f);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineColor");
        GL3D_GL()->glUniform4fv(location, 1, glm::value_ptr(shineColor));
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineSize");
        GL3D_GL()->glUniform1f(location, 0.3);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineFac");
        GL3D_GL()->glUniform1f(location, 0.72);
        one_scene->drawSpecialObject(rect, true);

        // shine pot 2
        shineColor = glm::vec4(255.0f/255.0f, 225.0f/255.0f, 175.0f/255.0, 0.4f);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineColor");
        GL3D_GL()->glUniform4fv(location, 1, glm::value_ptr(shineColor));
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineSize");
        GL3D_GL()->glUniform1f(location, 0.1);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineFac");
        GL3D_GL()->glUniform1f(location, 0.4);
        one_scene->drawSpecialObject(rect, true);

        // shine pot 3
        shineColor = glm::vec4(255.0f/255.0f, 225.0f/255.0f, 175.0f/255.0, 0.4f);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineColor");
        GL3D_GL()->glUniform4fv(location, 1, glm::value_ptr(shineColor));
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineSize");
        GL3D_GL()->glUniform1f(location, 0.1);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineFac");
        GL3D_GL()->glUniform1f(location, 0.1);
        one_scene->drawSpecialObject(rect, true);

        // shine pot 4
        shineColor = glm::vec4(125.0f/255.0f, 215.0f/255.0f, 125.0f/255.0, 0.4f);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineColor");
        GL3D_GL()->glUniform4fv(location, 1, glm::value_ptr(shineColor));
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineSize");
        GL3D_GL()->glUniform1f(location, 0.3);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineFac");
        GL3D_GL()->glUniform1f(location, -0.4);
        one_scene->drawSpecialObject(rect, true);

        // shine pot 5
        shineColor = glm::vec4(175.0f/255.0f, 215.0f/255.0f, 175.0f/255.0, 0.4f);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineColor");
        GL3D_GL()->glUniform4fv(location, 1, glm::value_ptr(shineColor));
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineSize");
        GL3D_GL()->glUniform1f(location, 0.5);
        location = GL3D_GL()->glGetUniformLocation(shineShader->getProgramID(), "shineFac");
        GL3D_GL()->glUniform1f(location, -1.0);
        one_scene->drawSpecialObject(rect, true);

        GL3D_GL()->glDisable(GL_BLEND);
        GL3D_GL()->glEnable(GL_DEPTH_TEST);
    }

    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    delete rect;

    QVector<string> cmd;
    cmd.clear();
    cmd.push_back(string("hdr_test"));
    this->canvas = gl3d::gl3d_post_process_set::shared_instance()->process(cmd,
                                                                           this->get_attached_scene(),
                                                                           this->canvas);
    // save g buffer to file
//    float * output_image = new float[gbufHeight * gbufWidth * 3];
//    unsigned char * image = new unsigned char [gbufHeight * gbufWidth * 4];
//    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gPosition);
//    GL3D_GL()->glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, output_image);
//    for (int i = 0; i < (gbufHeight * gbufWidth); i++) {
//        image[i * 4 + 0] = (unsigned char)(int)output_image[i * 3 + 0];
//        image[i * 4 + 1] = (unsigned char)(int)output_image[i * 3 + 1];
//        image[i * 4 + 2] = (unsigned char)(int)output_image[i * 3 + 2];
//        image[i * 4 + 3] = 0xff;
//    }
//    QImage img((uchar *)image, gbufWidth, gbufHeight, QImage::Format_RGBA8888);
//    img.save("test_pos.jpg");
//    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gNormal);
//    GL3D_GL()->glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, output_image);
//    for (int i = 0; i < (gbufHeight * gbufWidth); i++) {
//        image[i * 4 + 0] = (unsigned char)(int)output_image[i * 3 + 0];
//        image[i * 4 + 1] = (unsigned char)(int)output_image[i * 3 + 1];
//        image[i * 4 + 2] = (unsigned char)(int)output_image[i * 3 + 2];
//        image[i * 4 + 3] = 0xff;
//    }
//    QImage img2((uchar *)image, gbufWidth, gbufHeight, QImage::Format_RGBA8888);
//    img2.save("test_normal.jpg");
    // delete
//    delete posText;
//    delete normalText;
//    delete gColor;
}

void day::render() {
    this->rend_result();
}

void day::after_render() {
    test_flag_global = true;
    this->canvas = NULL;
}

void day::rend_shadow() {
    // 先绘制阴影贴图，给阴影贴图shader添加一下参数
    gl3d::scene *one_scene = this->get_attached_scene();
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void day::rend_main() {
    gl3d::scene *one_scene = this->get_attached_scene();
    // rend color
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("color");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->get_property()->global_shader = QString("color");
    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    GL3D_GL()->glDisable(GL_BLEND);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
    GL3D_GL()->glEnable(GL_STENCIL_TEST);
    GL3D_GL()->glStencilFunc(GL_ALWAYS, 1, 0xffff);
    GL3D_GL()->glClearStencil(0);
    GL3D_GL()->glClear(GL_STENCIL_BUFFER_BIT);
    GL3D_GL()->glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    one_scene->drawInstanced(false, 100);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void day::rend_result() {
    gl3d::scene *one_scene = this->get_attached_scene();
    special_obj *rect = new special_obj(this->canvas);

    rect->set_control_authority(GL3D_OBJ_ENABLE_DEL);
    rect->set_render_authority(GL3D_SCENE_DRAW_RESULT);

    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("post_process_result");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_RESULT;
    one_scene->get_property()->global_shader = QString("post_process_result");
    GL3D_GL()->glViewport(0, 0,
                          one_scene->get_width(), one_scene->get_height());
    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->drawSpecialObject(rect, true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    delete rect;
}

gl3d_material *day::build_material() {
    gl3d_material *ret = new gl3d_material(this->canvas);
    return ret;
}

class geo : public render_process {
public:
    void render() {
        gl3d::scene *one_scene = this->get_attached_scene();
        // 输入阴影贴图的参数，然后绘制主图像
        gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("geo");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        // 选择全局渲染器
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        GL3D_GL()->glDrawBuffers(1, attachments);
        one_scene->get_property()->global_shader = QString("geo");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL;
        one_scene->prepare_canvas(true);
        GL3D_GL()->glDisable(GL_CULL_FACE);
        GL3D_GL()->glDisable(GL_BLEND);
        GL3D_GL()->glDisable(GL_STENCIL_TEST);
        GL3D_GL()->glEnable(GL_DEPTH_TEST);
//    GL3D_GL()->glEnable(GL_BLEND);
//    GL3D_GL()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        one_scene->drawInstanced(false, 100);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

        // draw spaces
        one_scene->get_property()->global_shader = QString("lines");
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL;
        Program * lShader = GL3D_GET_SHADER("lines");
        GL3D_GL()->glUseProgram(lShader->getProgramID());
        GL3D_GL()->glBindVertexArray(one_scene->spaceManager->vao);
        GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, one_scene->spaceManager->vbo);
        auto location = GL3D_GL()->glGetAttribLocation(lShader->programID, "vertex_pos");
        GL3D_GL()->glEnableVertexAttribArray(one_scene->spaceManager->vao);
        GL3D_GL()->glEnableVertexAttribArray(location);
        GL3D_GL()->glVertexAttribPointer(location,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid*)0);
        location = GL3D_GL()->glGetUniformLocation(lShader->programID, "pvMatrix");
        glm::mat4 pvMat = one_scene->watcher->get_projection_matrix() * one_scene->watcher->get_viewing_matrix();
        GL3D_GL()->glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(pvMat));
        GL3D_GL()->glDrawArrays(GL_LINES, 0, one_scene->spaceManager->numVts);
        GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, 0);
        GL3D_GL()->glBindVertexArray(0);
    }
};
GL3D_ADD_RENDER_PROCESS(geo);

class night : public render_process {
public:
    void pre_render();

    void render();

    void after_render();

    // internel calls
    void rend_shadow();

    void rend_main();

    // result
    void rend_result();

    // set a material
    gl3d_material *build_material();

    // color canvas to present
    gl3d_general_texture *canvas;
};

GL3D_ADD_RENDER_PROCESS(night);

void night::pre_render() {
    this->rend_shadow();
    // render g-buffer
    int gbufWidth = 2 * (int)this->get_attached_scene()->get_width();
    int gbufHeight = 2 * (int)this->get_attached_scene()->get_height();

    gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                        gbufWidth, gbufHeight);
    GLuint gBuffer = fb.get_frame_obj();
    GLuint gPosition, gNormal, gColorSpec;

// - 颜色缓冲
    gl3d_general_texture * gColor = new gl3d::gl3d_general_texture(
            gl3d_general_texture::GL3D_RGBA,
            2.0 * this->get_attached_scene()->get_width(),
            2.0 * this->get_attached_scene()->get_height());
    fb.attach_color_text(gColor->get_text_obj());

    // 位置缓冲
    GL3D_GL()->glGenTextures(1, &gPosition);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gPosition);
    GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, gbufWidth, gbufHeight, 0, GL_RGB, GL_FLOAT, NULL);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT1,
                                      GL_TEXTURE_2D, gPosition, 0);

    // - 法线颜色缓冲
    GL3D_GL()->glGenTextures(1, &gNormal);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gNormal);
    GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, gbufWidth, gbufHeight, 0, GL_RGB, GL_FLOAT, NULL);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

//// - 漫反射 + Specular颜色缓冲
//    glGenTextures(1, &gAlbedoSpec);
//    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

// - 告诉OpenGL我们要使用哪个颜色附件来渲染
    fb.use_this_frame();
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    GL3D_GL()->glDrawBuffers(3, attachments);
    this->rend_main();

    // now render with light
    this->canvas = new gl3d::gl3d_general_texture(
            gl3d_general_texture::GL3D_RGBA,
            2.0 * this->get_attached_scene()->get_width(),
            2.0 * this->get_attached_scene()->get_height());
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0); // detach
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0); // detach
    GL3D_GL()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0); // detach
    fb.attach_color_text(this->canvas->get_text_obj());

    gl3d::scene *one_scene = this->get_attached_scene();
    // set g buffer and its texture
    Program * lightShader = GL3D_GET_SHADER("light");
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("light");
    GL3D_GL()->glUseProgram(lightShader->getProgramID());
    auto posText = new gl3d_general_texture(gPosition, gbufWidth, gbufHeight);
    auto normalText = new gl3d_general_texture(gNormal, gbufWidth, gbufHeight);
    special_obj *rect = new special_obj(gColor, posText, normalText);
    rect->buffer_data();
    // set shader
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->get_property()->global_shader = QString("light");
    current_shader_param->set_param();
    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    GL3D_GL()->glDrawBuffers(1, attachments);
//    GL3D_GL()->glEnable(GL_BLEND);
//    GL3D_GL()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    int i = 0;
    for (auto lightit : one_scene->get_light_srcs()->values()) {
        // set light position
        QString lightname = QString::asprintf("light[%d]", i);
        auto location = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), lightname.toStdString().c_str());
        glm::vec3 lpos = lightit->get_location();
        GL3D_GL()->glUniform3fv(location, 1, glm::value_ptr(lpos));
        i++;
        if (i >= gl3d_global_param::shared_instance()->maxInsPerDraw) {
//            i = 0;
            break;
        }
    }
    auto lnum = GL3D_GL()->glGetUniformLocation(lightShader->getProgramID(), "lightNum");
    GL3D_GL()->glUniform1i(lnum, i);
    if (i > 0)
        one_scene->drawSpecialObject(rect, true);
//        one_scene->drawObjectInstanced(lightShader->getProgramID(), true, i, rect);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    delete rect;

//    QVector<string> cmd;
//    cmd.clear();
//    cmd.push_back(string("hdr_test"));
//    this->canvas = gl3d::gl3d_post_process_set::shared_instance()->process(cmd,
//                                                                           this->get_attached_scene(),
//                                                                           this->canvas);
    // save g buffer to file
//    float * output_image = new float[gbufHeight * gbufWidth * 3];
//    unsigned char * image = new unsigned char [gbufHeight * gbufWidth * 4];
//    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gPosition);
//    GL3D_GL()->glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, output_image);
//    for (int i = 0; i < (gbufHeight * gbufWidth); i++) {
//        image[i * 4 + 0] = (unsigned char)(int)output_image[i * 3 + 0];
//        image[i * 4 + 1] = (unsigned char)(int)output_image[i * 3 + 1];
//        image[i * 4 + 2] = (unsigned char)(int)output_image[i * 3 + 2];
//        image[i * 4 + 3] = 0xff;
//    }
//    QImage img((uchar *)image, gbufWidth, gbufHeight, QImage::Format_RGBA8888);
//    img.save("test_pos.jpg");
//    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, gNormal);
//    GL3D_GL()->glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, output_image);
//    for (int i = 0; i < (gbufHeight * gbufWidth); i++) {
//        image[i * 4 + 0] = (unsigned char)(int)output_image[i * 3 + 0];
//        image[i * 4 + 1] = (unsigned char)(int)output_image[i * 3 + 1];
//        image[i * 4 + 2] = (unsigned char)(int)output_image[i * 3 + 2];
//        image[i * 4 + 3] = 0xff;
//    }
//    QImage img2((uchar *)image, gbufWidth, gbufHeight, QImage::Format_RGBA8888);
//    img2.save("test_normal.jpg");
    // delete
//    delete posText;
//    delete normalText;
//    delete gColor;
}

void night::render() {
    this->rend_result();
}

void night::after_render() {
    test_flag_global = true;
    this->canvas = NULL;
}

void night::rend_shadow() {
    // 先绘制阴影贴图，给阴影贴图shader添加一下参数
    gl3d::scene *one_scene = this->get_attached_scene();
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("shadow_mask");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->draw_shadow_mask();
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void night::rend_main() {
    gl3d::scene *one_scene = this->get_attached_scene();
    // rend color
    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("color");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_NORMAL;
    one_scene->get_property()->global_shader = QString("color");
    one_scene->prepare_canvas(false);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    GL3D_GL()->glDisable(GL_BLEND);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
    one_scene->drawInstanced(false, 100);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
}

void night::rend_result() {
    gl3d::scene *one_scene = this->get_attached_scene();
    special_obj *rect = new special_obj(this->canvas);

    rect->set_control_authority(GL3D_OBJ_ENABLE_DEL);
    rect->set_render_authority(GL3D_SCENE_DRAW_RESULT);

    gl3d::shader_param *current_shader_param = GL3D_GET_PARAM("post_process_result");
    current_shader_param->user_data.insert(string("scene"), one_scene);
    one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_RESULT;
    one_scene->get_property()->global_shader = QString("post_process_result");
    GL3D_GL()->glViewport(0, 0,
                          one_scene->get_width(), one_scene->get_height());
    one_scene->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    one_scene->drawSpecialObject(rect, true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));

    delete rect;
}

gl3d_material *night::build_material() {
    gl3d_material *ret = new gl3d_material(this->canvas);
    return ret;
}
