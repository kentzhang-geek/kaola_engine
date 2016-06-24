//
//  model_manager.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/27.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef model_manager_hpp
#define model_manager_hpp

#include "gl3d.hpp"
#include <vector>
#include <string>

using namespace std;

namespace gl3d {
    class model_param {
    public:
        string file_name;
        int id;
        gl3d::object * obj;
        virtual void set_param();
        model_param(int i, string fn);
        model_param();
    };
    
    class model_manager {
    public:
        static model_manager * shared_instance();
        void init_objs(gl3d::scene * main_scene);
        void release_loaders();
        void add_obj_to_scene(gl3d::scene * scene, char * obj_filename, glm::vec2 coord);
        vector<model_param *> models;
    private:
        int available_id;
        model_manager();
    };
}

#define GL3D_MODEL_PARAM(id, name) \
namespace gl3d { \
class __gl3d_model_param_##id : public model_param { \
public: \
void set_param(); \
__gl3d_model_param_##id() : ::model_param::model_param(id, string(name)) {} \
}; } \
const __gl3d_model_param_##id * GL3D_MODEL_PARAM_CLASS_NAME_##id##_instance = new __gl3d_model_param_##id(); \
void ::gl3d::__gl3d_model_param_##id::set_param()

#define GL3d_MODEL_PARAM_DEFAULT() \
 ::model_param::set_param();

#endif /* model_manager_hpp */
