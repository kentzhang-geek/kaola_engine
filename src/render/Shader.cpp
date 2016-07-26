//
//  Shader.cpp
//  GLES_Studies
//
//  Created by 刘罡 on 8/1/2016.
//  Copyright © 2016 刘罡. All rights reserved.
//

#include "kaola_engine/Shader.hpp"

gl3d::Shader::Shader(const std::string &shaderSource, GLenum shaderType){
    
    shaderID = GL3D_GL()->glCreateShader(shaderType);
    if(shaderID == 0){
        throw std::runtime_error("Failed to create shader");
    }
    
    const char* code = shaderSource.c_str();
    GL3D_GL()->glShaderSource(shaderID, 1, (const GLchar**) &code, nullptr);
    GL3D_GL()->glCompileShader(shaderID);
    
    GLint status;
    GL3D_GL()->glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        std::string msg("Failed to compile Shader:");
        
        GLint messageLen;
        GL3D_GL()->glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &messageLen);
        char* compileInfo = new char[messageLen + 1];
        GL3D_GL()->glGetShaderInfoLog(shaderID, messageLen, nullptr, compileInfo);
        msg += compileInfo;
        
        delete[] compileInfo;
        
        GL3D_GL()->glDeleteShader(shaderID);
        shaderID = 0;
        throw std::runtime_error(msg);
    }
}

gl3d::Shader::~Shader(){
    GL3D_GL()->glDeleteShader(shaderID);
    shaderID = 0;
}

GLuint gl3d::Shader::getShaderID() const{
    return shaderID;
}
