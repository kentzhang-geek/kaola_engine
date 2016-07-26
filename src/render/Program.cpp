//
//  Program.cpp
//  GLES_Studies
//
//  Created by 刘罡 on 8/1/2016.
//  Copyright © 2016 刘罡. All rights reserved.
//

#include "kaola_engine/Program.hpp"

using namespace std;

gl3d::Program::Program(const std::string &vertexShader, const std::string &fragmentShader){
    programID = GL3D_GL()->glCreateProgram();
    if(programID == 0){
        throw std::runtime_error("Failed to create program");
    }
    createVertexShader(vertexShader)->createFragmentShader(fragmentShader);
    
    GL3D_GL()->glLinkProgram(programID);
    
    GLint status;
    GL3D_GL()->glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if(status == GL_FALSE){
        std::string msg("Failed to link progra : ");
        GLint infoLen;
        GL3D_GL()->glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLen);
        char* info = new char[infoLen + 1];
        GL3D_GL()->glGetProgramInfoLog(programID, infoLen, nullptr, info);
        msg += info;
        delete [] info;
        
        GL3D_GL()->glDeleteProgram(programID);
        cout << msg << endl;
        throw std::runtime_error(msg);
    }
}

gl3d::Program::~Program() {
    delete this->vertexShader;
    delete this->fragmentShader;
    GL3D_GL()->glDeleteProgram(this->programID);
    return;
}

gl3d::Program* gl3d::Program::createShader(const std::string &src, GLenum type){
    if(type == GL_VERTEX_SHADER){
        vertexShader = new gl3d::Shader(src, type);
    } else if(type == GL_FRAGMENT_SHADER){
        fragmentShader = new gl3d::Shader(src, type);
    } else {
        throw std::runtime_error("Incorrect Shader type");
    }
    return this;
}

gl3d::Program* gl3d::Program::createVertexShader(const std::string &src){
    createShader(src, GL_VERTEX_SHADER);
    GL3D_GL()->glAttachShader(programID, vertexShader->getShaderID());
    return this;
}

gl3d::Program* gl3d::Program::createFragmentShader(const std::string &src){
    createShader(src, GL_FRAGMENT_SHADER);
    GL3D_GL()->glAttachShader(programID, fragmentShader->getShaderID());
    return this;
}

GLuint gl3d::Program::getProgramID() const{
    return programID;
}

GLint gl3d::Program::getUniformIndex(const std::string &uniformaName) const{
    return GL3D_GL()->glGetUniformLocation(programID, uniformaName.c_str());
}

GLint gl3d::Program::getAttributeIndex(const std::string &attributeName) const{
    if(attributeName.length() == 0){
        throw std::runtime_error("Attribute name can not be empty");
    }
    GLint ret = GL3D_GL()->glGetAttribLocation(programID, attributeName.c_str());
    if(ret == -1){
        throw std::runtime_error("Program attribute not found : " + attributeName);
    }
    return ret;
}
