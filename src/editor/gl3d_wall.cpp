#include "utils/gl3d_math.h"
#include "editor/gl3d_wall.h"
#include "utils/gl3d_lock.h"
#include "editor/surface.h"

using namespace std;
using namespace gl3d;
using namespace klm;

static const float wall_combine_distance = 1.0f;

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
    this->set_render_authority(GL3D_SCENE_DRAW_NORMAL);
    this->set_control_authority(GL3D_OBJ_ENABLE_DEL
                                | GL3D_OBJ_ENABLE_CHANGEMTL
                                | GL3D_OBJ_ENABLE_PICKING);
    this->start_point_fixed = false;
    this->end_point_fixed = false;
    this->fixed = false;
    this->set_obj_type(this->type_wall);
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

gl3d_wall::~gl3d_wall() {
    if (this->start_point_fixed) {
        this->seperate(this->start_point_attach);
    }

    if (this->end_point_fixed) {
        this->seperate(this->end_point_attach);
    }
}

glm::vec2 gl3d_wall::get_start_point() {
    return this->start_point;
}

void gl3d_wall::set_start_point(glm::vec2 st_tag) {
    if (!this->fixed) {
        this->start_point = st_tag;
    }
}

glm::vec2 gl3d_wall::get_end_point() {
    return this->end_point;
}

void gl3d_wall::set_end_point(glm::vec2 end_point_tag) {
    if (!this->fixed) {
        this->end_point = end_point_tag;
    }
}

void gl3d_wall::seperate(gl3d::gl3d_wall_attach & attachment) {
    gl3d_wall * attach = attachment.attach;
    if (attachment.attach_point == gl3d::gl3d_wall_attach::start_point) {
        // 附着墙相应位置在start点
        attach->start_point_attach.attach = NULL;
        attach->start_point_fixed = false;
    }
    else {
        // 附着墙相应位置在end点
        attach->end_point_attach.attach = NULL;
        attach->end_point_fixed = false;
    }
}

void gl3d_wall::release_last_data() {
    for (auto it = this->get_sfcs()->begin();
         it != this->get_sfcs()->end();
         it++) {
        delete (*it);
    }
    this->get_sfcs()->clear();
    this->set_number_of_meshes(0);
    this->get_meshes()->clear();
    this->get_mtls()->clear();
    this->set_data_buffered(false);

    return;
}

void gl3d_wall::set_thickness(float thickness_tag) {
    if (this->fixed) {
        return;
    }
    this->thickness = thickness_tag;
    if (this->start_point_fixed) {
        this->start_point_attach.attach->calculate_mesh();
    }
    if (this->end_point_fixed) {
        this->end_point_attach.attach->calculate_mesh();
    }
}

float gl3d_wall::get_thickness() {
    return this->thickness;
}

