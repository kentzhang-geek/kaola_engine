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
    models_cfg.cpp

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
    kaola_engine/mopenglview.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include

INCLUDEPATH += /usr/local/include/

mac: LIBS += -framework GLUT -L/usr/local/lib/ -lassimp
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

RESOURCES +=

DISTFILES += \
    kaola_engine/dm.fdata \
    kaola_engine/dm2.fdata \
    kaola_engine/multiple_text_vector_shadow.fdata \
    kaola_engine/multiple_text_vector.fdata \
    kaola_engine/picking_mask.fdata \
    kaola_engine/picking_mask.vdata \
    kaola_engine/shadow_mask.fdata \
    kaola_engine/shadow_mask.vdata \
    kaola_engine/text_vector_light.fdata \
    kaola_engine/vector_light_toon.fdata \
    kaola_engine/vector_light.fdata \
    kaola_engine/default.fdata \
    kaola_engine/default.vdata \
    kaola_engine/dm.vdata \
    kaola_engine/dm2.vdata \
    kaola_engine/image.fdata \
    kaola_engine/image.vdata \
    kaola_engine/multiple_text_vector_shadow.vdata \
    kaola_engine/multiple_text_vector.vdata \
    kaola_engine/multiple_text.fdata \
    kaola_engine/multiple_text.vdata \
    kaola_engine/skybox.fdata \
    kaola_engine/skybox.vdata \
    kaola_engine/text_vector_light.vdata \
    kaola_engine/vector_light_toon.vdata \
    kaola_engine/vector_light.vdata

target.files += $$[DISTFILES]
target.path = ./
INSTALLS += target

OBJECTIVE_SOURCES +=
