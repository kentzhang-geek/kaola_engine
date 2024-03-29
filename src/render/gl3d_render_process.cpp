//
//  gl3d_render_process.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d_render_process.hpp"

using namespace gl3d;

render_process::render_process() {
    this->user_objs.clear();
    this->attached_scene = NULL;
}

render_process::~render_process() {
    this->user_objs.clear();
}

void * render_process::get_user_object(string key) {
    return user_objs.value(key);
}

void render_process::add_user_object(string key, void *obj) {
    this->user_objs.insert(key, obj);
    return;
}

void render_process::attach_scene(scene *s) {
    this->attached_scene = s;
}

gl3d::scene * render_process::get_attached_scene() {
    return this->attached_scene;
}

//template <typename T>
//render_process_factory<T>::render_process_factory(string name) {
//    T * render = new T;
//    render_process_manager * m = render_process_manager::get_shared_instance();
//    m->add_render_process(name, render);
//}

render_process_manager::render_process_manager() {
    this->render_processes.clear();
}

static render_process_manager * one_instance;
render_process_manager * render_process_manager::get_shared_instance(){
    if (one_instance == NULL) {
        one_instance = new render_process_manager;
    }
    
    return one_instance;
}

void render_process_manager::add_render_process(string key, gl3d::render_process *render) {
    this->render_processes.insert(key, render);
    return;
}

render_process * render_process_manager::get_render_process(string key) {
    return this->render_processes.value(key);
}

string render_process_manager::get_current_process_name() {
    return current_process;
}

void render_process_manager::set_current_process(string name) {
    if (this->current_process.length() > 0) {
        // tear down last render process
        this->render_processes.value(this->current_process)->env_down();
    }
    this->current_process = name;
    // up new render process
    this->render_processes.value(this->current_process)->env_up();
}

void render_process_manager::release_render() {
    if (this->current_process.length() > 0) {
        // tear down last render process
        this->render_processes.value(this->current_process)->env_down();
    }
    this->current_process.clear();
}
