//
//  shader_manager.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/10.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef shader_manager_hpp
#define shader_manager_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "Program.hpp"
#include "Shader.hpp"

// 检测头文件的包含
#ifndef gl3d_hpp
//#error 在包含shader_manager之前，需要include gl3d.hpp
#endif

using namespace std;
using namespace gl3d;


// 这条宏由于给shader管理器加入相关shader加载所需的信息

#define GL3D_LOAD_SHADER(name, vertex, frag) const shader_loader __shader_loader_##name( \
string(#name), (char *)#vertex, (char *)#frag);

// 这条宏根据shader的名字去获取program
#define GL3D_GET_SHADER(name) (::gl3d::shader_manager::sharedInstance()->get_program(string(name)))

// 这条宏根据shader名字去获取参数
#define GL3D_GET_PARAM(name) (::gl3d::shader_manager::sharedInstance()->get_param(string(name)))

// 这条宏根据shader名字去获取其参数类

#define GL3D_SHADER_PARAM_CLASS_NAME(name) ::gl3d::__gl3d_shader_param_##name

// 这条宏根据shader名字定义其参数类名
#define GL3D_SHADER_PARAM_CLASS_DEFINE(name) \
class __gl3d_shader_param_##name : public ::gl3d::shader_param

/**
 *  @author Kent, 16-03-10 17:03:02
 *
 *  这条宏简单的去生成shader的参数设置函数头，注意param对象会默认存储当前渲染对象obj到内部user_data键object的值
 *
 *  @param name shader名字
 *
 *  @return 是否设置成功
 */
#define GL3D_SHADER_PARAM(name) \
namespace gl3d { \
class __gl3d_shader_param_##name : public shader_param { \
public: \
bool set_param(); \
__gl3d_shader_param_##name(char * s_name) : ::shader_param::shader_param(s_name) {} \
}; } \
const __gl3d_shader_param_##name GL3D_SHADER_PARAM_CLASS_NAME_##name##_instance = __gl3d_shader_param_##name((char *)#name); \
bool ::gl3d::__gl3d_shader_param_##name::set_param()

namespace gl3d {
    class shader_loader {
    public:
        string name;
        // use char * seems should be ok

        char * vertex;
        char * frag;
        shader_loader(string shaderName, char * vertexShader, char * fragShader);
    };
    
    // 这个类用于设置shader的参数
    class shader_param {
    public:
        string shader_name;
        map<string, void *> user_data;  // 用户自己的参数表
        shader_param() : shader_name(string("")) {}
        shader_param(char * name);
        ~shader_param();
        /**
         *  @author Kent, 16-03-10 16:03:34
         *
         *  实现这个方法以设置相应shader的参数(uniform)
         *
         *  @return 是否设置成功
         */
        virtual bool set_param() {return false;}
    };

    // 这个类用于管理shader以及其参数设置
    class shader_manager {
    private:
        shader_manager();
    public:
        static shader_manager * sharedInstance();
        map<string, gl3d::Program *> shaders;
        map<string, shader_loader *> loaders;
        map<string, shader_param *> params;
        gl3d::Program * get_program(string name);
        gl3d::shader_param * get_param(string name);
    };
}

// 工具宏
#define GL3D_GET_OBJ() (gl3d::object *)(this->user_data.at(string("object")))

#define GL3D_SET_MAT4(matrixName, matrixValue, program) \
glUniformMatrix4fv \
(glGetUniformLocation \
 (program, #matrixName), \
 1, GL_FALSE, glm::value_ptr(matrixValue));

#define GL3D_SET_VEC4(vecName, vecValue, program) \
glUniform4fv(glGetUniformLocation \
(program, #vecName), 1, glm::value_ptr(vecValue));

#define GL3D_SET_VEC3(vecName, vecValue, program) \
glUniform3fv(glGetUniformLocation \
(program, #vecName), 1, glm::value_ptr(vecValue));


#endif /* shader_manager_hpp */