// 计算坐标
void gl3d_wall::calculate_mesh() {
    if (glm::length(this->end_point - this->start_point) < this->get_thickness()) {
        return;
    }
    if (glm::length(this->end_point - this->start_point) < 0.01f) {
        return;
    }
    if (!gl3d_lock::shared_instance()->wall_lock.tryLock()) {
        // lock wall calculor
        return;
    }

    glm::mat4 rotate_mtx =
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec2 tmp = this->end_point - this->start_point;
    glm::vec4 direction =
            glm::vec4(tmp.x, tmp.y, 0.0f, 1.0f);
    direction = rotate_mtx * direction;
    direction = direction / direction.w;
    glm::vec2 top_dir = glm::normalize(glm::vec2(direction));

    gl3d::math::line_2d l_left(this->start_point + top_dir * this->thickness / 2.0f,
                               this->end_point + top_dir * this->thickness / 2.0f);
    gl3d::math::line_2d l_right(this->start_point - top_dir * this->thickness / 2.0f,
                                this->end_point - top_dir * this->thickness / 2.0f);
    bool cal_other_wall = true;
    if (glm::min(l_left.length(), l_right.length()) <
            glm::max(0.01f, this->get_thickness())) {
        cal_other_wall = false;
    }


    // 根据attach状态重算左右边线
    glm::vec2 tmp_vec = this->end_point - this->start_point;
    if (this->start_point_fixed && cal_other_wall) {
        // 计算附着墙的左右边线
        gl3d_wall * tmp_wall = this->start_point_attach.attach;
        tmp = tmp_wall->end_point - tmp_wall->start_point;
        // check another wall length
        if ((glm::length(tmp) > 0.0001)
                && (glm::length(tmp) > tmp_wall->get_thickness())
                && (glm::abs(glm::dot(glm::normalize(tmp), glm::normalize(tmp_vec))) < 0.9)) {
            direction = glm::vec4(tmp.x, tmp.y, 0.0f, 1.0f);
            direction = rotate_mtx * direction;
            direction = direction / direction.w;
            top_dir = glm::normalize(glm::vec2(direction));
            gl3d::math::line_2d tl_left(tmp_wall->start_point + top_dir * tmp_wall->thickness / 2.0f,
                                        tmp_wall->end_point + top_dir * tmp_wall->thickness / 2.0f);
            gl3d::math::line_2d tl_right(tmp_wall->start_point - top_dir * tmp_wall->thickness / 2.0f,
                                         tmp_wall->end_point - top_dir * tmp_wall->thickness / 2.0f);

            // 重算left和right的a点
            if (this->start_point_attach.attach_point == gl3d::gl3d_wall_attach::start_point) {
                // 附加到另外墙的start时
                glm::vec2 pt;
                if (gl3d::math::get_cross(l_left, tl_right, pt)) {
                    l_left.a = pt;
                }
                if (gl3d::math::get_cross(l_right, tl_left, pt)) {
                    l_right.a = pt;
                }
            }
            else {
                glm::vec2 pt;
                if (gl3d::math::get_cross(l_left, tl_left, pt)) {
                    l_left.a = pt;
                }
                if (gl3d::math::get_cross(l_right, tl_right, pt)) {
                    l_right.a = pt;
                }
            }
        }
    }
    if (this->end_point_fixed && cal_other_wall) {
        // 重算b点
        // 计算附着墙的左右边线
        gl3d_wall * tmp_wall = this->end_point_attach.attach;
        tmp = tmp_wall->end_point - tmp_wall->start_point;
        // check another wall length
        if ((glm::length(tmp) > 0.0001)
                && (glm::length(tmp) > tmp_wall->get_thickness())
                && (glm::abs(glm::dot(glm::normalize(tmp), glm::normalize(tmp_vec))) < 0.9)) {
            direction = glm::vec4(tmp.x, tmp.y, 0.0f, 1.0f);
            direction = rotate_mtx * direction;
            direction = direction / direction.w;
            top_dir = glm::normalize(glm::vec2(direction));
            gl3d::math::line_2d tl_left(tmp_wall->start_point + top_dir * tmp_wall->thickness / 2.0f,
                                        tmp_wall->end_point + top_dir * tmp_wall->thickness / 2.0f);
            gl3d::math::line_2d tl_right(tmp_wall->start_point - top_dir * tmp_wall->thickness / 2.0f,
                                         tmp_wall->end_point - top_dir * tmp_wall->thickness / 2.0f);

            // 重算left和right的a点
            if (this->end_point_attach.attach_point == gl3d::gl3d_wall_attach::start_point) {
                // 附加到另外墙的start时
                glm::vec2 pt;
                if (gl3d::math::get_cross(l_left, tl_left, pt))
                    l_left.b = pt;
                if (gl3d::math::get_cross(l_right, tl_right, pt))
                    l_right.b = pt;
            }
            else {
                glm::vec2 pt;
                if (gl3d::math::get_cross(l_left, tl_right, pt))
                    l_left.b = pt;
                if (gl3d::math::get_cross(l_right, tl_left, pt))
                    l_right.b = pt;
            }
        }
    }

    // clear last data
    this->release_last_data();

    // 根据左右边线得出surface
    QVector<glm::vec3 > points;
    klm::Surface * s;
    // left
    points.clear();
    points.push_back(glm::vec3(l_left.b.x, 0.0f, l_left.b.y));
    points.push_back(glm::vec3(l_left.a.x, 0.0f, l_left.a.y));
    points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
    points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
    s = new klm::Surface(points);
    this->sfcs.push_back(s);
    // right
    points.clear();
    points.push_back(glm::vec3(l_right.a.x, 0.0f, l_right.a.y));
    points.push_back(glm::vec3(l_right.b.x, 0.0f, l_right.b.y));
    points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
    points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
    s = new klm::Surface(points);
    this->sfcs.push_back(s);
    // back
    points.clear();
    points.push_back(glm::vec3(l_left.a.x, 0.0f, l_left.a.y));
    points.push_back(glm::vec3(l_right.a.x, 0.0f, l_right.a.y));
    points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
    points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
    this->sfcs.push_back(new klm::Surface(points));
    // front
    points.clear();
    points.push_back(glm::vec3(l_right.b.x, 0.0f, l_right.b.y));
    points.push_back(glm::vec3(l_left.b.x, 0.0f, l_left.b.y));
    points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
    points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
    this->sfcs.push_back(new klm::Surface(points));
    // top
    points.clear();
    points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
    points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
    points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
    points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
    this->sfcs.push_back(new klm::Surface(points));

    // will not fill meshes now

    gl3d_lock::shared_instance()->wall_lock.unlock();
    return;
}


