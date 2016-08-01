#include "utils/gl3d_math.h"
#include "editor/gl3d_wall.h"
#include "utils/gl3d_lock.h"
#include "editor/klm_surface.h"

using namespace std;
using namespace gl3d;

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
    if (!gl3d_lock::shared_instance()->wall_lock.tryLock()) {
        // lock wall calculor
        return;
    }
    // clear last data
    this->release_last_data();

    if (glm::length(this->start_point - this->end_point) < 0.01f) {
        // no wall drawed so do not calculate data
        gl3d_lock::shared_instance()->wall_lock.unlock();
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

    // 根据attach状态重算左右边线
    if (this->start_point_fixed) {
        // 计算附着墙的左右边线
        gl3d_wall * tmp_wall = this->start_point_attach.attach;
        tmp = tmp_wall->end_point - tmp_wall->start_point;
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
            if (gl3d::math::get_cross(l_left, tl_right, pt))
                l_left.a = pt;
            if (gl3d::math::get_cross(l_right, tl_left, pt))
                l_right.a = pt;
        }
        else {
            glm::vec2 pt;
            if (gl3d::math::get_cross(l_left, tl_left, pt))
                l_left.a = pt;
            if (gl3d::math::get_cross(l_right, tl_right, pt))
                l_right.a = pt;
        }
    }
    if (this->end_point_fixed) {
        // 重算b点
        // 计算附着墙的左右边线
        gl3d_wall * tmp_wall = this->end_point_attach.attach;
        tmp = tmp_wall->end_point - tmp_wall->start_point;
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
            if (gl3d::math::get_cross(l_left, l_right, pt))
                l_left.b = pt;
            if (gl3d::math::get_cross(l_right, tl_left, pt))
                l_right.b = pt;
        }
    }

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
    s->setDebug();
    this->sfcs.push_back(s);
    // right
    points.clear();
    points.push_back(glm::vec3(l_right.a.x, 0.0f, l_right.a.y));
    points.push_back(glm::vec3(l_right.b.x, 0.0f, l_right.b.y));
    points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
    points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
    s = new klm::Surface(points);
    s->setDebug();
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

    // add all the mesh from surface
    QVector<gl3d::mesh *> mss;
    for (auto it = this->sfcs.begin();
         it != this->sfcs.end();
         it++) {
        gl3d::surface_to_mesh(*it, mss);
    }
    for (auto its = mss.begin(); its != mss.end(); its++) {
        (*its)->recalculate_normals(0.707);
        (*its)->set_material_index(0);
        this->get_meshes()->push_back(*its);
    }
    this->set_number_of_meshes(this->get_meshes()->size());

    // mtl
    gl3d_material * p_mat = new gl3d_material();
    p_mat->colors.insert(p_mat->diffuse, glm::vec3(1.0));
    p_mat->colors.insert(p_mat->ambient, glm::vec3(1.0));
    this->set_control_authority(GL3D_OBJ_ENABLE_DEL
            | GL3D_OBJ_ENABLE_CHANGEMTL
            | GL3D_OBJ_ENABLE_PICKING);
    this->get_mtls()->insert(0, p_mat);
    this->set_render_authority(GL3D_SCENE_DRAW_NORMAL);

    this->buffer_data();

    gl3d_lock::shared_instance()->wall_lock.unlock();
    return;
}


void gl3d_wall::get_coord_on_screen(gl3d::scene * main_scene,
                                    glm::vec2 & start_pos,
                                    glm::vec2 &end_pos) {
    glm::vec2 stpos = this->get_start_point();
    glm::vec2 edpos = this->get_end_point();

    glm::mat4 pv = glm::mat4(1.0);
    pv = pv * *main_scene->watcher->get_projection_matrix();
    pv = pv * *main_scene->watcher->get_viewing_matrix();

    glm::vec4 coord_out;
    coord_out = pv * this->get_property()->rotate_mat * glm::vec4(stpos.x, stpos.y, 0.0f, 1.0f);
    start_pos = glm::vec2(coord_out.x, coord_out.y);
    coord_out = pv * this->get_property()->rotate_mat * glm::vec4(edpos.x, edpos.y, 0.0f, 1.0f);
    end_pos = glm::vec2(coord_out.x, coord_out.y);
}

