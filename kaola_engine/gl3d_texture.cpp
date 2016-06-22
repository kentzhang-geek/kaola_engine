//
//  gl3d_texture.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d_texture.hpp"
#include "gl3d_image.hpp"

using namespace std;
using namespace gl3d;

void gl3d_texture::init() {
    bzero(this, sizeof(gl3d_texture));
}

void gl3d_texture::set_parami(bool repeat) {
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

gl3d_texture::gl3d_texture(char * filename) {
    this->init();
    
    // create image
    this->img = new gl3d_image(filename);
    this->texture_name = string(filename);

    // buffer texture
    glGenTextures(1, &this->text_obj);
    glBindTexture(GL_TEXTURE_2D, this->text_obj);
//    this->set_parami(false);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 this->img->width,
                 this->img->height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 this->img->data);
    
    // 贴图缓存好之后就删除通用内存中的图片数据
    delete this->img;
    this->img = NULL;
}

// 绑定texture到对应的单元
void gl3d_texture::bind(GLenum text_unit) {
    glActiveTexture(text_unit);
    glBindTexture(GL_TEXTURE_2D, this->text_obj);
}

gl3d_texture::~gl3d_texture() {
    glDeleteTextures(1, &this->text_obj);
    delete this->img;
    
    return;
}