void gl3d_wall::get_coord_on_screen(gl3d::scene * main_scene,
                                    glm::vec2 & start_pos,
                                    glm::vec2 &end_pos) {
    glm::vec2 stpos = this->get_start_point();
    glm::vec2 edpos = this->get_end_point();

    glm::mat4 pvm = glm::mat4(1.0);
    pvm = pvm * *main_scene->watcher->get_projection_matrix();
    pvm = pvm * *main_scene->watcher->get_viewing_matrix();

    // set model matrix
    ::glm::mat4 trans = this->get_translation_mat() *
            this->get_rotation_mat() *
            this->get_scale_mat();
    // set norMtx
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)) * trans;
    pvm = pvm * trans;

    glm::vec4 coord_out;
    coord_out = pvm * this->get_property()->rotate_mat * glm::vec4(stpos.x, 0.0f, stpos.y, 1.0f);
    coord_out = (coord_out + 1.0f)/2.0f;
    start_pos = glm::vec2(coord_out.x, coord_out.y);
    start_pos.x = start_pos.x * main_scene->get_width();
    start_pos.y = main_scene->get_height() - start_pos.y * main_scene->get_height();
    coord_out = pvm * this->get_property()->rotate_mat * glm::vec4(edpos.x, 0.0f, edpos.y, 1.0f);
    coord_out = (coord_out + 1.0f)/2.0f;
    end_pos = glm::vec2(coord_out.x, coord_out.y);
    end_pos.x = end_pos.x * main_scene->get_width();
    end_pos.y = main_scene->get_height() - end_pos.y * main_scene->get_height();
}