bool gl3d_wall::combine(gl3d_wall * wall1, gl3d_wall * wall2) {
    float dis = glm::length(wall1->start_point - wall2->start_point);
    dis = glm::min(dis, (glm::length(wall1->start_point - wall2->end_point)));
    dis = glm::min(dis, (glm::length(wall1->end_point - wall2->end_point)));
    dis = glm::min(dis, (glm::length(wall1->end_point - wall2->start_point)));
    if (dis > wall_combine_distance) { // check is there near points
        return false;
    }

    // attach wall1
    float st_dis_tmp = glm::min(glm::length(wall1->start_point - wall2->end_point),
                                glm::length(wall1->start_point - wall2->start_point));
    float ed_dis_tmp = glm::min(glm::length(wall1->end_point - wall2->end_point),
                                glm::length(wall1->end_point - wall2->start_point));
    if (st_dis_tmp > ed_dis_tmp) {
        // 分离之前的attach
        if (wall1->end_point_fixed == true) {
            wall1->seperate(wall1->end_point_attach);
        }
        // 建立新的attach
        wall1->end_point_fixed = true;
        wall1->end_point_attach.attach = wall2;
        if (glm::length(wall1->end_point - wall2->end_point) >
                glm::length(wall1->end_point - wall2->start_point)) {
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
        if (glm::length(wall1->start_point - wall2->end_point) >
                glm::length(wall1->start_point - wall2->start_point)) {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::start_point;
        }
        else {
            wall1->start_point_attach.attach_point = gl3d::gl3d_wall_attach::end_point;
        }
    }

    // attach wall2
    st_dis_tmp = glm::min(glm::length(wall2->start_point - wall1->end_point),
                          glm::length(wall2->start_point - wall1->start_point));
    ed_dis_tmp = glm::min(glm::length(wall2->end_point - wall1->end_point),
                          glm::length(wall2->end_point - wall1->start_point));
    if (st_dis_tmp > ed_dis_tmp) {
        // 分离之前的attach
        if (wall2->end_point_fixed == true) {
            wall2->seperate(wall2->end_point_attach);
        }
        // 建立新的attach
        wall2->end_point_fixed = true;
        wall2->end_point_attach.attach = wall1;
        if (glm::length(wall2->end_point - wall1->end_point) >
                glm::length(wall2->end_point - wall1->start_point)) {
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
        if (glm::length(wall2->start_point - wall1->end_point) >
                glm::length(wall2->start_point - wall1->start_point)) {
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
void gl3d::surface_to_mesh(const klm::Surface * sfc,
                           QVector<gl3d::mesh *> &vct) {
    // process local verticles
    // get trans mat
    glm::mat4 trans_mat(1.0);
    sfc->getTransFromParent(trans_mat);

    // create vertex buffer
    glm::vec3 bnd_max(BOUND_MIN_DEFAULT);
    glm::vec3 bnd_min(BOUND_MAX_DEFAULT);
    GLfloat * tmp_data = NULL;
    int pts_len;
    gl3d::obj_points * pts = NULL;
    sfc->getRenderingVertices(tmp_data, pts_len);
    pts_len = pts_len / klm::Vertex::VERTEX_SIZE;
    pts = (gl3d::obj_points *)
            malloc(sizeof(gl3d::obj_points) * pts_len);
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        glm::vec4 tmp_vert = glm::vec4(
                    tmp_data[i * 5 + 0],
                tmp_data[i * 5 + 1],
                tmp_data[i * 5 + 2],
                1.0f);
        tmp_vert = trans_mat * tmp_vert;
        tmp_vert = tmp_vert / tmp_vert.w;
        bnd_max = glm::max(glm::vec3(tmp_vert), bnd_max);
        bnd_min = glm::min(glm::vec3(tmp_vert), bnd_min);
        pts[i].vertex_x = tmp_vert.x;
        pts[i].vertex_y = tmp_vert.y;
        pts[i].vertex_z = tmp_vert.z;
        pts[i].texture_x = tmp_data[i * 5 + 3];
        pts[i].texture_y = 1.0f - tmp_data[i * 5 + 4];
    }

    // create index buffer
    GLushort * idxes = NULL;
    int idx_len;
    sfc->getRenderingIndicies(idxes, idx_len);

    if (idxes == NULL) {
        cout << "aaaaa";

    }

    // create new mesh
    gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
    m->set_material_index(0);
    m->set_bounding_value_max(bnd_max);
    m->set_bounding_value_min(bnd_min);
    vct.push_back(m);

    // clean env
    free(pts);
    free(idxes);

    // process conective surface
    QVector<klm::Vertex *> vertexes;
    QVector<GLushort> indecis;
    sfc->getConnectiveVerticies(vertexes);
    sfc->getConnectiveIndicies(indecis);
    trans_mat = glm::mat4(1.0);
    if (sfc->getParent() != NULL) {
        sfc->getParent()->getTransFromParent(trans_mat);
    }
    bnd_max = glm::vec3(BOUND_MIN_DEFAULT);
    bnd_min = glm::vec3(BOUND_MAX_DEFAULT);
    // have conective surface , then process meshes
    if ((vertexes.size() > 0) && (indecis.size() > 0)) {
        pts = (obj_points *)malloc(sizeof(obj_points) * vertexes.size());
        memset(pts, 0, sizeof(obj_points) * vertexes.size());
        idxes = (GLushort *)malloc(sizeof(GLushort) * indecis.size());
        memset(idxes, 0, sizeof(GLushort) * indecis.size());
        pts_len = vertexes.size();
        idx_len = indecis.size();
        for (int j = 0; j < vertexes.size(); j++) {
            glm::vec4 tmp_vert = glm::vec4(
                        vertexes.at(j)->getX(),
                        vertexes.at(j)->getY(),
                        vertexes.at(j)->getZ(),
                        1.0f);
            tmp_vert = trans_mat * tmp_vert;
            tmp_vert = tmp_vert / tmp_vert.w;
            bnd_max = glm::max(glm::vec3(tmp_vert), bnd_max);
            bnd_min = glm::min(glm::vec3(tmp_vert), bnd_min);
            pts[j].vertex_x = tmp_vert.x;
            pts[j].vertex_y = tmp_vert.y;
            pts[j].vertex_z = tmp_vert.z;
            pts[j].texture_x = vertexes.at(j)->getW();
            pts[j].texture_y = 1.0f - vertexes.at(j)->getH();
        }
        for (int j = 0; j < indecis.size(); j++) {
            idxes[j] = indecis.at(j);
        }
        // create new mesh
        gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
        m->set_material_index(0);
        m->set_bounding_value_max(bnd_max);
        m->set_bounding_value_min(bnd_min);
        vct.push_back(m);
        // clean env
        free(pts);
        free(idxes);
    }
    return;
}


gl3d::gl3d_wall_attach::gl3d_wall_attach() {
    this->attach_point = this->start_point;
    this->attach = NULL;
}
