//
//  gl3d_render_process.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_render_process_hpp
#define gl3d_render_process_hpp

#include <stdio.h>
#include <QVector>
#include <QMap>
#include "gl3d_out_headers.h"
#include "gl3d_viewer.h"
#include "gl3d_mesh.h"
#include "gl3d_material.hpp"
#include "utils/gl3d_scale.h"
#include "gl3d_object.h"
#include "gl3d_general_texture.hpp"
#include "gl3d_viewer.h"
#include "shader_manager.hpp"
#include "gl3d_scene.h"

using namespace std;

namespace gl3d {
    class render_process {
    public:
        render_process();
        ~render_process();
        virtual void pre_render() {};
        virtual void render() = 0;
        virtual void after_render() {};
        virtual void env_up() {};
        virtual void env_down() {};
        void * get_user_object(string key);
        void add_user_object(string key, void * obj);
        virtual void invoke() {};
        void attach_scene(gl3d::scene * sc);
        gl3d::scene * get_attached_scene();
    private:
        gl3d::scene * attached_scene;
        QMap<string, void *> user_objs;
    };
    
    class render_process_manager {
    public:
        static render_process_manager * get_shared_instance();
        void add_render_process(string key, render_process * render);
        render_process * get_render_process(string key);
        void set_current_process(string name);
        string get_current_process_name();
        void release_render();
        
    private:
        string current_process;
        QMap<string, render_process *> render_processes;
        render_process_manager();
        ~render_process_manager();
    };
    
    template <class T>
    class render_process_factory {
    public:
        render_process_factory(string name){
            render_process_manager * m = render_process_manager::get_shared_instance();
            m->add_render_process(name, new T);
        };
    };
}

#define GL3D_GET_RENDER_PROCESS(name) (gl3d::render_process_manager::get_shared_instance()->get_render_process(string(#name)))
#define GL3D_GET_CURRENT_RENDER_PROCESS() (gl3d::render_process_manager::get_shared_instance()->get_render_process((gl3d::render_process_manager::get_shared_instance()->get_current_process_name())))

#define GL3D_SET_CURRENT_RENDER_PROCESS(name, input_scene) \
    GL3D_GET_RENDER_PROCESS(name)->attach_scene(input_scene); \
    (gl3d::render_process_manager::get_shared_instance()->set_current_process(string(#name)));
#define GL3D_ADD_RENDER_PROCESS(name) const gl3d::render_process_factory<name> __tag_##render_process_##name(string(#name));

#endif /* gl3d_render_process_hpp */
