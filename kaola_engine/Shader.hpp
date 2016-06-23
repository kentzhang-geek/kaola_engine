//
//  Shader.hpp
//  GLES_Studies
//
//  Created by 刘罡 on 8/1/2016.
//  Copyright © 2016 刘罡. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <OpenGL.h>
#include <OpenGLAvailability.h>

#include <string>
#include <stdexcept>
#include <cassert>
#include <fstream>
#include <sstream>

#include "gl3d_out_headers.h"

namespace gl3d{
    
    class Shader
    {
    private:
        GLuint shaderID;
    public:
        Shader(const std::string &shaderSource, const GLenum type);
        virtual ~Shader();
        GLuint getShaderID() const;
    };
    
}

#endif /* Shader_hpp */
