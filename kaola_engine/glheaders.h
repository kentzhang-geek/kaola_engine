#ifndef GLHEADERS_H
#define GLHEADERS_H

// QT里面使用glew会有冲突!
//#define GLEW_STATIC
//#pragma comment(lib, "glew32.lib")
//#include "GL/glew.h"

#include <QtOpenGL>
#include <QOpenGLFunctions_3_0>

extern QOpenGLFunctions_3_0 * gl3d_win_gl_functions;


// 直接用宏去替换用到的function
#define glActiveTexture(...) gl3d_win_gl_functions->glActiveTexture(__VA_ARGS__)
#define glUniform1f(...) gl3d_win_gl_functions->glUniform1f(__VA_ARGS__)
#define glUniform4fv(...) gl3d_win_gl_functions->glUniform4fv(__VA_ARGS__)
#define glUniformMatrix4fv(...) gl3d_win_gl_functions->glUniformMatrix4fv(__VA_ARGS__)
#define glUniform3fv(...) gl3d_win_gl_functions->glUniform3fv(__VA_ARGS__)
#define glGetUniformLocation(...) gl3d_win_gl_functions->glGetUniformLocation(__VA_ARGS__)
#define glUniform1i(...) gl3d_win_gl_functions->glUniform1i(__VA_ARGS__)
#define glBindBuffer(...) gl3d_win_gl_functions->glBindBuffer(__VA_ARGS__)
#define glBufferData(...) gl3d_win_gl_functions->glBufferData(__VA_ARGS__)
#define glGenBuffers(...) gl3d_win_gl_functions->glGenBuffers(__VA_ARGS__)
#define glDeleteBuffers(...) gl3d_win_gl_functions->glDeleteBuffers(__VA_ARGS__)
#define glGenFramebuffers(...) gl3d_win_gl_functions->glGenFramebuffers(__VA_ARGS__)
#define glBindFramebuffer(...) gl3d_win_gl_functions->glBindFramebuffer(__VA_ARGS__)
#define glGenRenderbuffers(...) gl3d_win_gl_functions->glGenRenderbuffers(__VA_ARGS__)
#define glBindRenderbuffer(...) gl3d_win_gl_functions->glBindRenderbuffer(__VA_ARGS__)
#define glFramebufferRenderbuffer(...) gl3d_win_gl_functions->glFramebufferRenderbuffer(__VA_ARGS__)
#define glCheckFramebufferStatus(...) gl3d_win_gl_functions->glCheckFramebufferStatus(__VA_ARGS__)
#define glBindVertexArray(...) gl3d_win_gl_functions->glBindVertexArray(__VA_ARGS__)
#define glEnableVertexAttribArray(...) gl3d_win_gl_functions->glEnableVertexAttribArray(__VA_ARGS__)
#define glVertexAttribPointer(...) gl3d_win_gl_functions->glVertexAttribPointer(__VA_ARGS__)
#define glGetAttribLocation(...) gl3d_win_gl_functions->glGetAttribLocation(__VA_ARGS__)
#define glUniform2fv(...) gl3d_win_gl_functions->glUniform2fv(__VA_ARGS__)
#define glRenderbufferStorage(...) gl3d_win_gl_functions->glRenderbufferStorage(__VA_ARGS__)
#define glDeleteFramebuffers(...) gl3d_win_gl_functions->glDeleteFramebuffers(__VA_ARGS__)
#define glDeleteRenderbuffers(...) gl3d_win_gl_functions->glDeleteRenderbuffers(__VA_ARGS__)
#define glFramebufferTexture2D(...) gl3d_win_gl_functions->glFramebufferTexture2D(__VA_ARGS__)
#define glCreateProgram(...) gl3d_win_gl_functions->glCreateProgram(__VA_ARGS__)
#define glLinkProgram(...) gl3d_win_gl_functions->glLinkProgram(__VA_ARGS__)
#define glGetProgramiv(...) gl3d_win_gl_functions->glGetProgramiv(__VA_ARGS__)
#define glGetProgramInfoLog(...) gl3d_win_gl_functions->glGetProgramInfoLog(__VA_ARGS__)
#define glAttachShader(...) gl3d_win_gl_functions->glAttachShader(__VA_ARGS__)
#define glDeleteProgram(...) gl3d_win_gl_functions->glDeleteProgram(__VA_ARGS__)
#define glClearDepthf(...) gl3d_win_gl_functions->glClearDepth(__VA_ARGS__)
#define glUseProgram(...) gl3d_win_gl_functions->glUseProgram   (__VA_ARGS__)
#define glUniformMatrix3fv(...) gl3d_win_gl_functions->glUniformMatrix3fv(__VA_ARGS__)
#define glGenVertexArrays(...) gl3d_win_gl_functions->glGenVertexArrays(__VA_ARGS__)
#define glDeleteVertexArrays(...) gl3d_win_gl_functions->glDeleteVertexArrays(__VA_ARGS__)
#define glCreateShader(...) gl3d_win_gl_functions->glCreateShader(__VA_ARGS__)
#define glShaderSource(...) gl3d_win_gl_functions->glShaderSource(__VA_ARGS__)
#define glCompileShader(...) gl3d_win_gl_functions->glCompileShader(__VA_ARGS__)
#define glGetShaderiv(...) gl3d_win_gl_functions->glGetShaderiv(__VA_ARGS__)
#define glGetShaderInfoLog(...) gl3d_win_gl_functions->glGetShaderInfoLog(__VA_ARGS__)
#define glDeleteShader(...) gl3d_win_gl_functions->glDeleteShader(__VA_ARGS__)
#define glColorMask(...) gl3d_win_gl_functions->glColorMask(__VA_ARGS__)
#define glEnable(...) gl3d_win_gl_functions->glEnable(__VA_ARGS__)
#define glStencilFunc(...) gl3d_win_gl_functions->glStencilFunc(__VA_ARGS__)
#define glDisable(...) gl3d_win_gl_functions->glDisable(__VA_ARGS__)
#define glStencilOp(...) gl3d_win_gl_functions->glStencilOp(__VA_ARGS__)
#define glDrawElements(...) gl3d_win_gl_functions->glDrawElements(__VA_ARGS__)
#define glDepthFunc(...) gl3d_win_gl_functions->glDepthFunc(__VA_ARGS__)
#define glCullFace(...) gl3d_win_gl_functions->glCullFace(__VA_ARGS__)
#define glBlendFunc(...) gl3d_win_gl_functions->glBlendFunc(__VA_ARGS__)
#define glTexParameteri(...) gl3d_win_gl_functions->glTexParameteri(__VA_ARGS__)
#define glGenTextures(...) gl3d_win_gl_functions->glGenTextures(__VA_ARGS__)
#define glBindTexture(...) gl3d_win_gl_functions->glBindTexture(__VA_ARGS__)
#define glTexImage2D(...) gl3d_win_gl_functions->glTexImage2D(__VA_ARGS__)
#define glDeleteTextures(...) gl3d_win_gl_functions->glDeleteTextures(__VA_ARGS__)

#define glDepthMask(...) gl3d_win_gl_functions->glDepthMask(__VA_ARGS__)
#define glViewport(...) gl3d_win_gl_functions->glViewport(__VA_ARGS__)
#define glClear(...) gl3d_win_gl_functions->glClear(__VA_ARGS__)
#define glClearColor(...) gl3d_win_gl_functions->glClearColor(__VA_ARGS__)
#define glReadPixels(...) gl3d_win_gl_functions->glReadPixels(__VA_ARGS__)

#endif // GLHEADERS_H
