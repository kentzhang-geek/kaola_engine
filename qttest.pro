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
    kaola_engine/mopenglview.cpp \
    kaola_engine/Program.cpp \
    kaola_engine/Shader.cpp \
    kaola_engine/gl3d_image.cpp \
    models_cfg.cpp \
    ray_tracer.cpp

HEADERS  += mainwindow.h \
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
    kaola_engine/log.h \
    kaola_engine/model_manager.hpp \
    kaola_engine/shader_manager.hpp \
    ui_mainwindow.h \
    kaola_engine/Program.hpp \
    kaola_engine/Shader.hpp \
    kaola_engine/mopenglview.h \
    kaola_engine/glheaders.h \
    ray_tracer.h

FORMS    += mainwindow.ui

#INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include
#DEPENDPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include

#INCLUDEPATH += /usr/local/include/

#mac: LIBS += -framework GLUT -L/usr/local/lib/ -lassimp
#else:unix|win32: LIBS += -lGLUT

INCLUDEPATH += D:\User\Desktop\KLM\LIBS\assimp-3.2\include
INCLUDEPATH += D:\User\Desktop\KLM\LIBS\glm_build

RESOURCES +=

DISTFILES +=

target.files += $$[DISTFILES]
target.path = ./
INSTALLS += target

OBJECTIVE_SOURCES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../LIBS/assimp_build/code/release/ -lassimp-vc130-mtd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../LIBS/assimp_build/code/debug/ -lassimp-vc130-mtd
else:unix: LIBS += -L$$PWD/../LIBS/assimp_build/code/ -lassimp-vc130-mtd

INCLUDEPATH += $$PWD/../LIBS/assimp-3.2/include
DEPENDPATH += $$PWD/../LIBS/assimp-3.2/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../LIBS/assimp_build/code/release/libassimp-vc130-mtd.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../LIBS/assimp_build/code/debug/libassimp-vc130-mtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../LIBS/assimp_build/code/release/assimp-vc130-mtd.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../LIBS/assimp_build/code/debug/assimp-vc130-mtd.lib
else:unix: PRE_TARGETDEPS += $$PWD/../LIBS/assimp_build/code/libassimp-vc130-mtd.a

win32: LIBS += -L$$PWD/../LIBS/glew-1.13.0/lib/Release/x64/ -lglew32

INCLUDEPATH += $$PWD/../LIBS/glew-1.13.0/include
DEPENDPATH += $$PWD/../LIBS/glew-1.13.0/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../LIBS/glew-1.13.0/lib/Release/x64/glew32.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../LIBS/glew-1.13.0/lib/Release/x64/libglew32.a
