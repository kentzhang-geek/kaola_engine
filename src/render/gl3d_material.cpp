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

using namespace std;
using namespace gl3d;

extern std::string gl3d_sandbox_path;

gl3d_material::gl3d_material(aiMaterial * mtl) {
    this->init();
    string path = gl3d_sandbox_path;
    path = path + string("\\");
    string text_file_name;
    aiString text_path;
    gl3d_texture * ins_text;
    
    // 加载3种类型的贴图
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        ins_text = new gl3d_texture((char *)text_file_name.c_str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->textures.insert(diffuse, ins_text);
        //            log_c("OK load texture index is %d, file name is %s", i, text_file_name.c_str());
    }
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_AMBIENT, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        ins_text = new gl3d_texture((char *)text_file_name.c_str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->textures.insert(ambient, ins_text);
        //            log_c("OK load texture index is %d, file name is %s", i, text_file_name.c_str());
    }
    text_file_name.clear();
    text_path.Clear();
    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &text_path)) {
        // combine absolute file path
        text_file_name = path + string(text_path.C_Str());
        ins_text = new gl3d_texture((char *)text_file_name.c_str());
        // KENT WARN : 如果两个text的index相同，则不进行覆盖
        this->textures.insert(specular, ins_text);
        //            log_c("OK load texture index is %d, file name is %s", i, text_file_name.c_str());
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
    return;
}

void gl3d_material::use_this(GLuint pro) {
    if (this->is_empty) {
        return;
    }
    
    // 初始化贴图单元使能够
    glUniform1i(glGetUniformLocation(pro, "gl3d_t_ambient_enable"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_t_diffuse_enable"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_t_specular_enable"), 0);
    if (this->textures.size() != 0) {
        // 绑定texture
        for (auto iter_t = this->textures.begin(); iter_t != this->textures.end(); iter_t++) {
            switch (iter_t.key()) {
                case ambient :
                    // 原色贴图在 0 号单元
                    iter_t.value()->bind(GL_TEXTURE0);
                    glUniform1i(glGetUniformLocation(pro, "gl3d_t_ambient_enable"), 1);
                    break;
                case diffuse :
                    // 散射贴图在 1 号单元
                    iter_t.value()->bind(GL_TEXTURE1);
                    glUniform1i(glGetUniformLocation(pro, "gl3d_t_diffuse_enable"), 1);
                    break;
                case specular :
                    // 镜面反射贴图在 2 号单元
                    iter_t.value()->bind(GL_TEXTURE2);
                    glUniform1i(glGetUniformLocation(pro, "gl3d_t_specular_enable"), 1);
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
    this->is_empty = false;
}

// 特殊用途的材质贴图
gl3d_material::gl3d_material(gl3d_general_texture * gtext) {
    this->init();

    this->textures.insert(diffuse, gtext);
    this->colors.insert(ambient, glm::vec3(0.0));
    this->colors.insert(diffuse, glm::vec3(0.0));
    this->colors.insert(specular, glm::vec3(0.0));
}

gl3d_material::gl3d_material(string file_name) {
    this->init();
    string path = gl3d_sandbox_path;
    path = path + string("/") + file_name;
    gl3d_texture * ins_text;
    
    ins_text = new gl3d_texture((char *)path.c_str());
    // KENT WARN : 如果两个text的index相同，则不进行覆盖
    this->textures.insert(diffuse, ins_text);
    // KENT WARN : 目前只用散射色
//    this->textures.insert(std::pair<material_type, gl3d_texture *>(ambient, ins_text));
//    this->textures.insert(std::pair<material_type, gl3d_texture *>(specular, ins_text));
    
    // KENT WARN : 设置初始颜色为0.2
    this->colors.insert(ambient, glm::vec3(0.3));
    this->colors.insert(diffuse, glm::vec3(0.0));
    this->colors.insert(specular, glm::vec3(0.0));
}