bool gl3d_wall::combine(gl3d_wall * wall1, gl3d_wall * wall2, glm::vec2 combine_point) {
    float dis = glm::length(combine_point - wall2->start_point);
    dis = glm::min(dis, (glm::length(wall1->start_point - combine_point)));
    dis = glm::min(dis, (glm::length(combine_point - wall2->end_point)));
    dis = glm::min(dis, (glm::length(wall1->end_point - combine_point)));
    if (dis > wall_combine_distance) { // check is there near points
        return false;
    }

    // attach wall1
    float st_dis_tmp = glm::length(wall1->start_point - combine_point);
    float ed_dis_tmp = glm::length(wall1->end_point - combine_point);
    if (st_dis_tmp > ed_dis_tmp) {
        // 分离之前的attach
        if (wall1->end_point_fixed == true) {
            wall1->seperate(wall1->end_point_attach);
        }
        // 建立新的attach
        wall1->end_point_fixed = true;
        wall1->end_point_attach.attach = wall2;
        if (glm::length(combine_point - wall2->end_point) >=
                glm::length(combine_point - wall2->start_point)) {
            wall1->end_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall1->end_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }
    else {
        // 分离之前的attach
        if (wall1->start_point_fixed == true) {
            wall1->seperate(wall1->start_point_attach);
        }
        // 建立新的attach
        wall1->start_point_fixed = true;
        wall1->start_point_attach.attach = wall2;
        if (glm::length(combine_point - wall2->end_point) >
                glm::length(combine_point - wall2->start_point)) {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }

    // attach wall2
    st_dis_tmp = glm::length(wall2->start_point - combine_point);
    ed_dis_tmp = glm::length(wall2->end_point - combine_point);
    if (st_dis_tmp > ed_dis_tmp) {
        // 分离之前的attach
        if (wall2->end_point_fixed == true) {
            wall2->seperate(wall2->end_point_attach);
        }
        // 建立新的attach
        wall2->end_point_fixed = true;
        wall2->end_point_attach.attach = wall1;
        if (glm::length(combine_point - wall1->end_point) >=
                glm::length(combine_point - wall1->start_point)) {
            wall2->end_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall2->end_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }
    else {
        // 分离之前的attach
        if (wall2->start_point_fixed == true) {
            wall2->seperate(wall2->start_point_attach);
        }
        // 建立新的attach
        wall2->start_point_fixed = true;
        wall2->start_point_attach.attach = wall1;
        if (glm::length(combine_point - wall1->end_point) >
                glm::length(combine_point - wall1->start_point)) {
            wall2->start_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall2->start_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }

    wall1->calculate_mesh();
    wall2->calculate_mesh();
    return true;
}


bool gl3d_wall::combine(gl3d_wall * wall1, gl3d_wall * wall2, tag_combine_traits combine_traits) {

    // attach wall1
    if ((combine_traits == gl3d_wall::combine_wall1_end_to_wall2_end) ||
            (combine_traits == gl3d_wall::combine_wall1_end_to_wall2_start)) {
        // 分离之前的attach
        if (wall1->end_point_fixed == true) {
            wall1->seperate(wall1->end_point_attach);
        }
        // 建立新的attach
        wall1->end_point_fixed = true;
        wall1->end_point_attach.attach = wall2;
        if (combine_traits == gl3d_wall::combine_wall1_end_to_wall2_start) {
            wall1->end_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall1->end_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }
    else {
        // 分离之前的attach
        if (wall1->start_point_fixed == true) {
            wall1->seperate(wall1->start_point_attach);
        }
        // 建立新的attach
        wall1->start_point_fixed = true;
        wall1->start_point_attach.attach = wall2;
        if (combine_traits == gl3d_wall::combine_wall1_start_to_wall2_start) {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }

    // attach wall2
    if ((combine_traits == gl3d_wall::combine_wall1_end_to_wall2_end) ||
            (combine_traits == gl3d_wall::combine_wall1_start_to_wall2_end)) {
        // 分离之前的attach
        if (wall2->end_point_fixed == true) {
            wall2->seperate(wall2->end_point_attach);
        }
        // 建立新的attach
        wall2->end_point_fixed = true;
        wall2->end_point_attach.attach = wall1;
        if (combine_traits == gl3d_wall::combine_wall1_start_to_wall2_end) {
            wall2->end_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall2->end_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }
    else {
        // 分离之前的attach
        if (wall2->start_point_fixed == true) {
            wall2->seperate(wall2->start_point_attach);
        }
        // 建立新的attach
        wall2->start_point_fixed = true;
        wall2->start_point_attach.attach = wall1;
        if (combine_traits == gl3d_wall::combine_wall1_start_to_wall2_start) {
            wall2->start_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall2->start_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }

    wall1->calculate_mesh();
    wall2->calculate_mesh();
    return true;
}

float gl3d_wall::get_length() {
    glm::vec2 v = this->end_point - this->start_point;
    return glm::length(v);
}

bool gl3d_wall::set_length(float len) {
    if (this->fixed) {
        return false;
    }
    if (this->start_point_fixed && this->end_point_fixed) {
        return false; // both two side fixed , so set length failed
    }

    if (!this->end_point_fixed) {
        // start point fixed , change end point
        glm::vec2 dir = this->end_point - this->start_point;
        dir = glm::normalize(dir);
        this->end_point = this->start_point + len * dir;
    }
    else {
        // end point fixed , change start point
        glm::vec2 dir = this->start_point - this->end_point;
        dir = glm::normalize(dir);
        this->start_point = this->end_point + len * dir;
    }

    this->calculate_mesh();
    return true;
}

#define BOUND_MAX_DEFAULT 1000.0f
#define BOUND_MIN_DEFAULT 0.0f
void gl3d::surface_to_mesh(klm::Surface * sfc,
                           QVector<gl3d::mesh *> &vct) {
    // process local verticles
    // create vertex buffer
    glm::vec3 bnd_max(BOUND_MIN_DEFAULT);
    glm::vec3 bnd_min(BOUND_MAX_DEFAULT);
    GLfloat * tmp_data = NULL;
    int pts_len;
    gl3d::obj_points * pts = NULL;

    const QVector<Surface::Vertex * > * vertexes = sfc->getRenderingVertices();
    pts_len = vertexes->size();
    pts = new gl3d::obj_points[pts_len];
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        glm::vec3 tmp_vert = glm::vec3(
                    vertexes->at(i)->x(),
                    vertexes->at(i)->y(),
                    vertexes->at(i)->z()
                    );
        bnd_max = glm::max(tmp_vert, bnd_max);
        bnd_min = glm::min(tmp_vert, bnd_min);
        pts[i].vertex_x = tmp_vert.x;
        pts[i].vertex_y = tmp_vert.y;
        pts[i].vertex_z = tmp_vert.z;
        pts[i].texture_x = vertexes->at(i)->w();
        pts[i].texture_y = 1.0f - vertexes->at(i)->h();
    }

    // create index buffer
    GLushort * idxes = NULL;
    int idx_len;
    const QVector<GLushort> * indecis = sfc->getRenderingIndices();
    idx_len = indecis->size();
    idxes = new GLushort[idx_len];
    for (int j = 0; j < indecis->size(); j++) {
        idxes[j] = indecis->at(j);
    }

    // create new mesh
    gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
    m->set_material_index(0);
    m->set_bounding_value_max(bnd_max);
    m->set_bounding_value_min(bnd_min);
    m->set_texture_repeat(true);
    vct.push_back(m);

    // clean env
    delete pts;
    delete idxes;

    // process conective surface
    // have conective surface , then process meshes
    if (sfc->isConnectiveSurface()) {
        vertexes = sfc->getConnectiveVerticies();
        indecis = sfc->getConnectiveIndicies();
        bnd_max = glm::vec3(BOUND_MIN_DEFAULT);
        bnd_min = glm::vec3(BOUND_MAX_DEFAULT);

        pts_len = vertexes->size();
        idx_len = indecis->size();
        pts = new obj_points[pts_len];
        idxes = new GLushort[idx_len];
        memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
        memset(idxes, 0, sizeof(GLushort) * idx_len);
        for (int j = 0; j < vertexes->size(); j++) {
            glm::vec3 tmp_vert = glm::vec3(
                        vertexes->at(j)->x(),
                        vertexes->at(j)->y(),
                        vertexes->at(j)->z()
                        );
            bnd_max = glm::max(tmp_vert, bnd_max);
            bnd_min = glm::min(tmp_vert, bnd_min);
            pts[j].vertex_x = tmp_vert.x;
            pts[j].vertex_y = tmp_vert.y;
            pts[j].vertex_z = tmp_vert.z;
            pts[j].texture_x = vertexes->at(j)->w();
            pts[j].texture_y = 1.0f - vertexes->at(j)->h();
        }
        for (int j = 0; j < indecis->size(); j++) {
            idxes[j] = indecis->at(j);
        }
        // create new mesh
        gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
        m->set_material_index(0);
        m->set_bounding_value_max(bnd_max);
        m->set_bounding_value_min(bnd_min);
        m->set_texture_repeat(true);
        vct.push_back(m);
        // clean env
        delete pts;
        delete idxes;
    }

    // process sub surface
    if (sfc->getSurfaceCnt() > 0) {
        for (int i = 0; i < sfc->getSurfaceCnt(); i++) {
            surface_to_mesh(sfc->getSubSurface(i), vct);
        }
    }
    return;
}


gl3d::gl3d_wall_attach::gl3d_wall_attach() {
    this->attach_point = this->start_point;
    this->attach = NULL;
}

bool gl3d_wall::is_data_changed() {
    return true;
}

bool gl3d_wall::is_visible() {
    return true;
}

glm::mat4 gl3d_wall::get_translation_mat() {
    return object::get_translation_mat();
}

glm::mat4 gl3d_wall::get_rotation_mat() {
    return object::get_rotation_mat();
}

glm::mat4 gl3d_wall::get_scale_mat() {
    return object::get_scale_mat();
}

void gl3d_wall::get_abstract_meshes(QVector<gl3d::mesh *> & ms) {
    for (auto it = this->sfcs.begin();
         it != this->sfcs.end();
         it++) {
        gl3d::surface_to_mesh(*it, ms);
    }
}

void gl3d_wall::get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) {
    static gl3d_material * mtl = new gl3d_material("_7.jpg");
    mt.insert(0, mtl);
    mt.insert(1, mtl);
    mt.insert(2, mtl);
    mt.insert(3, mtl);
    mt.insert(4, mtl);
    mt.insert(5, mtl);
    mt.insert(6, mtl);
}

void gl3d_wall::set_translation_mat(const glm::mat4 & trans) {
    object::set_translation_mat(trans);
    return;
}

void gl3d_wall::clear_abstract_meshes(QVector<gl3d::mesh *> & ms) {
    for (auto it = ms.begin();
         it != ms.end();
         it++) {
        delete (*it);
    }
    ms.clear();
}

void gl3d_wall::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) {
    mt.clear();
}

// get triangles from surfaces
static void get_faces_from_surface(klm::Surface *sfc, QVector<math::triangle_facet> &faces) {
    for (int i = 0; i < sfc->getSurfaceCnt(); i++) {
        get_faces_from_surface(sfc->getSubSurface(i), faces);
    }

    // process local verticles
    const QVector<Surface::Vertex * > * vertexes = sfc->getRenderingVertices();
    const QVector<GLushort> * indecis = sfc->getRenderingIndices();
    for (int i = 0; i < (indecis->size() / 3); i++) {
        GLushort b0 = indecis->at(i * 3 + 0);
        GLushort b1 = indecis->at(i * 3 + 1);
        GLushort b2 = indecis->at(i * 3 + 2);

        glm::vec3 pta = glm::vec3(vertexes->at(b0)->x(),
                                  vertexes->at(b0)->y(),
                                  vertexes->at(b0)->z());
        glm::vec3 ptb = glm::vec3(vertexes->at(b1)->x(),
                                  vertexes->at(b1)->y(),
                                  vertexes->at(b1)->z());
        glm::vec3 ptc = glm::vec3(vertexes->at(b2)->x(),
                                  vertexes->at(b2)->y(),
                                  vertexes->at(b2)->z());
        faces.push_back(math::triangle_facet(pta, ptb, ptc));
    }

    // process conective surface
    // have conective surface , then process meshes
    if (sfc->isConnectiveSurface()) {
        vertexes = sfc->getConnectiveVerticies();
        indecis = sfc->getConnectiveIndicies();
        for (int i = 0; i < (indecis->size() / 3); i++) {
            GLushort b0 = indecis->at(i * 3 + 0);
            GLushort b1 = indecis->at(i * 3 + 1);
            GLushort b2 = indecis->at(i * 3 + 2);

            glm::vec3 pta = glm::vec3(vertexes->at(b0)->x(),
                                      vertexes->at(b0)->y(),
                                      vertexes->at(b0)->z());
            glm::vec3 ptb = glm::vec3(vertexes->at(b1)->x(),
                                      vertexes->at(b1)->y(),
                                      vertexes->at(b1)->z());
            glm::vec3 ptc = glm::vec3(vertexes->at(b2)->x(),
                                      vertexes->at(b2)->y(),
                                      vertexes->at(b2)->z());
            faces.push_back(math::triangle_facet(pta, ptb, ptc));
        }
    }

    return;
}

// calculate points which line cross triangles
typedef QPair<glm::vec3, glm::vec3> point_and_normal;
using namespace gl3d::math;
static void cast_ray_to_faces(QVector<math::triangle_facet> &faces,
                              QVector<point_and_normal> &crosses,
                              math::line_3d ray_cast) {
    for (QVector<math::triangle_facet>::iterator it = faces.begin();
         it != faces.end();
         it++) {
        glm::vec3 pt;
        if (math::line_cross_facet(*it, ray_cast, pt)) {
            if (it->is_point_in_facet(pt)) {
                glm::vec3 nor;
                nor = glm::cross(it->c - it->b, it->a - it->b);
                nor = glm::normalize(nor);
                crosses.push_back(point_and_normal(pt, nor));
            }
        }
    }

    return;
}

bool gl3d_wall::get_coord_on_wall(scene *sce,
                                  glm::vec2 coord_on_screen,
                                  glm::vec3 &out_point_on_wall,
                                  glm::vec3 &out_point_normal) {
    if (this->sfcs.size() <= 0) {
        return false;
    }
    gl3d::viewer * cam = sce->watcher;
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    coord_on_screen.y = cam->get_height() - coord_on_screen.y;
    glm::vec3 coord_in = glm::vec3(coord_on_screen.x, coord_on_screen.y, 0.0);
    coord_in.z = 1.0;
    glm::vec3 txxx = glm::unProject(coord_in,
                                    glm::mat4(1.0),
                                    (*cam->get_projection_matrix()) *
                                    (*cam->get_viewing_matrix())
                                    * ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)),
                                    glm::vec4(0.0, 0.0,
                                              cam->get_width(),
                                              cam->get_height()));
    coord_in.z = 0.1;
    txxx = glm::unProject(coord_in,
                                    glm::mat4(1.0),
                                    (*cam->get_projection_matrix()) *
                                    (*cam->get_viewing_matrix())
                                    * ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)),
                                    glm::vec4(0.0, 0.0,
                                              cam->get_width(),
                                              cam->get_height()));

    glm::vec3 near_pt = cam->get_current_position();
    txxx = glm::normalize(txxx);
    math::line_3d ray_cast(near_pt, near_pt + txxx);

    QVector<math::triangle_facet> faces;
    QVector<point_and_normal> crosses;
    glm::vec3 target_pt;
    glm::vec3 o_nor;
    float dis_to_pt = 0.0f;
    faces.clear();
    crosses.clear();

    // get all the facet
    for (auto it = this->sfcs.begin();
         it != this->sfcs.end();
         it++) {
        get_faces_from_surface(*it, faces);
    }

    // get all cross points
    cast_ray_to_faces(faces, crosses, ray_cast);

    // check out real cross
    if (crosses.size() <  0) {
        return false;
    }
    dis_to_pt = glm::length(crosses.at(0).first - near_pt);
    target_pt = crosses.at(0).first;
    for (auto it = crosses.begin();
         it != crosses.end();
         it++) {
        if (glm::length((*it).first - near_pt) < dis_to_pt) {
            // if has nearer point
            dis_to_pt = glm::length((*it).first - near_pt);
            target_pt = (*it).first;
            // and its normal
            o_nor = it->second;
        }
    }

    out_point_on_wall = target_pt;
    out_point_normal = o_nor;

    return true;
}
