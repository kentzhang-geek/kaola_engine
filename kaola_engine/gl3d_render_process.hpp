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
#include <vector>
#include "gl3d_out_headers.h"

using namespace std;

namespace gl3d {
    class render_process {
    public:
        render_process();
        ~render_process();
        virtual void pre_render() {};
        virtual void render() = 0;
        virtual void after_render() {};
        void * get_user_object(string key);
        void add_user_object(string key, void * obj);
        virtual void invoke() {};
    private:
        map<string, void *> user_objs;
    };
    
    class render_process_manager {
    public:
        static render_process_manager * get_shared_instance();
        void add_render_process(string key, render_process * render);
        render_process * get_render_process(string key);
        void set_current_process(string name);
        string get_current_process_name();
        
    private:
        string current_process;
        map<string, render_process *> render_processes;
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
#define GL3D_ADD_RENDER_PROCESS(name) const gl3d::render_process_factory<name> __tag_##render_process_##name(string(#name));
#define GL3D_SET_CURRENT_RENDER_PROCESS(name) (gl3d::render_process_manager::get_shared_instance()->set_current_process(string(#name)));
#define GL3D_GET_CURRENT_RENDER_PROCESS() (gl3d::render_process_manager::get_shared_instance()->get_render_process((gl3d::render_process_manager::get_shared_instance()->get_current_process_name())))


#endif /* gl3d_render_process_hpp */
