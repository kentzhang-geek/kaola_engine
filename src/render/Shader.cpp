//
//  Shader.cpp
//  GLES_Studies
//
//  Created by 刘罡 on 8/1/2016.
//  Copyright © 2016 刘罡. All rights reserved.
//

#include "kaola_engine/Shader.hpp"

gl3d::Shader::Shader(const std::string &shaderSource, GLenum shaderType){
    
    shaderID = glCreateShader(shaderType);
    if(shaderID == 0){
        throw std::runtime_error("Failed to create shader");
    }
    
    const char* code = shaderSource.c_str();
    glShaderSource(shaderID, 1, (const GLchar**) &code, nullptr);
    glCompileShader(shaderID);
    
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        std::string msg("Failed to compile Shader:");
        
        GLint messageLen;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &messageLen);
        char* compileInfo = new char[messageLen + 1];
        glGetShaderInfoLog(shaderID, messageLen, nullptr, compileInfo);
        msg += compileInfo;
        
        delete[] compileInfo;
        
        glDeleteShader(shaderID);
        shaderID = 0;
        throw std::runtime_error(msg);
    }
}

gl3d::Shader::~Shader(){
    glDeleteShader(shaderID);
    shaderID = 0;
}

GLuint gl3d::Shader::getShaderID() const{
    return shaderID;
}
