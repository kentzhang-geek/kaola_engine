//
//  default_shader_param.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/10.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>

#include "kaola_engine/gl3d.hpp"
#include "utils/gl3d_global_param.h"

// test param configuration
using namespace gl3d;
using namespace std;

GL3D_SHADER_PARAM(default) {
    return true;
}

GL3D_SHADER_PARAM(vector_light) {
    gl3d::object * obj = GL3D_GET_OBJ();
    GLuint pro = GL3D_GET_SHADER("vector_light")->getProgramID();
    ::glm::vec4 tmp = glm::vec4(0.0, 0.2, 0.3, 1.0);
    GL3D_SET_VEC4(mtlAmbientColor, tmp, pro);
    tmp = glm::vec4(0.0, 0.4, 0.6, 1.0);
    GL3D_SET_VEC4(mtlDiffuseColor, tmp, pro);
    tmp = glm::vec4(0.2);
    GL3D_SET_VEC4(mtlSpecularColor, tmp, pro);
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 1);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.02);
    glm::vec3 light = glm::vec3(0.0, 0.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);

    return false;
}

GL3D_SHADER_PARAM(vector_light_toon) {
    gl3d::object * obj = GL3D_GET_OBJ();
    GLuint pro = GL3D_GET_SHADER("vector_light")->getProgramID();
    ::glm::vec4 tmp = glm::vec4(0.0, 0.2, 0.3, 1.0);
    GL3D_SET_VEC4(mtlAmbientColor, tmp, pro);
    tmp = glm::vec4(0.0, 0.4, 0.6, 1.0);
    GL3D_SET_VEC4(mtlDiffuseColor, tmp, pro);
    tmp = glm::vec4(0.2);
    GL3D_SET_VEC4(mtlSpecularColor, tmp, pro);
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 1);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.02);
    glm::vec3 light = glm::vec3(0.0, 0.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    return false;
}


GL3D_SHADER_PARAM(multiple_text) {
    GLuint pro = GL3D_GET_SHADER("multiple_text")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    return true;
}

GL3D_SHADER_PARAM(multiple_text_vector) {
    GLuint pro = GL3D_GET_SHADER("multiple_text_vector")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 设置材质贴图矩阵
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 1.0);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    // enable shadow test
    glUniform1i(glGetUniformLocation(pro, "shadow_enable"), (obj->get_property()->draw_authority&GL3D_SCENE_DRAW_SHADOW) && GL3D_SCENE_DRAW_SHADOW);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    // 计算model matrix
    ::glm::mat4 trans = glm::mat4(1.0);
    GLfloat s_range = 0.0;
    s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    trans = ::glm::scale(trans, glm::vec3(s_range));
    
    // 计算阴影中心位置
    glm::vec3 shadow_center = *((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_position();
    shadow_center += 20.0f * glm::normalize(*((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_lookat());
    shadow_center.y = 0.0;
    
    glm::vec3 lightInvDir = glm::vec3(0, 15.0f, -15.0f);
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-30.0,30.0,-30.0,30.0,-15.0, 500.0);
    //    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0.0), glm::vec3(0, 0, 1.0));
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir + shadow_center, shadow_center, glm::vec3(0, 1.0, 0));
    glm::mat4 depthModelMatrix = trans;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glGetUniformLocation(pro, "s_mtx"), 1, GL_FALSE, ::glm::value_ptr(depthMVP));
    
    // 检查是否固定alpha值
    if (!(obj->get_property()->draw_authority & GL3D_SCENE_DRAW_GROUND)) {
        glUniform1f(glGetUniformLocation(pro, "alpha"), 1.0);
        glDepthMask(GL_TRUE);
    }
    else {
        float alpha = *(float *)obj->user_data.value(string("alpha"));
        glUniform1f(glGetUniformLocation(pro, "alpha"), alpha);
        glDepthMask(GL_FALSE);
    }
    glCullFace(GL_BACK);  // 显示正面不显示背面
    
    GL3D_SET_VEC3(shadow_center_input, shadow_center, pro);
    GL3D_SET_MAT4(transmtx, trans, pro);
    
    return true;
}

GL3D_SHADER_PARAM(picking_mask) {
    static bool dispath_once = true;
    static GLuint text_obj = 0;
    GLuint pro = GL3D_GET_SHADER("picking_mask")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 只有coding map在3号单元上
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_picking_mask"), 5);
    
    GLuint id = obj->get_property()->id;
    GLubyte color[4];
    color[0] = id;
    color[1] = id>>8;
    color[2] = id>>16;
    color[3] = 0xff;
        
    if (dispath_once) {
        glGenTextures(1, &text_obj);
        dispath_once = false;
    }
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, text_obj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 1,
                 1,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 (GLvoid *) color);
    
    return true;
}

