#-------------------------------------------------
#
# Project created by QtCreator 2016-06-14T12:38:29
#
#-------------------------------------------------

QT       += core gui opengl widgets

win32: QMAKE_CXXFLAGS += /MP

TRANSLATIONS += zh.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qttest
TEMPLATE = app

SOURCES += \
    src/Qt_tests/main.cpp \
    src/Qt_tests/mainwindow.cpp \
    src/Qt_tests/models_cfg.cpp \
    src/Qt_tests/ray_tracer.cpp \
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
    src/editor/gl3d_wall.cpp \
    src/Qt_tests/drawhomewin.cpp \
    src/Qt_tests/ui/pickupdig.cpp \
    src/editor/bounding_box.cpp \
    src/editor/vertex.cpp \
    src/editor/gl_utility.cpp \
#    src/editor/klm_surface.cpp \
    src/utils/gl3d_lock.cpp \
    src/utils/gl3d_math.cpp \
    src/Qt_tests/ui/drawoption.cpp \
    src/utils/qui/frmmessagebox.cpp \
    src/utils/qui/iconhelper.cpp \
    src/editor/gl3d_surface_object.cpp \
    src/render/gl3d_abstract_object.cpp \
    src/editor/surface.cpp \
    src/editor/merchandise.cpp \
    src/resource_and_network/klm_resource_manager.cpp \
    src/utils/io_utility.cpp \
    src/resource_and_network/global_material.cpp \
    src/editor/design_scheme.cpp \
    src/Qt_tests/commands.cpp \
    src/editor/command.cpp

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
    src/Qt_tests/mainwindow.h \
    src/Qt_tests/ray_tracer.h \
    include/utils/gl3d_scale.h \
    include/utils/gl3d_global_param.h \
    include/kaola_engine/gl3d_post_process.h \
    include/utils/gl3d_utils.h \
    include/utils/gl3d_post_process_template.h \
    include/kaola_engine/gl3d_general_light_source.h \
#    include/editor/klm_surface.h \
    include/editor/bounding_box.h \
    src/Qt_tests/drawhomewin.h \
    include/editor/vertex.h \
    include/utils/gl3d_path_config.h \
    src/Qt_tests/ui/pickupdig.h \
    include/editor/gl3d_wall.h \
    include/editor/gl_utility.h \
    include/utils/gl3d_lock.h \
    include/utils/gl3d_math.h \
    src/Qt_tests/ui/drawoption.h \
    src/utils/qui/frmmessagebox.h \
    src/utils/qui/iconhelper.h \
    src/utils/qui/myhelper.h \
    include/editor/gl3d_surface_object.h \
    include/kaola_engine/gl3d_abstract_object.h \
    include/editor/surface.h \
    include/editor/merchandise.h \
    include/resource_and_network/klm_resource_manager.h \
    include/utils/io_utility.h \
    include/resource_and_network/global_material.h \
    include/editor/design_scheme.h \
    src/Qt_tests/commands.h \
    include/editor/command.h

FORMS    += \
    src/Qt_tests/mainwindow.ui \
    src/Qt_tests/drawhomewin.ui \
    src/utils/qui/frmmessagebox.ui

PRECOMPILED_HEADER += $$HEADERS

win32:INCLUDEPATH += include

win32:INCLUDEPATH += LIBS/cgal/auxiliary/gmp/include/

#INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include
#DEPENDPATH += $$PWD/../../../../usr/local/Cellar/glm/0.9.7.1/include

#INCLUDEPATH += /usr/local/include/

#mac: LIBS += -framework GLUT -L/usr/local/lib/ -lassimp
#else:unix|win32: LIBS += -lGLUT

INCLUDEPATH += LIBS\assimp-3.2\include
INCLUDEPATH += LIBS\glm_build
INCLUDEPATH += LIBS\boost_1_61_0
INCLUDEPATH += include


