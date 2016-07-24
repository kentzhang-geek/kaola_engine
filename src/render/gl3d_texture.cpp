//
//  gl3d_texture.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_image.hpp"

using namespace std;
using namespace gl3d;

void gl3d_texture::init() {
//    memset(this, 0, sizeof(gl3d_texture));
}

void gl3d_texture::set_parami(bool repeat) {
    /**
     *  @author Kent, 16-01-18 22:01:22
     *
     *  这几个参数相当重要，不设置会导致贴图失败，注意似乎有平铺贴图和拉伸贴图的区别!
     */
    if (repeat) {
        // 这里平铺贴图
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else {
        // 这里应该是拉伸贴图
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL3D_GL()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

gl3d_texture::gl3d_texture(char * filename) : gl3d_general_texture(
                                                  gl3d_general_texture::GL3D_RGBA, 10, 10) {
    this->init();

    // create image
    this->img = new gl3d_image(filename);
    this->texture_name = string(filename);

    this->set_size_x(this->img->width);
    this->set_size_y(this->img->height);
    GLuint ooo = this->get_text_obj();
    this->buffer_data(this->img->data);
    
    // 贴图缓存好之后就删除通用内存中的图片数据
    delete this->img;
    this->img = NULL;
}

gl3d_texture::~gl3d_texture() {
    delete this->img;
    
    return;
}
