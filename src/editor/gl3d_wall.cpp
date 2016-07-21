#include "editor/gl3d_wall.h"

using namespace std;
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


gl3d_wall::gl3d_wall(glm::vec2 s_pt, glm::vec2 e_pt, float t_thickness, float t_hight) {
    // get point
    this->start_point = s_pt;
    this->end_point = e_pt;
    this->thickness = t_thickness;
    this->hight = t_hight;

    // mtl
    gl3d_material * p_mat = new gl3d_material();
    p_mat->colors.insert(p_mat->diffuse, glm::vec3(1.0));
    this->get_property()->authority = GL3D_OBJ_ENABLE_DEL
            | GL3D_OBJ_ENABLE_CHANGEMTL
            | GL3D_OBJ_ENABLE_PICKING
            | GL3D_OBJ_ENABLE_CULLING;
    this->get_mtls()->insert(0, p_mat);
    this->get_property()->draw_authority = GL3D_SCENE_DRAW_WALL;

    // recalculate meshes
    this->calculate_mesh();
    this->buffer_data();
}

// 计算坐标
void gl3d_wall::calculate_mesh() {
    glm::mat4 rotate_mtx =
            glm::rotate(glm::mat4(1.0), -90.0, glm::vec3(0.0, 0.0, 1.0));
    glm::vec2 tmp = this->end_point - this->start_point;
    glm::vec4 direction =
            glm::vec4(tmp.x, tmp.y, 0.0, 1.0);
    direction = rotate_mtx * direction;
    direction = direction / direction.w;
    glm::vec2 top_dir = glm::normalize(glm::vec2(direction));

    glm::vec3 pts[4];
    pts[0] = this->start_point + this->get_thickness() * top_dir / 2.0;
    pts[1] = this->start_point - this->get_thickness() * top_dir / 2.0;
    pts[2] = this->end_point + this->get_thickness() * top_dir / 2.0;
    pts[3] = this->end_point - this->get_thickness() * top_dir / 2.0;

    for (int i = 0; i < 4; i++) {
        pts[i].z = pts[i].y;
    }

    obj_points * p_points = (obj_points *)malloc(sizeof(obj_points) * 8);
    for (int i = 0; i < 4; i++) {
        p_points->vertex_x = pts[i].x;
        p_points->vertex_y = 0.0;
        p_points->vertex_z = pts[i].z;
    }
    for (int i = 4; i < 8; i++) {
        p_points->vertex_x = pts[i].x;
        p_points->vertex_y = this->hight;
        p_points->vertex_z = pts[i].z;
    }
}
