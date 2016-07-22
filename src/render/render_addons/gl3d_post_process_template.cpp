#include "utils/gl3d_post_process_template.h"

using namespace gl3d;

static gl3d::obj_points post_rect[4] = {
    {-1.0, 1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
     0.0, 1.0,     // text
     0},
    {-1.0, -1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
     0.0, 0.0,     // text
     0},
    {1.0, 1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
     1.0, 1.0,     // text
     0},
    {1.0, -1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
     1.0, 0.0,     // text
     0},
};

static GLushort post_indexes[6] = {
    0, 2, 1,
    1, 2, 3,
};

special_obj::special_obj(gl3d_general_texture * p_tex) :
    object(post_rect, 4, post_indexes, 6) {
    gl3d_material * p_mat = new gl3d_material(p_tex);
    this->get_meshes()->at(0)->set_bounding_value_max(glm::vec3(1.0, 1.0, 0.0));
    this->get_meshes()->at(0)->set_bounding_value_max(glm::vec3(-1.0, -1.0, 0.0));
    this->get_meshes()->at(0)->set_material_index(0);
    this->get_property()->authority = GL3D_OBJ_ENABLE_DEL;
    this->get_mtls()->insert(0, p_mat);
    this->get_property()->draw_authority = GL3D_SCENE_DRAW_SPECIAL;
    this->get_property()->scale_unit = gl3d::scale::special;
}

