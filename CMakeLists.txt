project(qttest)
cmake_minimum_required(VERSION 2.8.11)

# root of your msvc14 x64 prebuild, there should be a `/lib/cmake` dir
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{QT5_DIR})
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "D:/Qt/Qt5.6.1/5.6/msvc2015_64") # remember to add `C:QtVS2015x64Qt5.6.05.6msvc2015_64bin` to %PATH%
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(SRCS
        src/Qt_tests/main.cpp
        src/Qt_tests/mainwindow.cpp
        src/Qt_tests/models_cfg.cpp
        src/Qt_tests/ray_tracer.cpp
        src/render/render_addons/default_shader_param.cpp
        src/render/gl3d_framebuffer.cpp
        src/render/gl3d_general_texture.cpp
        src/render/gl3d_image.cpp
        src/render/gl3d_material.cpp
        src/render/gl3d_mesh.cpp
        src/render/gl3d_object.cpp
        src/render/gl3d_render_process.cpp
        src/render/gl3d_scene.cpp
        src/render/gl3d_texture.cpp
        src/render/gl3d_viewer.cpp
        src/render/model_manager.cpp
        src/render/mopenglview.cpp
        src/render/Program.cpp
        src/render/render_addons/render_processes.cpp
        src/render/Shader.cpp
        src/render/render_addons/shader_list.cpp
        src/render/shader_manager.cpp
        src/utils/gl3d_scale.cpp
        src/utils/gl3d_global_param.cpp
        src/render/gl3d_post_process.cpp
        src/render/render_addons/post_processer_1.cpp
        src/render/render_addons/gl3d_post_process_template.cpp
        src/render/gl3d_general_light_source.cpp
        src/render/light_system/simple_directional_light.cpp
        src/editor/gl3d_wall.cpp
        src/Qt_tests/drawhomewin.cpp
        src/Qt_tests/ui/pickupdig.cpp
        src/editor/bounding_box.cpp
        src/editor/vertex.cpp
        src/editor/gl_utility.cpp
        #    src/editor/klm_surface.cpp
        src/utils/gl3d_lock.cpp
        src/utils/gl3d_math.cpp
        src/Qt_tests/ui/drawoption.cpp
        src/utils/qui/frmmessagebox.cpp
        src/utils/qui/iconhelper.cpp
        src/editor/gl3d_surface_object.cpp
        src/render/gl3d_abstract_object.cpp
        src/editor/surface.cpp
        src/editor/merchandise.cpp
        src/resource_and_network/klm_resource_manager.cpp
        src/utils/io_utility.cpp
        src/resource_and_network/global_material.cpp
        src/editor/design_scheme.cpp
        src/editor/command.cpp
        src/render/loading_object.cpp
        src/editor/gl3d_door.cpp
        src/editor/gl3d_window.cpp
        src/editor/style_package.cpp
        src/resource_and_network/network_tool.cpp
        )

set(qttest_MOCS
        include/kaola_engine/gl3d.hpp
        include/kaola_engine/gl3d_framebuffer.hpp
        include/kaola_engine/gl3d_general_texture.hpp
        include/kaola_engine/gl3d_image.hpp
        include/kaola_engine/gl3d_material.hpp
        include/kaola_engine/gl3d_mesh.h
        include/kaola_engine/gl3d_obj_authority.h
        include/kaola_engine/gl3d_object.h
        include/kaola_engine/gl3d_out_headers.h
        include/kaola_engine/gl3d_render_process.hpp
        include/kaola_engine/gl3d_scene.h
        include/kaola_engine/gl3d_texture.hpp
        include/kaola_engine/gl3d_viewer.h
        include/kaola_engine/glheaders.h
        include/kaola_engine/kaola_engine.h
        include/kaola_engine/log.h
        include/kaola_engine/model_manager.hpp
        include/kaola_engine/mopenglview.h
        include/kaola_engine/Program.hpp
        include/kaola_engine/Shader.hpp
        include/kaola_engine/shader_manager.hpp
        src/Qt_tests/mainwindow.h
        src/Qt_tests/ray_tracer.h
        include/utils/gl3d_scale.h
        include/utils/gl3d_global_param.h
        include/kaola_engine/gl3d_post_process.h
        include/utils/gl3d_utils.h
        include/utils/gl3d_post_process_template.h
        include/kaola_engine/gl3d_general_light_source.h
        #    include/editor/klm_surface.h
        include/editor/bounding_box.h
        src/Qt_tests/drawhomewin.h
        include/editor/vertex.h
        include/utils/gl3d_path_config.h
        src/Qt_tests/ui/pickupdig.h
        include/editor/gl3d_wall.h
        include/editor/gl_utility.h
        include/utils/gl3d_lock.h
        include/utils/gl3d_math.h
        src/Qt_tests/ui/drawoption.h
        src/utils/qui/frmmessagebox.h
        src/utils/qui/iconhelper.h
        src/utils/qui/myhelper.h
        include/editor/gl3d_surface_object.h
        include/kaola_engine/gl3d_abstract_object.h
        include/editor/surface.h
        include/editor/merchandise.h
        include/resource_and_network/klm_resource_manager.h
        include/utils/io_utility.h
        include/resource_and_network/global_material.h
        include/editor/design_scheme.h
        include/editor/command.h
        include/kaola_engine/loading_object.h
        include/editor/gl3d_door.h
        include/editor/gl3d_window.h
        include/editor/sign_config.h
        include/editor/style_package.h
        include/resource_and_network/network_tool.h
        )

include_directories(LIBS/glm_build)
include_directories(include)
include_directories(LIBS/assimp-3.2/include)
include_directories(LIBS/pugixml-1.7/pugixml-1.7/src)
include_directories(LIBS/boost_1_61_0)
include_directories(LIBS/cgal/include)
include_directories(LIBS/cgal_lib/include)
include_directories(LIBS/cgal/auxiliary/gmp/include/)
include_directories(../build-qttest-Desktop_Qt_5_6_1_MSVC2015_64bit-Debug)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
qt5_wrap_ui(UI_HEADERS
        src/Qt_tests/mainwindow.ui
        src/Qt_tests/drawhomewin.ui
        src/utils/qui/frmmessagebox.ui
        ui/window_or_door_selected.ui
        ui/furniture_selected.ui
        )

qt5_add_resources(QRCS res.qrc)
add_executable(${PROJECT_NAME} ${SRCS} ${UI_HEADERS} ${QRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::OpenGL)
