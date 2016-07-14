//
//  gl3d_general_texture.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/6/9.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_general_texture_hpp
#define gl3d_general_texture_hpp

#include <stdio.h>
#include "gl3d_out_headers.h"
#include "utils/gl3d_utils.h"

using namespace std;

namespace gl3d {
    class gl3d_general_texture {
    public:
        enum texture_type {
            GL3D_DEPTH_COMPONENT = 0,
            GL3D_ALPHA,
            GL3D_RGB,
            GL3D_RGBA,
            GL3D_LUMINANCE,
            GL3D_LUMINANCE_ALPHA
        };
        gl3d_general_texture() {};
        gl3d_general_texture(texture_type type, GLuint x, GLuint y);
        ~gl3d_general_texture();
        void bind(GLenum text_unit);
        void buffer_data(GLvoid * data);
        void clean_data();
        static void set_parami(bool repeat);

        // Properties
        GL3D_UTILS_PROPERTY(text_obj, GLuint);
        GL3D_UTILS_PROPERTY(size_x, GLuint);
        GL3D_UTILS_PROPERTY(size_y, GLuint);
        GL3D_UTILS_PROPERTY(type, texture_type);

    private:
        void init();
    };
};

using namespace gl3d;

#endif /* gl3d_general_texture_hpp */
