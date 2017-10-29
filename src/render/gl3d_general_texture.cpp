//
//  gl3d_general_texture.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/9.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d_general_texture.hpp"

void gl3d_general_texture::init() {
}

gl3d_general_texture::gl3d_general_texture(GLuint obj, GLuint width, GLuint height) {
    this->init();
    this->set_text_obj(obj);
    this->set_size_x(width);
    this->set_size_y(height);
    this->type = GL3D_DATA;
}

gl3d_general_texture::gl3d_general_texture(gl3d_general_texture::texture_type set_type, GLuint x, GLuint y) {
    this->init();
    
    this->type = set_type;
    this->size_x = x;
    this->size_y = y;
    
    // gen texture
    GL3D_GL()->glGenTextures(1, &this->text_obj);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, this->text_obj);
    
    // 设置贴图格式
    switch (set_type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT,
                         x,
                         y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_BYTE,
                         NULL);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
//            glTexStorage2DEXT(GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE_24_8_OES, x, y);
            break;
        case GL3D_RGBA:
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D,
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
//            throw std::invalid_argument("invalid general texture type");
            break;
    }
    
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, 0);
}

void gl3d_general_texture::clean_data() {
    GL3D_GL()->glActiveTexture(GL_TEXTURE_2D);
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            GL3D_GL()->glBindTexture(GL_TEXTURE_2D, this->text_obj);
            this->set_parami(false);
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_DEPTH_COMPONENT,
                         size_x,
                         size_y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_BYTE,
                         NULL);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
            GL3D_GL()->glBindTexture(GL_TEXTURE_2D, 0);
            break;
        case GL3D_RGBA:
            GL3D_GL()->glBindTexture(GL_TEXTURE_2D, this->text_obj);
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         size_x,
                         size_y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
            GL3D_GL()->glBindTexture(GL_TEXTURE_2D, 0);
            break;
        default:
//            throw std::invalid_argument("invalid general texture type");
            break;
    }
}

void gl3d_general_texture::bind(GLenum text_unit) {
    GL3D_GL()->glActiveTexture(text_unit);
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, this->text_obj);
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
            break;
        case GL3D_RGBA:
            this->set_parami(true);
            break;
        default:
//            throw std::invalid_argument("invalid general texture type");
            break;
    }

}

void gl3d_general_texture::buffer_data(GLvoid *data) {
    GL3D_GL()->glBindTexture(GL_TEXTURE_2D, this->text_obj);
    switch (type) {
        case GL3D_DEPTH_COMPONENT:
            this->set_parami(false);
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT,
                         size_x,
                         size_y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_BYTE,
                         data);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
            break;
        case GL3D_RGBA:
            GL3D_GL()->glTexImage2D(GL_TEXTURE_2D,
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
//            throw std::invalid_argument("invalid general texture type");
            break;
    }
}

gl3d_general_texture::~gl3d_general_texture() {
    GL3D_GL()->glDeleteTextures(1, &this->text_obj);
    
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
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else {
        // 这里应该是拉伸贴图
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 各向异性
    GLfloat fLargest;
    GL3D_GL()->glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    if (1.0 < fLargest) {
//        fLargest = 1.0 + (fLargest - 1.0) * 0.6;
        GL3D_GL()->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
    }
}
