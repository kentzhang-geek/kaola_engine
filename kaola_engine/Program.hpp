//
//  Program.hpp
//  GLES_Studies
//
//  Created by 刘罡 on 8/1/2016.
//  Copyright © 2016 刘罡. All rights reserved.
//

#ifndef Program_hpp
#define Program_hpp

#include <string>
#include <iostream>

#include "glm/glm.hpp"
#include "Shader.hpp"

namespace gl3d {
    class Program{
    private:
        GLuint programID;
        gl3d::Shader *vertexShader;
        gl3d::Shader *fragmentShader;
        
        Program* createShader(const std::string &src, GLenum type);
        Program* createVertexShader(const std::string &src);
        Program* createFragmentShader(const std::string &src);
    public:
        Program(const std::string &vertSrc, const std::string &fragSrc);
        ~Program();
        ::std::string shader_name;
        GLuint getProgramID() const;
        GLint getAttributeIndex(const std::string &attributeName) const;
        GLint getUniformIndex(const std::string &uniformaName) const;        
    };
}

#endif /* Program_hpp */
