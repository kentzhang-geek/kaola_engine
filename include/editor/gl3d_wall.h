#ifndef GL3D_WALL_H
#define GL3D_WALL_H

#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "kaola_engine/Program.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_render_process.hpp"

// OpenGL in windows should use GLEW
#include "kaola_engine/glheaders.h"

// kaolaengine components
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_general_light_source.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_object.h"
#include "kaola_engine/gl3d_scene.h"

using namespace std;

namespace gl3d {
class gl3d_wall : public object {
public:
    gl3d_wall() : start_point(0.0), end_point(0.0) {}
    gl3d_wall(glm::vec2 s_pt, glm::vec2 e_pt, float t_thickness, float hight);

    // properties
    GL3D_UTILS_PROPERTY(start_point, glm::vec2);
    GL3D_UTILS_PROPERTY(end_point, glm::vec2);
    GL3D_UTILS_PROPERTY(thickness, float);
    GL3D_UTILS_PROPERTY(hight, float);
    GL3D_UTILS_PROPERTY(start_point_fixed, bool);
    GL3D_UTILS_PROPERTY(end_point_fixed, bool);

    void calculate_mesh();

    void get_coord_on_screen(scene * main_scene, glm::vec2 &start_pos, glm::vec2 &end_pos);

    static const float wall_combine_distance = 1.0f;
    static bool combine(gl3d_wall * wall1, gl3d_wall * wall2);

    gl3d::obj_points bottom_pts[4];

    float get_length();
    bool set_length(float len);

private:
    void release_last_data();
    void init();
};
}

#endif // GL3D_WALL_H
