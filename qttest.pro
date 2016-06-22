#-------------------------------------------------
#
# Project created by QtCreator 2016-06-14T12:38:29
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qttest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mopenglview.cpp \
    kaola_engine/assimp/color4.inl \
    kaola_engine/assimp/material.inl \
    kaola_engine/assimp/matrix3x3.inl \
    kaola_engine/assimp/matrix4x4.inl \
    kaola_engine/assimp/quaternion.inl \
    kaola_engine/assimp/vector2.inl \
    kaola_engine/assimp/vector3.inl \
    kaola_engine/default_shader_param.cpp \
    kaola_engine/gl3d_framebuffer.cpp \
    kaola_engine/gl3d_general_texture.cpp \
    kaola_engine/gl3d_material.cpp \
    kaola_engine/gl3d_mesh.cpp \
    kaola_engine/gl3d_object.cpp \
    kaola_engine/gl3d_render_process.cpp \
    kaola_engine/gl3d_scene.cpp \
    kaola_engine/gl3d_texture.cpp \
    kaola_engine/gl3d_viewer.cpp \
    kaola_engine/model_manager.cpp \
    kaola_engine/render_processes.cpp \
    kaola_engine/shader_list.cpp \
    kaola_engine/shader_manager.cpp \
    kaola_engine/tiny_obj_loader.cc \
    kaola_engine/mopenglview.cpp \
    kaola_engine/Program.cpp \
    kaola_engine/Shader.cpp

HEADERS  += mainwindow.h \
    mopenglview.h \
    kaola_engine/gl3d.hpp \
    kaola_engine/gl3d_framebuffer.hpp \
    kaola_engine/gl3d_general_texture.hpp \
    kaola_engine/gl3d_image.hpp \
    kaola_engine/gl3d_material.hpp \
    kaola_engine/gl3d_obj_authority.h \
    kaola_engine/gl3d_out_headers.h \
    kaola_engine/gl3d_render_process.hpp \
    kaola_engine/gl3d_texture.hpp \
    kaola_engine/kaola_engine.h \
    kaola_engine/KaolaView.h \
    kaola_engine/log.h \
    kaola_engine/model_manager.hpp \
    kaola_engine/shader_manager.hpp \
    kaola_engine/tiny_obj_loader.h \
    ui_mainwindow.h \
    kaola_engine/Program.hpp \
    kaola_engine/Shader.hpp

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include


INCLUDEPATH += /Users/kent/Skylab/third_party/oglplus/include
DEPENDPATH += /Users/kent/Skylab/third_party/oglplus/include

mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/release/ -lGLEW
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/debug/ -lGLEW
else:unix: LIBS += -L$$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/ -lGLEW

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/release/libGLEW.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/debug/libGLEW.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/release/GLEW.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/debug/GLEW.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/glew/1.11.0/lib/libGLEW.a

RESOURCES += \
    shaders.qrc

DISTFILES +=

OBJECTIVE_SOURCES += \
    kaola_engine/gl3d_image.mm \
    kaola_engine/KaolaView.mm
