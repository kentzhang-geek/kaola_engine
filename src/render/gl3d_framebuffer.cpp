//
//  gl3d_framebuffer.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/9.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d_framebuffer.hpp"

using namespace gl3d;

gl3d_framebuffer::gl3d_framebuffer(gl3d_frame_config config_in, GLuint x, GLuint y) {
    this->width = x;
    this->height = y;
    this->config = config_in;
    this->is_generated_by_object = false;
    
    glGenFramebuffers(1, &this->frame_obj);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_obj);
    glViewport(0, 0, width, height);
    
//    if (config & GL3D_FRAME_HAS_COLOR_BUF) {
//        // create a color buffer that has same size as frame buffer
//        glGenRenderbuffers(1, &this->color_obj);
//        glBindRenderbuffer(GL_RENDERBUFFER, this->color_obj);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
//        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->color_obj);
//        GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//        if (status != GL_FRAMEBUFFER_COMPLETE) {
//            GL3D_UTILS_ERROR("attach color text : Framebuffer status: %x", (int)status);
//        }
//    }

//    // 有深度时
//    if (config & GL3D_FRAME_HAS_DEPTH_BUF) {
//        // create depth & stencil packed buffer
//        glGenRenderbuffers(1, &this->depth_obj);
//        glBindRenderbuffer(GL_RENDERBUFFER, this->depth_obj);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depth_obj);
//        GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//        if (status != GL_FRAMEBUFFER_COMPLETE) {
//            GL3D_UTILS_ERROR("attach color text : Framebuffer status: %x", (int)status);
//        }
//    }
//    // 有模板缓冲区
//    if (config & GL3D_FRAME_HAS_STENCIL_BUF) {
//        // Create a stencil buffer that has the same size as the frame buffer.
//        glGenRenderbuffers(1, &this->stenc_obj);
//        glBindRenderbuffer(GL_RENDERBUFFER, this->stenc_obj);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
//        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER, this->stenc_obj);
//        GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//        if (status != GL_FRAMEBUFFER_COMPLETE) {
//            GL3D_UTILS_ERROR("attach color text : Framebuffer status: %x", (int)status);
//        }
//    }

    if (config & GL3D_FRAME_HAS_COLOR_BUF) {
        // create a color buffer that has same size as frame buffer
        glGenRenderbuffers(1, &this->color_obj);
        glBindRenderbuffer(GL_RENDERBUFFER, this->color_obj);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->color_obj);
    }

    // 同时有模板和深度时
    if ((config & GL3D_FRAME_HAS_DEPTH_BUF) && (config & GL3D_FRAME_HAS_STENCIL_BUF)) {
        // create depth & stencil packed buffer
        glGenRenderbuffers(1, &this->stenc_depth_obj);
        glBindRenderbuffer(GL_RENDERBUFFER, this->stenc_depth_obj);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->stenc_depth_obj);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->stenc_depth_obj);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            GL3D_UTILS_WARN("Framebuffer status: %x", (int)status);
        }
    }
    // 单独有模板缓冲区
    else if (config & GL3D_FRAME_HAS_STENCIL_BUF) {
        // Create a stencil buffer that has the same size as the frame buffer.
        glGenRenderbuffers(1, &this->stenc_depth_obj);
        glBindRenderbuffer(GL_RENDERBUFFER, this->stenc_depth_obj);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->stenc_depth_obj);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            GL3D_UTILS_WARN("Framebuffer status: %x", (int)status);
        }
    }
    // 只有深度缓冲区时
    else if (config & GL3D_FRAME_HAS_DEPTH_BUF) {
        // Create a depth buffer that has the same size as the frame buffer.
        glGenRenderbuffers(1, &this->depth_obj);
        glBindRenderbuffer(GL_RENDERBUFFER, this->depth_obj);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depth_obj);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            GL3D_UTILS_WARN("Framebuffer status: %x", (int)status);
        }
    }
}

gl3d_framebuffer::gl3d_framebuffer(GLuint buffer_object, glm::vec2 size) {
    this->frame_obj = buffer_object;
    this->is_generated_by_object = true;
    this->width = size.x;
    this->height = size.y;
}

bool gl3d_framebuffer::is_complete() {
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        GL3D_UTILS_WARN("Framebuffer status: %x", (int)status);
        return false;
    }

    return true;
}

gl3d_framebuffer::~gl3d_framebuffer() {
    glDeleteFramebuffers(1, &this->frame_obj);
    if (this->config & GL3D_FRAME_HAS_COLOR_BUF) {
        glDeleteRenderbuffers(1, &this->color_obj);
    }
    if (this->config & GL3D_FRAME_HAS_DEPTH_BUF) {
        glDeleteRenderbuffers(1, &this->depth_obj);
    }
    if (this->config & GL3D_FRAME_HAS_STENCIL_BUF) {
        glDeleteRenderbuffers(1, &this->stenc_depth_obj);
    }
}

void gl3d_framebuffer::use_this_frame() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_obj);
    glViewport(0, 0, width, height);
}

void gl3d_framebuffer::unbind_this_frame() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void gl3d_framebuffer::attach_color_text(GLuint text) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_obj);
    glViewport(0, 0, width, height);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, text, 0);
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        GL3D_UTILS_WARN("attach color text : Framebuffer status: %x", (int)status);
    }
}

void gl3d_framebuffer::attach_depth_text(GLuint text) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_obj);
    glViewport(0, 0, width, height);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, text, 0);
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        GL3D_UTILS_WARN("framebuffer attach_depth_text failed");
        GL3D_UTILS_WARN("Framebuffer status: %x", (int)status);
    }
}

GLuint gl3d_framebuffer::get_frame_obj() {
    return this->frame_obj;
}

bool gl3d_framebuffer::generated_by_object() {
    return this->is_generated_by_object;
}