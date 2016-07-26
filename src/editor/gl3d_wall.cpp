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

void gl3d_wall::init() {
    this->get_property()->scale_unit = gl3d::scale::wall;
    this->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;
    this->get_property()->authority = GL3D_OBJ_ENABLE_DEL
            | GL3D_OBJ_ENABLE_CHANGEMTL
            | GL3D_OBJ_ENABLE_PICKING;
}

gl3d_wall::gl3d_wall(glm::vec2 s_pt, glm::vec2 e_pt, float t_thickness, float t_hight) {
    this->init();

    // get point
    this->start_point = s_pt;
    this->end_point = e_pt;
    this->thickness = t_thickness;
    this->hight = t_hight;

    // recalculate meshes
    this->calculate_mesh();
}

void gl3d_wall::release_last_data() {
    for (auto it = this->get_meshes()->begin();
         it != this->get_meshes()->end();
         it++) {
        delete (*it);
    }
    this->set_number_of_meshes(0);
    this->get_meshes()->clear();

    for (auto it = this->get_mtls()->begin();
         it != this->get_mtls()->end();
         it++) {
        delete it.value();
    }
    this->get_mtls()->clear();
    this->set_data_buffered(false);

    return;
}

// 计算坐标
void gl3d_wall::calculate_mesh() {
    // clear last data
    this->release_last_data();

    glm::mat4 rotate_mtx =
            glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec2 tmp = this->end_point - this->start_point;
    glm::vec4 direction =
            glm::vec4(tmp.x, tmp.y, 0.0f, 1.0f);
    direction = rotate_mtx * direction;
    direction = direction / direction.w;
    glm::vec2 top_dir = glm::normalize(glm::vec2(direction));

    glm::vec2 pts[4];
    pts[0] = this->start_point + this->get_thickness() * top_dir / 2.0f;
    pts[1] = this->start_point - this->get_thickness() * top_dir / 2.0f;
    pts[2] = this->end_point + this->get_thickness() * top_dir / 2.0f;
    pts[3] = this->end_point - this->get_thickness() * top_dir / 2.0f;

    glm::vec2 max_bnd = glm::vec2(pts[0]);
    glm::vec2 min_bnd = glm::vec2(pts[0]);
    obj_points p_points[8];
    for (int i = 0; i < 4; i++) {
        p_points[i].vertex_x = pts[i].x;
        p_points[i].vertex_y = 0.0;
        p_points[i].vertex_z = pts[i].y;
        max_bnd = glm::max(pts[i], max_bnd);
        min_bnd = glm::min(pts[i], min_bnd);
    }
    for (int i = 4; i < 8; i++) {
        p_points[i].vertex_x = pts[i-4].x;
        p_points[i].vertex_y = this->hight;
        p_points[i].vertex_z = pts[i-4].y;
        max_bnd = glm::max(pts[i-4], max_bnd);
        min_bnd = glm::min(pts[i-4], min_bnd);
    }
    GLushort p_indexes[36] = {
        0, 1, 2, // bottom
        1, 3, 2,
        4, 5, 6, // top
        5, 7, 6,
        1, 7, 5, // front
        1, 3, 7,
        2, 4, 6, // back
        2, 0, 4,
        0, 5, 4, // left
        0, 1, 5,
        3, 6, 7, // right
        3, 2, 6
    };

    // create mesh and push back
    mesh * p_mesh = new mesh(p_points, 8, p_indexes, 36);
    p_mesh->set_bounding_value_max(
                glm::vec3(max_bnd.x, this->hight, max_bnd.y));
    p_mesh->set_bounding_value_min(
                glm::vec3(min_bnd.x, 0.0, min_bnd.y));
    p_mesh->set_material_index(0);
    p_mesh->recalculate_normals(0.707);
    this->set_number_of_meshes(1);
    this->get_meshes()->push_back(p_mesh);

    // mtl
    gl3d_material * p_mat = new gl3d_material();
    p_mat->colors.insert(p_mat->diffuse, glm::vec3(1.0));
    this->get_property()->authority = GL3D_OBJ_ENABLE_DEL
            | GL3D_OBJ_ENABLE_CHANGEMTL
            | GL3D_OBJ_ENABLE_PICKING;
    this->get_mtls()->insert(0, p_mat);
    this->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;

    this->buffer_data();
}


void gl3d_wall::get_coord_on_screen(glm::vec2 * coord_screen, gl3d::scene * main_scene) {
    glm::vec2 stpos = this->get_start_point();
    glm::vec2 edpos = this->get_end_point();

    glm::mat4 pv = glm::mat4(1.0);
    pv = pv * *main_scene->watcher->get_projection_matrix();
    pv = pv * *main_scene->watcher->get_viewing_matrix();
    glm::mat4 m1 = glm::translate(glm::mat4(1.0), glm::vec3());
}

bool gl3d_wall::process_corner(gl3d_wall * wall1, gl3d_wall * wall2) {

    return false;
}
