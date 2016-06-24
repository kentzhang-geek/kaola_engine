//
//  gl3d_general_texture.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/9.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d_general_texture.hpp"

void gl3d_general_texture::init() {
    memset(this, 0, sizeof(gl3d_general_texture));
}

gl3d_general_texture::gl3d_general_texture(gl3d_general_texture::texture_type set_type, GLuint x, GLuint y) {
    this->init();
    
    this->type = type;
    this->size_x = x;
    this->size_y = y;
    
    // gen texture
    glGenTextures(1, &this->text_obj);
    glBindTexture(GL_TEXTURE_2D, this->text_obj);
    
    // 设置贴图格式
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT,
                         x,
                         y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT,
                         NULL);
//            glTexStorage2DEXT(GL_TEXTURE_2D, 0, GL_UNSIGNED_INT_24_8_OES, x, y);
            break;
        case GL3D_RGBA:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         x,
                         y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
//            glTexStorage2DEXT(GL_TEXTURE_2D, 0, GL_RGBA8_OES, x, y);
            break;
        default:
            throw std::invalid_argument("invalid general texture type");
            break;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void gl3d_general_texture::clean_data() {
    glActiveTexture(GL_TEXTURE_2D);
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            glBindTexture(GL_TEXTURE_2D, this->text_obj);
            this->set_parami(false);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                         size_x,
                         size_y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT,
                         NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
        case GL3D_RGBA:
            glBindTexture(GL_TEXTURE_2D, this->text_obj);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         size_x,
                         size_y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
        default:
            throw std::invalid_argument("invalid general texture type");
            break;
    }
}

void gl3d_general_texture::bind(GLenum text_unit) {
    glActiveTexture(text_unit);
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            break;
        case GL3D_RGBA:
            break;
        default:
            throw std::invalid_argument("invalid general texture type");
            break;
    }
    glBindTexture(GL_TEXTURE_2D, this->text_obj);
}

void gl3d_general_texture::buffer_data(GLvoid *data) {
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT,
                         size_x,
                         size_y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT,
                         data);
            break;
        case GL3D_RGBA:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         size_x,
                         size_y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         data);
            break;
        default:
            throw std::invalid_argument("invalid general texture type");
            break;
    }
}

GLuint gl3d_general_texture::get_gl_obj() {
    return this->text_obj;
}

gl3d_general_texture::~gl3d_general_texture() {
    glDeleteTextures(1, &this->text_obj);
    
    return;
}

void gl3d_general_texture::set_parami(bool repeat) {
    /**
     *  @author Kent, 16-01-18 22:01:22
     *
     *  这几个参数相当重要，不设置会导致贴图失败，注意似乎有平铺贴图和拉伸贴图的区别!
     */
    if (repeat) {
        // 这里平铺贴图
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else {
        // 这里应该是拉伸贴图
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}