GL3D_SHADER_PARAM(shadow_mask) {
    GLuint pro = GL3D_GET_SHADER("shadow_mask")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 计算model matrix
    ::glm::mat4 trans;
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    GLfloat s_range = 0.0;
    s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::scale(trans, glm::vec3(s_range));
    
    // 计算阴影中心位置
    glm::vec3 shadow_center = *((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_position();
    shadow_center += 20.0f * glm::normalize(*((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_lookat());
    shadow_center.y = 0.0;

    glm::vec3 lightInvDir = glm::vec3(0, 15.0f, -15.0f);
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-30.0,30.0,-30.0,30.0,-15.0, 500.0);
    //    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0.0), glm::vec3(0, 0, 1.0));
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir + shadow_center, shadow_center, glm::vec3(0, 1.0, 0));
    glm::mat4 depthModelMatrix = trans;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glGetUniformLocation(pro, "s_mtx"), 1, GL_FALSE, ::glm::value_ptr(depthMVP));

    return true;
}

GL3D_SHADER_PARAM(multiple_text_vector_shadow) {
    GLuint pro = GL3D_GET_SHADER("multiple_text_vector_shadow")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 设置材质贴图矩阵
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 1.0);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    // enable shadow test
    glUniform1i(glGetUniformLocation(pro, "shadow_enable"), (obj->get_property()->draw_authority&GL3D_SCENE_DRAW_SHADOW) && GL3D_SCENE_DRAW_SHADOW);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    // 计算model matrix
    ::glm::mat4 trans = glm::mat4(1.0);
    GLfloat s_range = 0.0;
    s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    trans = ::glm::scale(trans, glm::vec3(s_range));
    
    // 计算阴影中心位置
    glm::vec3 shadow_center = *((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_position();
    shadow_center += 20.0f * glm::normalize(*((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_lookat());
    shadow_center.y = 0.0;

    glm::vec3 lightInvDir = glm::vec3(0, 15.0f, -15.0f);
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-30.0,30.0,-30.0,30.0,-15.0, 500.0);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir + shadow_center, shadow_center, glm::vec3(0, 1.0, 0));
    glm::mat4 depthModelMatrix = trans;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glGetUniformLocation(pro, "s_mtx"), 1, GL_FALSE, ::glm::value_ptr(depthMVP));

    // 拿到阴影贴图，并且把阴影贴图绑定到采样器7上
    gl3d_general_texture * shadow_txt = ((gl3d::scene *)this->user_data.value(string("scene")))->get_shadow_texture();
    shadow_txt->bind(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_shadow"), 4);
    
    // 阴影贴图大小
    glm::vec2 textsize = glm::vec2(2048.0);
    glUniform2fv(glGetUniformLocation(pro, "shadow_map_size"), 1, glm::value_ptr(textsize));
    
    // 检查是否固定alpha值
    if (!(obj->get_property()->draw_authority & GL3D_SCENE_DRAW_GROUND)) {
        glUniform1f(glGetUniformLocation(pro, "alpha"), 1.0);
        glDepthMask(GL_TRUE);
    }
    else {
        float alpha = *(float *)obj->user_data.value(string("alpha"));
        glUniform1f(glGetUniformLocation(pro, "alpha"), alpha);
        glDepthMask(GL_FALSE);
    }
    glCullFace(GL_BACK);  // 显示正面不显示背面

    GL3D_SET_VEC3(shadow_center_input, shadow_center, pro);
    GL3D_SET_MAT4(transmtx, trans, pro);
    
    return true;
}

GL3D_SHADER_PARAM(image) {
    GLuint pro = GL3D_GET_SHADER("image")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    gl3d::scene * scene = (gl3d::scene *)this->user_data.value(string("scene"));
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    ::glm::mat4 pvm = scene->watcher->projection_matrix;
    glm::vec3 currentpos = *scene->watcher->get_position();
    glm::vec3 lookat = *scene->watcher->get_lookat();
    glm::vec3 headto = *scene->watcher->get_headto();
    pvm = pvm * ::glm::lookAt(currentpos, currentpos + lookat, headto);

    // set model matrix
    ::glm::mat4 trans;
    // set norMtx
    ::glm::mat4 norMtx;
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    // KENT TODO : 这里似乎加上View MTX之后就会变得像手电筒一样
    //    norMtx = this->watcher->viewing_matrix * trans;
    norMtx = trans;	
    trans = ::glm::scale(trans, glm::vec3(s_range));
    // KENT WARN : 这里要注意是z轴向上还是y轴向上
    trans = ::glm::scale(trans, glm::vec3(1.0, -1.0, 1.0));
    pvm *= trans;    // final MVP
    
    GL3D_SET_MAT4(pvmImageMatrix, pvm, pro);
    glCullFace(GL_FRONT);  // 显示背面不显示正面（由于y轴反转）
    
    return true;
}

GL3D_SHADER_PARAM(skybox) {
    GLuint pro = GL3D_GET_SHADER("skybox")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    return true;
}


GL3D_SHADER_PARAM(dm) {
    GLuint pro = GL3D_GET_SHADER("dm")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 设置材质贴图矩阵
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.6);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    // 计算model matrix
    ::glm::mat4 trans = glm::mat4(1.0);
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    trans = ::glm::scale(trans, glm::vec3(s_range));
    
    // 计算阴影中心位置
    glm::vec3 shadow_center = *((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_position();
    shadow_center += 20.0f * glm::normalize(*((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_lookat());
    shadow_center.y = 0.0;

    glm::vec3 lightInvDir = glm::vec3(0, 15.0f, -15.0f);
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-30.0,30.0,-30.0,30.0,-15.0, 500.0);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir + shadow_center, shadow_center, glm::vec3(0, 1.0, 0));
    glm::mat4 depthModelMatrix = trans;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glGetUniformLocation(pro, "s_mtx"), 1, GL_FALSE, ::glm::value_ptr(depthMVP));
    
    // 拿到阴影贴图，并且把阴影贴图绑定到采样器7上
    gl3d_general_texture * shadow_txt = ((gl3d::scene *)this->user_data.value(string("scene")))->get_shadow_texture();
    shadow_txt->bind(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_shadow"), 4);
    
    // 阴影贴图大小
    glm::vec2 textsize = glm::vec2(2048.0);
    glUniform2fv(glGetUniformLocation(pro, "shadow_map_size"), 1, glm::value_ptr(textsize));
    
    // 检查是否固定alpha值
    if (!(obj->get_property()->draw_authority & GL3D_SCENE_DRAW_GROUND)) {
        glUniform1f(glGetUniformLocation(pro, "alpha"), 1.0);
        glDepthMask(GL_TRUE);
    }
    else {
        float alpha = *(float *)obj->user_data.value(string("alpha"));
        glUniform1f(glGetUniformLocation(pro, "alpha"), alpha);
        glDepthMask(GL_FALSE);
    }
    glCullFace(GL_BACK);  // 显示正面不显示背面
    
    GL3D_SET_VEC3(shadow_center_input, shadow_center, pro);
    GL3D_SET_MAT4(transmtx, trans, pro);
    
    return true;
}

GL3D_SHADER_PARAM(dm2) {
    GLuint pro = GL3D_GET_SHADER("dm2")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    
    // 设置材质贴图矩阵
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.6);
    
    // 原色，散射与镜面分别是0，1，2
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    
    // 计算model matrix
    ::glm::mat4 trans = glm::mat4(1.0);
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    trans = ::glm::scale(trans, glm::vec3(s_range));
    
    // 计算阴影中心位置
    glm::vec3 shadow_center = *((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_position();
    shadow_center += 20.0f * glm::normalize(*((gl3d::scene *)this->user_data.value(string("scene")))->watcher->get_lookat());
    shadow_center.y = 0.0;

    glm::vec3 lightInvDir = glm::vec3(0, 15.0f, -15.0f);
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-30.0,30.0,-30.0,30.0,-15.0, 500.0);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir + shadow_center, shadow_center, glm::vec3(0, 1.0, 0));
    glm::mat4 depthModelMatrix = trans;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glGetUniformLocation(pro, "s_mtx"), 1, GL_FALSE, ::glm::value_ptr(depthMVP));
    
    // 检查是否固定alpha值
    if (!(obj->get_property()->draw_authority & GL3D_SCENE_DRAW_GROUND)) {
        glUniform1f(glGetUniformLocation(pro, "alpha"), 1.0);
        glDepthMask(GL_TRUE);
    }
    else {
        float alpha = *(float *)obj->user_data.value(string("alpha"));
        glUniform1f(glGetUniformLocation(pro, "alpha"), alpha);
        glDepthMask(GL_FALSE);
    }
    glCullFace(GL_BACK);  // 显示正面不显示背面
    
    GL3D_SET_VEC3(shadow_center_input, shadow_center, pro);
    GL3D_SET_MAT4(transmtx, trans, pro);
    
    return true;
}