macx: INCLUDEPATH += LIBS/assimp-3.2/include
macx: INCLUDEPATH += LIBS/glm_build
macx: INCLUDEPATH += LIBS/boost_1_61_0
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/LIBS/assimp_build/code/release/ -lassimp-vc130-mtd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/LIBS/assimp_build/code/debug/ -lassimp-vc130-mtd
#else:unix: LIBS += -L$$PWD/LIBS/assimp_build/code/ -lassimp-vc130-mtd

INCLUDEPATH += $$PWD/LIBS/assimp-3.2/include
DEPENDPATH += $$PWD/LIBS/assimp-3.2/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/assimp_build/code/release/libassimp-vc130-mtd.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/assimp_build/code/debug/libassimp-vc130-mtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/assimp_build/code/release/assimp-vc130-mtd.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/assimp_build/code/debug/assimp-vc130-mtd.lib
#else:unix: PRE_TARGETDEPS += $$PWD/LIBS/assimp_build/code/libassimp-vc130-mtd.a

#win32: LIBS += -L$$PWD/LIBS/glew-1.13.0/lib/Release/x64/ -lglew32

#INCLUDEPATH += $$PWD/LIBS/glew-1.13.0/include
#DEPENDPATH += $$PWD/LIBS/glew-1.13.0/include

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/glew-1.13.0/lib/Release/x64/glew32.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/glew-1.13.0/lib/Release/x64/libglew32.a

win32: LIBS += -lGlU32

macx: LIBS += -L$$PWD/LIBS/assimp-3.2/lib/ -lassimp

DEPENDPATH += $$PWD/LIBS/assimp-3.2/include

macx: PRE_TARGETDEPS += $$PWD/LIBS/assimp-3.2/lib/libassimp.a

macx: LIBS += -L$$PWD/LIBS/pugixml-1.7/scripts/ -lpugixml

INCLUDEPATH += $$PWD/LIBS/pugixml-1.7/src
DEPENDPATH += $$PWD/LIBS/pugixml-1.7/src

macx: PRE_TARGETDEPS += $$PWD/LIBS/pugixml-1.7/scripts/libpugixml.a

win32: LIBS += -L$$PWD/LIBS/pugixml-1.7/pugixml-1.7/scripts/vs2015/x64_Debug/ -lpugixml

INCLUDEPATH += $$PWD/LIBS/pugixml-1.7/pugixml-1.7/src
DEPENDPATH += $$PWD/LIBS/pugixml-1.7/pugixml-1.7/src

win32: PRE_TARGETDEPS += $$PWD/LIBS/pugixml-1.7/pugixml-1.7/scripts/vs2015/x64_Debug/pugixml.lib


win32: LIBS += -L$$PWD/LIBS/cgal_lib/lib/ -lCGAL_Core-vc140-mt-gd-4.8.1

INCLUDEPATH += $$PWD/LIBS/cgal/include
DEPENDPATH += $$PWD/LIBS/cgal/include
INCLUDEPATH += $$PWD/LIBS/cgal_lib/include
DEPENDPATH += $$PWD/LIBS/cgal_lib/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/CGAL_Core-vc140-mt-gd-4.8.1.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/libCGAL_Core-vc140-mt-gd-4.8.1.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/LIBS/cgal_lib/lib/ -lCGAL-vc140-mt-gd-4.8.1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/LIBS/cgal_lib/lib/ -lCGAL-vc140-mt-gd-4.8.1

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/libCGAL-vc140-mt-gd-4.8.1.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/libCGAL-vc140-mt-gd-4.8.1.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/CGAL-vc140-mt-gd-4.8.1.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/CGAL-vc140-mt-gd-4.8.1.lib

win32: LIBS += -L$$PWD/LIBS/cgal_lib/lib/Debug/ -lCGAL_Core-vc140-mt-gd-4.8.1

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/Debug/CGAL_Core-vc140-mt-gd-4.8.1.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/Debug/libCGAL_Core-vc140-mt-gd-4.8.1.a

win32: LIBS += -L$$PWD/LIBS/cgal_lib/lib/Debug/ -lCGAL-vc140-mt-gd-4.8.1

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/Debug/CGAL-vc140-mt-gd-4.8.1.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LIBS/cgal_lib/lib/Debug/libCGAL-vc140-mt-gd-4.8.1.a
