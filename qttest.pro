#-------------------------------------------------
#
# Project created by QtCreator 2016-06-14T12:38:29
#
#-------------------------------------------------

QT       += core gui opengl widgets network

TRANSLATIONS += zh.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gltest
TEMPLATE = app

SOURCES += \
    src/Qt_tests/main.cpp \
    src/Qt_tests/models_cfg.cpp \
    src/render/render_addons/default_shader_param.cpp \
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
    src/render/render_addons/render_processes.cpp \
    src/render/Shader.cpp \
    src/render/render_addons/shader_list.cpp \
    src/render/shader_manager.cpp \
    src/utils/gl3d_scale.cpp \
    src/utils/gl3d_global_param.cpp \
    src/render/gl3d_post_process.cpp \
    src/render/render_addons/post_processer_1.cpp \
    src/render/render_addons/gl3d_post_process_template.cpp \
    src/render/gl3d_general_light_source.cpp \
    src/render/light_system/simple_directional_light.cpp \
#    src/Qt_tests/ui/pickupdig.cpp \
    src/editor/bounding_box.cpp \
    src/editor/vertex.cpp \
    src/editor/gl_utility.cpp \
#    src/editor/klm_surface.cpp \
    src/utils/gl3d_lock.cpp \
    src/utils/gl3d_math.cpp \
    src/Qt_tests/ui/drawoption.cpp \
    src/utils/qui/frmmessagebox.cpp \
    src/utils/qui/iconhelper.cpp \
#    src/editor/gl3d_surface_object.cpp \
    src/render/gl3d_abstract_object.cpp \
#    src/editor/surface.cpp \
    src/editor/merchandise.cpp \
    src/utils/io_utility.cpp \
#    src/resource_and_network/global_material.cpp \
#    src/render/loading_object.cpp \
#    src/editor/gl3d_door.cpp \
#    src/editor/gl3d_window.cpp \
#    src/editor/style_package.cpp \
    demomainwindow.cpp \
    src/render/spacemanager.cpp \
    src/render/SharedObject.cpp

HEADERS  += \
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
    src/Qt_tests/ray_tracer.h \
    include/utils/gl3d_scale.h \
    include/utils/gl3d_global_param.h \
    include/kaola_engine/gl3d_post_process.h \
    include/utils/gl3d_utils.h \
    include/utils/gl3d_post_process_template.h \
    include/kaola_engine/gl3d_general_light_source.h \
    include/editor/bounding_box.h \
#    src/Qt_tests/drawhomewin.h \
    include/editor/vertex.h \
    include/utils/gl3d_path_config.h \
#    src/Qt_tests/ui/pickupdig.h \
    include/editor/gl_utility.h \
    include/utils/gl3d_lock.h \
    include/utils/gl3d_math.h \
    src/Qt_tests/ui/drawoption.h \
    src/utils/qui/frmmessagebox.h \
    src/utils/qui/iconhelper.h \
    src/utils/qui/myhelper.h \
#    include/editor/gl3d_surface_object.h \
    include/kaola_engine/gl3d_abstract_object.h \
#    include/editor/surface.h \
    include/editor/merchandise.h \
    include/utils/io_utility.h \
#    include/resource_and_network/global_material.h \
#    include/kaola_engine/loading_object.h \
#    include/editor/gl3d_door.h \
#    include/editor/gl3d_window.h \
    include/editor/sign_config.h \
#    include/editor/style_package.h \
    include/resource_and_network/network_tool.h \
#    include/resource_and_network/pack_tool.h \
    include/resource_and_network/global_info.h \
    demomainwindow.h \
    include/kaola_engine/spacemanager.h \
    include/kaola_engine/SharedObject.h

FORMS    += \
    src/utils/qui/frmmessagebox.ui \
    ui/window_or_door_selected.ui \
    ui/furniture_selected.ui \
    ui/login.ui \
    ui/proc.ui \
    demomainwindow.ui

PRECOMPILED_HEADER += $$HEADERS

win32:INCLUDEPATH += include

INCLUDEPATH += LIBS\assimp-3.2\include
INCLUDEPATH += LIBS\glm_build
INCLUDEPATH += include


macx: INCLUDEPATH += LIBS/assimp-3.2/include
macx: INCLUDEPATH += LIBS/glm_build
macx: INCLUDEPATH += include

RESOURCES += \
    res.qrc \
    src/utils/qui/rc.qrc \
    src/utils/qui/rc.qrc

DISTFILES += \
    zh.ts \
    zh.qm

target.files += $$[DISTFILES]
target.path = ./
INSTALLS += target

OBJECTIVE_SOURCES +=

win32: LIBS += -L$$PWD/LIBS/assimp_install/lib/ -lassimp

INCLUDEPATH += $$PWD/LIBS/assimp_install/include
DEPENDPATH += $$PWD/LIBS/assimp_install/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/assimp_install/lib/assimp.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/assimp_install/lib/libassimp.a

win32: LIBS += -L$$PWD/LIBS/assimp_install/lib/ -lzlibstatic

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/assimp_install/lib/zlibstatic.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/assimp_install/lib/libzlibstatic.a

win32: LIBS += -L$$PWD/LIBS/pugixml-1.7/install/lib/ -lpugixml

INCLUDEPATH += $$PWD/LIBS/pugixml-1.7/install/include
DEPENDPATH += $$PWD/LIBS/pugixml-1.7/install/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/pugixml-1.7/install/lib/pugixml.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/pugixml-1.7/install/lib/libpugixml.a
