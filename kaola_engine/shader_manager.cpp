//
//  shader_manager.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/10.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d.hpp"
#include "shader_manager.hpp"

using namespace gl3d;
using namespace std;

static shader_manager * __global_shader_manager = NULL;

/**
 *  @author Kent, 16-03-10 16:03:38
 *
 *  记录shader的名字和路径到shader管理器
 */
shader_loader::shader_loader(string shaderName, char * vertexShader, char * fragShader) {
    this->vertex = vertexShader;
    this->frag = fragShader;
    this->name = shaderName;
    ::shader_manager::sharedInstance()->loaders.insert(std::pair<string, shader_loader *>(shaderName, this));
}

shader_manager::shader_manager() {
    this->shaders.clear();
    this->loaders.clear();
    this->params.clear();
    
    __global_shader_manager = this;
}

shader_param::shader_param(char * name) : shader_name(string("")) {
    this->shader_name = string(name);
    ::shader_manager::sharedInstance()->params.insert(pair<string, shader_param *>(string(name), this));
}

shader_param::~shader_param() {
    ::shader_manager::sharedInstance()->params.erase(this->shader_name);
}

// single instance mod
shader_manager * shader_manager::sharedInstance() {
    if (NULL == __global_shader_manager) {
        __global_shader_manager = new shader_manager();
    }
    
    return __global_shader_manager;
}

gl3d::Program * shader_manager::get_program(string name) {
    try {
        return this->shaders.at(name);
    } catch (std::out_of_range & oe) {
        log_c("Does Not Found Shader Program %s", name.c_str());
        return NULL;
    }
}

gl3d::shader_param * shader_manager::get_param(string name) {
    try {
        return this->params.at(name);
    } catch (std::out_of_range & oe) {
        log_c("Does Not Found Shader Parameter %s", name.c_str());
        return NULL;
    }
}
