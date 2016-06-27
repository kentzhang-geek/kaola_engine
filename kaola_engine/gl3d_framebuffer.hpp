//
//  gl3d_framebuffer.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/9.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_framebuffer_hpp
#define gl3d_framebuffer_hpp

#include <stdio.h>
#include "gl3d_out_headers.h"
#include "log.h"

using namespace std;

typedef unsigned long long gl3d_frame_config;

#define GL3D_FRAME_HAS_COLOR_BUF (1UL << 0)
#define GL3D_FRAME_HAS_DEPTH_BUF (1UL << 1)
#define GL3D_FRAME_HAS_STENCIL_BUF (1UL << 2)

#define GL3D_FRAME_HAS_ALL (-1)

namespace gl3d {
    class gl3d_framebuffer {
    public:
        gl3d_framebuffer(gl3d_frame_config config, GLuint x, GLuint y);
        ~gl3d_framebuffer();
        void use_this_frame();
        void unbind_this_frame();
        void attach_color_text(GLuint text);
        void attach_depth_text(GLuint text);
        GLuint get_frame_obj();
    private:
        GLuint frame_obj;
        GLuint color_obj;
        GLuint depth_obj;
        GLuint stenc_obj;
        gl3d_frame_config config;
        GLuint width;
        GLuint height;
    };
}

#endif /* gl3d_framebuffer_hpp */
