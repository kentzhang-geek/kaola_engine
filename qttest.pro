#-------------------------------------------------
#
# Project created by QtCreator 2016-06-14T12:38:29
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qttest
TEMPLATE = app

SOURCES += \
    src/Qt_tests/main.cpp \
    src/Qt_tests/mainwindow.cpp \
    src/Qt_tests/models_cfg.cpp \
    src/Qt_tests/ray_tracer.cpp \
    src/render/default_shader_param.cpp \
    src/render/gl3d_framebuffer.cpp \
    src/render/gl3d_general_texture.cpp \
    src/render/gl3d_image.cpp \
    src/render/gl3d_material.cpp \
    src/render/gl3d_mesh.cpp \
    src/render/gl3d_object.cpp \
    src/render/gl3d_render_process.cpp \
    src/render/gl3d_scene.cpp \
    src/render/gl3d_texture.cpp \
    src/render/gl3d_viewer.cpp \
    src/render/model_manager.cpp \
    src/render/mopenglview.cpp \
    src/render/Program.cpp \
    src/render/render_processes.cpp \
    src/render/Shader.cpp \
    src/render/shader_list.cpp \
    src/render/shader_manager.cpp

HEADERS  += mainwindow.h \
    ui_mainwindow.h \
    ray_tracer.h \
    include/kaola_engine/gl3d.hpp \
    include/kaola_engine/gl3d_framebuffer.hpp \
    include/kaola_engine/gl3d_general_texture.hpp \
    include/kaola_engine/gl3d_image.hpp \
    include/kaola_engine/gl3d_material.hpp \
    include/kaola_engine/gl3d_mesh.h \
    include/kaola_engine/gl3d_obj_authority.h \
    include/kaola_engine/gl3d_object.h \
    include/kaola_engine/gl3d_out_headers.h \
    include/kaola_engine/gl3d_render_process.hpp \
    include/kaola_engine/gl3d_scene.h \
    include/kaola_engine/gl3d_texture.hpp \
    include/kaola_engine/gl3d_viewer.h \
    include/kaola_engine/glheaders.h \
    include/kaola_engine/kaola_engine.h \
    include/kaola_engine/log.h \
    include/kaola_engine/model_manager.hpp \
    include/kaola_engine/mopenglview.h \
    include/kaola_engine/Program.hpp \
    include/kaola_engine/Shader.hpp \
    include/kaola_engine/shader_manager.hpp \
    src/Qt_tests/mainwindow.h \
    src/Qt_tests/ray_tracer.h

FORMS    += \
    src/Qt_tests/mainwindow.ui

win32:INCLUDEPATH += D:\User\Desktop\KLM\qt_opengl_engine\include

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
