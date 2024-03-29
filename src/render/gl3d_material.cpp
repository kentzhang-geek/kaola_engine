//
//  gl3d_material.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/16.
//  Copyright © 2016年 Kent. All rights reserved.
//
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "utils/gl3d_path_config.h"

using namespace std;
using namespace gl3d;

extern std::string gl3d_sandbox_path;

gl3d_material::gl3d_material(aiMaterial * mtl, string base_path) {
    this->init();
    string path = base_path;
    path = path + string(GL3D_PATH_SEPRATOR);
    string text_file_name;
    aiString text_path;

    // 加载3种类型的贴图
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->type_to_image.insert(diffuse, new gl3d_image((char *)text_file_name.c_str()));
    }
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_AMBIENT, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->type_to_image.insert(ambient, new gl3d_image((char *)text_file_name.c_str()));
    }
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->type_to_image.insert(specular, new gl3d_image((char *)text_file_name.c_str()));
    }
    
    // 加载3种类型的颜色
    aiColor3D ai_clr;
    glm::vec3 tmp_clr;
    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, ai_clr)) {
        tmp_clr = glm::vec3(ai_clr.r, ai_clr.g, ai_clr.b);
        this->colors.insert(diffuse, tmp_clr);
    }
    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, ai_clr)) {
        tmp_clr = glm::vec3(ai_clr.r, ai_clr.g, ai_clr.b);
        this->colors.insert(ambient, tmp_clr);
    }
    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, ai_clr)) {
        tmp_clr = glm::vec3(ai_clr.r, ai_clr.g, ai_clr.b);
        this->colors.insert(specular, tmp_clr);
    }
    
    // 设置empty标志
    if ((this->colors.size() == 0) && (this->textures.size() == 0)) {
        this->is_empty = true;
        log_c("this mtl is empty!");
    }
}

gl3d_material::~gl3d_material() {
    auto iter = this->textures.begin();
    for (; iter != this->textures.end(); iter++) {
        delete iter.value();
    }
    for (auto it = this->type_to_image.begin();
            it != this->type_to_image.end();
            it++) {
        delete it.value();
    }
    return;
}

void gl3d_material::use_this(GLuint pro) {
    if (this->is_empty) {
        return;
    }
    this->buffer_data();
    
    // 初始化贴图单元使能够
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_ambient_enable"), 0);
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_diffuse_enable"), 0);
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_specular_enable"), 0);
    if (this->textures.size() != 0) {
        // 绑定texture
        for (auto iter_t = this->textures.begin(); iter_t != this->textures.end(); iter_t++) {
            switch (iter_t.key()) {
                case ambient :
                    // 原色贴图在 0 号单元
                    iter_t.value()->bind(GL_TEXTURE0);
                    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_ambient_enable"), 1);
                    break;
                case diffuse :
                    // 散射贴图在 1 号单元
                    iter_t.value()->bind(GL_TEXTURE1);
                    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_diffuse_enable"), 1);
                    break;
                case specular :
                    // 镜面反射贴图在 2 号单元
                    iter_t.value()->bind(GL_TEXTURE2);
                    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_t_specular_enable"), 1);
                    break;
                default:
                    log_c("Load texture type what failed");
                    break;
            }
        }
    }
    
    // 初始化3个颜色
    GL3D_SET_VEC4(mtlAmbientColor, glm::vec4(0.2), pro);
    GL3D_SET_VEC4(mtlDiffuseColor, glm::vec4(0.8), pro);
    GL3D_SET_VEC4(mtlSpecularColor, glm::vec4(1.0), pro);
    if (this->colors.size() != 0) {
        for (auto iter_c = this->colors.begin();  iter_c != this->colors.end(); iter_c++) {
            switch (iter_c.key()) {
                case ambient :
                    // 原色设置
                    GL3D_SET_VEC4(mtlAmbientColor, glm::vec4(iter_c.value(), 1.0), pro);
                    break;
                case diffuse :
                    // 散射色设置
                    GL3D_SET_VEC4(mtlDiffuseColor, glm::vec4(iter_c.value(), 1.0), pro);
                    break;
                case specular :
                    // 镜面颜色设置
                    GL3D_SET_VEC4(mtlSpecularColor, glm::vec4(iter_c.value(), 1.0), pro);
                    break;
                default:
                    log_c("Load color type what failed");
                    break;
            }
        }
    }
}

void gl3d_material::init() {
    this->colors.clear();
    this->textures.clear();
    this->type_to_image.clear();
    this->data_buffered = false;
    this->is_empty = false;
}

void gl3d_material::buffer_data() {
    if (this->data_buffered) {
        return;
    }
    for (auto it = this->type_to_image.begin();
            it != this->type_to_image.end();
            it++) {
        this->textures.insert(it.key(), new gl3d_texture(it.value()));
    }
    this->type_to_image.clear();
    this->data_buffered = true;
}

// 特殊用途的材质贴图
gl3d_material::gl3d_material(gl3d_general_texture * gtext) {
    this->init();

    this->textures.insert(diffuse, gtext);
    this->colors.insert(ambient, glm::vec3(0.0));
    this->colors.insert(diffuse, glm::vec3(0.0));
    this->colors.insert(specular, glm::vec3(0.0));
}

gl3d_material::gl3d_material() {
    this->init();
    this->is_empty = true;
}

gl3d_material::gl3d_material(string file_name) {
    this->init();
    string path = file_name;

    // KENT WARN : 如果两个text的index相同，则不进行覆盖
    this->type_to_image.insert(diffuse, new gl3d_image((char *)file_name.c_str()));
    // KENT WARN : 目前只用散射色
//    this->textures.insert(std::pair<material_type, gl3d_texture *>(ambient, ins_text));
//    this->textures.insert(std::pair<material_type, gl3d_texture *>(specular, ins_text));
    
    // KENT WARN : 设置初始颜色为0.3
    this->colors.insert(ambient, glm::vec3(0.3));
    this->colors.insert(diffuse, glm::vec3(0.0));
    this->colors.insert(specular, glm::vec3(0.0));
}

gl3d_material* gl3d_material::from_color(glm::vec3 rgb, int alpha) {
    gl3d_material * mat = new gl3d_material();

    mat->colors.clear();
    mat->colors.insert(ambient, rgb * 0.3f);
    mat->colors.insert(diffuse, rgb);
    mat->colors.insert(ambient, rgb);

    mat->is_empty = false;

    return mat;
}