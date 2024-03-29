#include "utils/gl3d_math.h"
#include "editor/gl3d_wall.h"
#include "utils/gl3d_lock.h"
#include "editor/surface.h"
#include "resource_and_network/global_material.h"
#include "utils/gl3d_global_param.h"

using namespace std;
using namespace gl3d;
using namespace klm;
using namespace gl3d::math;

static const float wall_combine_distance = 1.0f;

static bool is_point_in_faces(QVector<math::triangle_facet> faces, glm::vec3 pt);

static gl3d::obj_points post_rect[4] = {
        {-1.0, 1.0,  0.0,  // vt
                0.0, 0.0, 0.0,   // normal
                0.0, 0.0, 0.0, 0.0, // color
                0.0, 1.0,     // text
                0},
        {-1.0, -1.0, 0.0,  // vt
                0.0, 0.0, 0.0,   // normal
                0.0, 0.0, 0.0, 0.0, // color
                0.0, 0.0,     // text
                0},
        {1.0,  1.0,  0.0,  // vt
                0.0, 0.0, 0.0,   // normal
                0.0, 0.0, 0.0, 0.0, // color
                1.0, 1.0,     // text
                0},
        {1.0,  -1.0, 0.0,  // vt
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
    this->relate_rooms.clear();
    this->is_picked = false;
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

void gl3d_wall::clean() {
    if (this->start_point_fixed) {
        this->seperate(this->start_point_attach);
    }

    if (this->end_point_fixed) {
        this->seperate(this->end_point_attach);
    }

    for (auto it = this->holes_on_this_wall.begin();
         it != this->holes_on_this_wall.end();
         it++) {
        it.value()->set_on_witch_wall(NULL);
        delete it.value();
    }
    this->holes_on_this_wall.clear();

    // delete related doors and windows
    scene * msc = (scene *)gl3d_global_param::shared_instance()->main_scene;
    klm::design::scheme * skt = msc->get_attached_sketch();
    Q_FOREACH(gl3d_door * dit, *(skt->get_doors())) {
            if (dit->attached_wall_id == this->get_id()) {
                skt->del_door(dit);
            }
        }
    Q_FOREACH(gl3d_window * wit, *(skt->get_windows())) {
            if (wit->attached_wall_id == this->get_id()) {
                skt->del_window(wit);
            }
        }
}

gl3d_wall::~gl3d_wall() {
    if (this->start_point_fixed) {
        this->seperate(this->start_point_attach);
    }

    if (this->end_point_fixed) {
        this->seperate(this->end_point_attach);
    }

    for (auto it = this->holes_on_this_wall.begin();
         it != this->holes_on_this_wall.end();
         it++) {
        it.value()->set_on_witch_wall(NULL);
        delete it.value();
    }
    this->holes_on_this_wall.clear();

    for (auto it = this->sfcs.begin();
         it != this->sfcs.end();
         it++) {
        delete *it;
    }
    this->sfcs.clear();

    // delete related doors and windows
    scene * msc = (scene *)gl3d_global_param::shared_instance()->main_scene;
    klm::design::scheme * skt = msc->get_attached_sketch();
    Q_FOREACH(gl3d_door * dit, *(skt->get_doors())) {
            if (dit->attached_wall_id == this->get_id()) {
                skt->del_door(dit);
            }
        }
    Q_FOREACH(gl3d_window * wit, *(skt->get_windows())) {
            if (wit->attached_wall_id == this->get_id()) {
                skt->del_window(wit);
            }
        }
}

int gl3d_wall::get_availble_hole_id() {
    int i = 0;
    for (i = 0; i < this->holes_on_this_wall.size(); i++) {
        if (!this->holes_on_this_wall.contains(i))
            return i;
    }

    return i + 1;
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

void gl3d_wall::seperate(gl3d::gl3d_wall_attach &attachment) {
    gl3d_wall *attach = attachment.attach;
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
        gl3d_wall *tmp_wall = this->start_point_attach.attach;
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
        gl3d_wall *tmp_wall = this->end_point_attach.attach;
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

    try {
        // 根据左右边线得出surface
        QVector<glm::vec3> points;
        klm::Surface *s;
        // left
        points.clear();
        points.push_back(glm::vec3(l_left.b.x, 0.0f, l_left.b.y));
        points.push_back(glm::vec3(l_left.a.x, 0.0f, l_left.a.y));
        points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
        points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
        s = new klm::Surface(points);
        s->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
        this->sfcs.push_back(s);
        // right
        points.clear();
        points.push_back(glm::vec3(l_right.a.x, 0.0f, l_right.a.y));
        points.push_back(glm::vec3(l_right.b.x, 0.0f, l_right.b.y));
        points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
        points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
        s = new klm::Surface(points);
        s->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
        this->sfcs.push_back(s);
        // back
        points.clear();
        points.push_back(glm::vec3(l_left.a.x, 0.0f, l_left.a.y));
        points.push_back(glm::vec3(l_right.a.x, 0.0f, l_right.a.y));
        points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
        points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
        s = new klm::Surface(points);
        s->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
        this->sfcs.push_back(s);
        // front
        points.clear();
        points.push_back(glm::vec3(l_right.b.x, 0.0f, l_right.b.y));
        points.push_back(glm::vec3(l_left.b.x, 0.0f, l_left.b.y));
        points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
        points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
        s = new klm::Surface(points);
        s->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
        this->sfcs.push_back(s);
        // top
        points.clear();
        points.push_back(glm::vec3(l_left.a.x, this->hight, l_left.a.y));
        points.push_back(glm::vec3(l_right.a.x, this->hight, l_right.a.y));
        points.push_back(glm::vec3(l_right.b.x, this->hight, l_right.b.y));
        points.push_back(glm::vec3(l_left.b.x, this->hight, l_left.b.y));
        s = new klm::Surface(points);
        s->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
        this->sfcs.push_back(s);
    }
    catch (SurfaceException &exp) {
        // do nothing
        Q_FOREACH(klm::Surface *s, this->sfcs) {
                delete s;
            }
        this->sfcs.clear();
        return;
    }

    // will not fill meshes now
    // now need to dig hole
    try {
        // get left and right faces
        QVector<math::triangle_facet> faces_left;
        QVector<math::triangle_facet> faces_right;
        faces_left.clear();
        gl3d::get_faces_from_surface(this->sfcs.at(0), faces_left);
        gl3d::get_faces_from_surface(this->sfcs.at(1), faces_right);

        for (auto it = this->holes_on_this_wall.begin();
             it != this->holes_on_this_wall.end();
                ) {
            // check hole valid
            if (!it.value()->is_valid()) {
                it.value()->set_on_witch_wall(NULL);
                it = this->holes_on_this_wall.erase(it);
            }
            else {
                it++;
            }
        }

        for (auto it = this->holes_on_this_wall.begin();
             it != this->holes_on_this_wall.end();
             it++) {
            // generate line group
            QVector<line_3d> hole_line;
            hole_line.clear();
            glm::vec3 pa = it.value()->get_pta();
            glm::vec3 pb = it.value()->get_ptb();
            if (glm::length(this->get_start_point() - glm::vec2(pa.x, pa.z)) >=
                glm::length(this->get_start_point() - glm::vec2(pb.x, pb.z))) {
                auto tmppt = pa;
                pa = pb;
                pb = tmppt;
            }

            // Dig holes on wall
            glm::vec3 dir_3d = convert_vec2_to_vec3(top_dir);
            QVector<glm::vec3> hole_vertex;
            glm::vec3 pa_left = pa + dir_3d * this->thickness / 2.0f;
            glm::vec3 pb_left = pb + dir_3d * this->thickness / 2.0f;
            glm::vec3 pa_right = pa - dir_3d * this->thickness / 2.0f;
            glm::vec3 pb_right = pb - dir_3d * this->thickness / 2.0f;
            pa_left.y = glm::min(pa.y, pb.y);
            pb_left.y = glm::max(pa.y, pb.y);
            pa_right.y = glm::min(pa.y, pb.y);
            pb_right.y = glm::max(pa.y, pb.y);
            hole_vertex.clear();
            // dig right wall
            glm::mat4 model_mat = this->sfcs.at(1)->getRenderingTransform();
            model_mat = glm::inverse(model_mat);
            glm::vec4 tmp = model_mat * glm::vec4(pa_right, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pb_right.x, pa_right.y, pb_right.z, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pb_right, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pa_right.x, pb_right.y, pa_right.z, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            klm::Surface *sf = this->sfcs.at(1)->addSubSurface(hole_vertex);
            if (NULL != sf) {
                // TODO : need fix
                sf->setTranslate(glm::vec3(0.0f, 0.0f, this->thickness));
                sf->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
                sf->setConnectiveMateiral(new klm::Surfacing("mtl000000"));
                sf->hideSurface();
            }
            hole_vertex.clear();
            // dig left wall
            model_mat = this->sfcs.at(0)->getRenderingTransform();
            model_mat = glm::inverse(model_mat);
            tmp = model_mat * glm::vec4(pa_right, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pa_right.x, pb_right.y, pa_right.z, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pb_right, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            tmp = model_mat * glm::vec4(pb_right.x, pa_right.y, pb_right.z, 1.0f);
            tmp = tmp / tmp.w;
            hole_vertex.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
            sf = this->sfcs.at(0)->addSubSurface(hole_vertex);
            if (NULL != sf) {
                sf->hideSurface();
            }
            hole_vertex.clear();
        }
    }
    catch (SurfaceException &exp) {
        GL3D_UTILS_THROW("Dig Hole failed ");
    }


    gl3d_lock::shared_instance()->wall_lock.unlock();
    return;
}


void gl3d_wall::get_coord_on_screen(gl3d::scene *main_scene,
                                    glm::vec2 &start_pos,
                                    glm::vec2 &end_pos) {
    glm::vec2 stpos = this->get_start_point();
    glm::vec2 edpos = this->get_end_point();

    glm::mat4 pvm = glm::mat4(1.0);
    pvm = pvm * main_scene->watcher->get_projection_matrix();
    pvm = pvm * main_scene->watcher->get_viewing_matrix();

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
    coord_out = (coord_out + 1.0f) / 2.0f;
    start_pos = glm::vec2(coord_out.x, coord_out.y);
    start_pos.x = start_pos.x * main_scene->get_width();
    start_pos.y = main_scene->get_height() - start_pos.y * main_scene->get_height();
    coord_out = pvm * this->get_property()->rotate_mat * glm::vec4(edpos.x, 0.0f, edpos.y, 1.0f);
    coord_out = (coord_out + 1.0f) / 2.0f;
    end_pos = glm::vec2(coord_out.x, coord_out.y);
    end_pos.x = end_pos.x * main_scene->get_width();
    end_pos.y = main_scene->get_height() - end_pos.y * main_scene->get_height();
}

bool gl3d_wall::combine(gl3d_wall *wall1, gl3d_wall *wall2, glm::vec2 combine_point) {
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


bool gl3d_wall::combine(gl3d_wall *wall1, gl3d_wall *wall2, tag_combine_traits combine_traits) {

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

void gl3d::surface_to_mesh(klm::Surface *sfc,
                           QVector<gl3d::mesh *> &vct, bool picked) {
    // process local verticles
    // create vertex buffer
    glm::vec3 bnd_max(BOUND_MIN_DEFAULT);
    glm::vec3 bnd_min(BOUND_MAX_DEFAULT);
    GLfloat *tmp_data = NULL;
    int pts_len;
    gl3d::obj_points *pts = NULL;

    const QVector<Surface::Vertex *> *vertexes = sfc->getRenderingVertices();
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
    GLushort *idxes = NULL;
    int idx_len;
    const QVector<GLushort> *indecis = sfc->getRenderingIndices();
    idx_len = indecis->size();
    idxes = new GLushort[idx_len];
    for (int j = 0; j < indecis->size(); j++) {
        idxes[j] = indecis->at(j);
    }

    // create new mesh
    if (sfc->isSurfaceVisible()) {
        gl3d::mesh *m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
        global_material_lib::shared_instance()->request_new_mtl(sfc->getSurfaceMaterial()->getID(), NULL, NULL);
        m->set_material_index(
                global_material_lib::shared_instance()->get_mtl_id(sfc->getSurfaceMaterial()->getID()));
        m->set_bounding_value_max(bnd_max);
        m->set_bounding_value_min(bnd_min);
        m->set_texture_repeat(true);
        m->set_is_blink(picked);
        vct.push_back(m);
    }

    // clean env
    delete pts;
    delete idxes;

    // process conective surface
    // have conective surface , then process meshes
    if (sfc->isConnectiveSurface() && sfc->isConnectiveVisible()) {
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
        gl3d::mesh *m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
        global_material_lib::shared_instance()->request_new_mtl(sfc->getConnectiveMaterial()->getID(), NULL, NULL);
        m->set_material_index(
                global_material_lib::shared_instance()->get_mtl_id(sfc->getConnectiveMaterial()->getID()));
        m->set_bounding_value_max(bnd_max);
        m->set_bounding_value_min(bnd_min);
        m->set_texture_repeat(true);
        m->set_is_blink(picked);
        vct.push_back(m);
        // clean env
        delete pts;
        delete idxes;
    }

    // process sub surface
    if ((sfc->getSurfaceCnt() > 0) && (sfc->isSurfaceVisible())) {
        for (int i = 0; i < sfc->getSurfaceCnt(); i++) {
            surface_to_mesh(sfc->getSubSurface(i), vct, picked);
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

void gl3d_wall::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    for (auto it = this->sfcs.begin();
         it != this->sfcs.end();
         it++) {
        gl3d::surface_to_mesh(*it, ms);
    }

    Q_FOREACH(gl3d::mesh * ims, ms) {
            if (this->is_picked) {
                ims->set_is_blink(true);
            }
            else {
                ims->set_is_blink(false);
            }
        }
}

void gl3d_wall::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    global_material_lib::shared_instance()->copy_mtls_pair_info(mt);
}

void gl3d_wall::set_translation_mat(const glm::mat4 &trans) {
    object::set_translation_mat(trans);
    return;
}

void gl3d_wall::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    for (auto it = ms.begin();
         it != ms.end();
         it++) {
        delete (*it);
    }
    ms.clear();
}

void gl3d_wall::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    mt.clear();
}

// get triangles from surfaces
void gl3d::get_faces_from_surface(klm::Surface *sfc, QVector<math::triangle_facet> &faces) {
    for (int i = 0; i < sfc->getSurfaceCnt(); i++) {
        gl3d::get_faces_from_surface(sfc->getSubSurface(i), faces);
    }

    // process local verticles
    const QVector<Surface::Vertex *> *vertexes = sfc->getRenderingVertices();
    const QVector<GLushort> *indecis = sfc->getRenderingIndices();
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

klm::Surface* gl3d::pick_up_surface_in_surface(klm::Surface *sfc, glm::vec3 pt) {
    QVector<math::triangle_facet> faces;
    faces.clear();
    gl3d::get_face_from_root_surface(sfc, faces);
    Q_FOREACH(auto fit, faces) {
            if (fit.is_point_in_facet(pt)) {
                return sfc;
            }
        }

    for (int i = 0; i < sfc->getSurfaceCnt(); i++) {
        klm::Surface * ret_sfc = pick_up_surface_in_surface(sfc->getSubSurface(i), pt);
        if (NULL != ret_sfc)
            return ret_sfc;
    }

    return NULL;
}

static bool is_point_in_faces(QVector<math::triangle_facet> faces, glm::vec3 pt) {
    for (auto it = faces.begin();
         it != faces.end();
         it++) {
        if (it->is_point_in_facet(pt)) {
            return true;
        }
    }

    return false;
}

// calculate points which line cross triangles
typedef QPair<glm::vec3, glm::vec3> point_and_normal;
using namespace gl3d::math;

static inline bg_Point vec3_to_bg_pt(glm::vec3 pt) {
    return bg_Point(pt.x, pt.y, pt.z);
}

static inline bg_Polygon triangle_to_bg_poly(math::triangle_facet f) {
    bg_Polygon poly;
    poly.outer().push_back(vec3_to_bg_pt(f.a));
    poly.outer().push_back(vec3_to_bg_pt(f.b));
    poly.outer().push_back(vec3_to_bg_pt(f.c));
    return poly;
}

static void cast_ray_to_faces(QVector<math::triangle_facet> &faces,
                              QVector<point_and_normal> &crosses,
                              math::line_3d ray_cast) {
    for (QVector<math::triangle_facet>::iterator it = faces.begin();
         it != faces.end();
         it++) {
        glm::vec3 pt;
        if (math::line_cross_facet(*it, ray_cast, pt)) {
            bg_Point bpt = vec3_to_bg_pt(pt);
            bg_Polygon poly = triangle_to_bg_poly(*it);
//            if (bg::within(bpt, poly)) {
            if (it->is_point_in_facet(pt)) {
                glm::vec3 nor;
                nor = glm::cross(it->a - it->b, it->c - it->b);
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
    gl3d::viewer *cam = sce->watcher;
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
            gl3d::gl3d_global_param::shared_instance()->canvas_width);
    coord_on_screen.y = cam->get_height() - coord_on_screen.y;
    glm::vec3 coord_in = glm::vec3(coord_on_screen.x, coord_on_screen.y, 0.0);
    coord_in.z = 1.0;
    glm::vec3 txxx = glm::unProject(coord_in,
                                    glm::mat4(1.0),
                                    cam->get_projection_matrix() * cam->get_viewing_matrix() *
                                    ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)),
                                    glm::vec4(0.0, 0.0,
                                              cam->get_width(),
                                              cam->get_height()));
    coord_in.z = 0.1;
    txxx = txxx - glm::unProject(coord_in,
                                 glm::mat4(1.0),
                                 cam->get_projection_matrix() * cam->get_viewing_matrix() *
                                 ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)),
                                 glm::vec4(0.0, 0.0,
                                           cam->get_width(),
                                           cam->get_height()));

    glm::vec3 near_pt = cam->get_current_position();
    txxx = glm::normalize(txxx);
//    txxx = glm::normalize(cam->get_look_direction());
    math::line_3d ray_cast(near_pt, near_pt + txxx);

    QVector<math::triangle_facet> faces;
    QVector<point_and_normal> crosses;
    glm::vec3 target_pt;
    glm::vec3 o_nor(1.0f);
    float dis_to_pt = 0.0f;
    faces.clear();
    crosses.clear();

    // get left and right facet
    gl3d::get_faces_from_surface(this->sfcs.at(0), faces);
    gl3d::get_faces_from_surface(this->sfcs.at(1), faces);
//    for (auto it = this->sfcs.begin();
//         it != this->sfcs.end();
//         it++) {
//        get_faces_from_surface(*it, faces);
//    }

    // get all cross points
    cast_ray_to_faces(faces, crosses, ray_cast);

    // check out real cross
    if (crosses.size() <= 0) {
        return false;
    }
    dis_to_pt = glm::length(crosses.at(0).first - near_pt);
    target_pt = crosses.at(0).first;
    o_nor = crosses.at(0).second;
    for (auto it = crosses.begin();
         it != crosses.end();
         it++) {
        if (glm::length((*it).first - near_pt) < dis_to_pt) {
            // if has nearer point
            dis_to_pt = glm::length((*it).first - near_pt);
            target_pt = it->first;
            // and its normal
            o_nor = it->second;
        }
    }

    out_point_on_wall = target_pt;
    out_point_normal = o_nor;

    return true;
}

int hole::global_hole_id = 1;

void hole::init() {
    this->pta = glm::vec3(0.0f);
    this->ptb = glm::vec3(0.0f);
    this->on_witch_wall = NULL;
    this->hole_id = this->global_hole_id++;
    this->sfc = NULL;
}

hole::hole() {
    this->init();
}

hole::hole(const hole &cp) {
    GL3D_UTILS_THROW("hole should not copy");
    return;
}

hole::hole(gl3d_wall *w, glm::vec3 point_a, glm::vec3 point_b, int hid) {
    this->init();
    this->hole_id = hid;
    this->pta = point_a;
    this->ptb = point_b;
    this->on_witch_wall = w;
    if (this->is_valid()) {
        w->holes_on_this_wall.insert(this->hole_id, this);
    }
    else {
        this->on_witch_wall = NULL;
    }
}

hole::hole(gl3d_wall *w, glm::vec3 center_point, float width, float min_height, float max_height, int hid) {
    this->init();
    this->hole_id = hid;

    // now calculate point a and point b
    glm::vec3 dir = math::convert_vec2_to_vec3(
            w->get_end_point() - w->get_start_point()
    );
    dir = glm::normalize(dir);
    this->pta = center_point - dir * width * 0.5f;
    this->pta.y = min_height;
    this->ptb = center_point + dir * width * 0.5f;
    this->ptb.y = max_height;

    this->on_witch_wall = w;
    if (this->is_valid()) {
        w->holes_on_this_wall.insert(this->hole_id, this);
    }
    else {
        this->on_witch_wall = NULL;
    }
}

bool hole::is_valid() {
    if (this->on_witch_wall == NULL) {
        return false;
    }
    QVector<math::triangle_facet> faces;
    faces.clear();
    glm::vec3 pa;
    glm::vec3 pb;
    pa = convert_vec2_to_vec3(this->on_witch_wall->get_start_point());
    pb = convert_vec2_to_vec3(this->on_witch_wall->get_end_point());

    // get the facet
    faces.push_back(triangle_facet(
            pa,
            glm::vec3(pb.x, this->on_witch_wall->get_hight(), pb.z),
            glm::vec3(pa.x, this->on_witch_wall->get_hight(), pa.z)
    ));
    faces.push_back(triangle_facet(
            pa,
            pb,
            glm::vec3(pb.x, this->on_witch_wall->get_hight(), pb.z)
    ));

    bool pa_locate = false;
    bool pb_locate = false;
    for (auto it = faces.begin();
         it != faces.end();
         it++) {
        if (it->is_point_in_facet(this->pta)) {
            pa_locate = true;
        }
        if (it->is_point_in_facet(this->ptb)) {
            pb_locate = true;
        }
    }

    return pa_locate && pb_locate;
}

room::room(QVector<glm::vec3> e_pts) {
    this->edge_points = e_pts;
    this->ground = new Surface(e_pts);
    this->ground->setSurfaceMaterial(new klm::Surfacing("mtl000001"));
    QVector<glm::vec3> ceil_pts;
    Q_FOREACH(glm::vec3 pit, e_pts) {
            pit.y = gl3d::gl3d_global_param::shared_instance()->room_height;
            ceil_pts.push_front(pit);
        }
    this->ceil = new Surface(ceil_pts);
    this->ceil->setSurfaceMaterial(new klm::Surfacing("mtl000001"));
    this->name = "";
    this->relate_walls.clear();
    this->picked = false;
}

void room::set_picked(bool on_off) {
    this->picked = on_off;
    Q_FOREACH(gl3d_wall * wit, this->relate_walls) {
            wit->set_is_picked(on_off);
        }
}

bool room::get_picked() {
    return this->picked;
}

room::~room() {
    if (NULL != this->ground) {
        delete this->ground;
        this->ground = NULL;
    }
    if (NULL != this->ceil) {
        delete this->ceil;
        this->ceil = NULL;
    }
    Q_FOREACH(gl3d_wall *wit, this->relate_walls) {
            if (this->relate_walls.contains(wit))
                this->relate_walls.remove(wit);
        }
    this->relate_walls.clear();
    return;
}

bool gl3d::gl3d_wall::wall_cross(gl3d_wall *wall_cutter, gl3d_wall *wall_target, QSet<gl3d_wall *> &output_walls) {
    math::line_2d cut_l(wall_cutter->get_start_point(), wall_cutter->get_end_point());
    math::line_2d tar_l(wall_target->get_start_point(), wall_target->get_end_point());
    glm::vec2 cross_pt;
    // if has point near point, then return false
    if ((math::point_near_point(cut_l.a, tar_l.a)) ||
        (math::point_near_point(cut_l.a, tar_l.b)) ||
        (math::point_near_point(cut_l.b, tar_l.a)) ||
        (math::point_near_point(cut_l.b, tar_l.b))) {
        return false;
    }
    // if line parallel , then return
    if (glm::abs(glm::dot(glm::normalize(cut_l.b - cut_l.a), glm::normalize(tar_l.b - tar_l.a))) > 0.99f)
        return false;
    if (math::get_cross(cut_l, tar_l, cross_pt)) {
        if ((cut_l.point_on_line(cross_pt) && tar_l.point_on_line(cross_pt))  // point should on line
            && (glm::min(cut_l.point_min_distance_to_vertex(cross_pt), tar_l.point_min_distance_to_vertex(cross_pt)) >
                glm::max(wall_cutter->get_thickness(),
                         wall_target->get_thickness()))) // should away from vertex of wall
        {
            // now need to cut wall_target, now wall1 st to cross
            // TODO : should fix wall_target combine
            gl3d_wall *w = wall_target;
            gl3d_wall *n_w = new gl3d_wall(w->get_start_point(), cross_pt, w->get_thickness(), w->get_hight());
            if (w->start_point_fixed) {
                if (w->start_point_attach.attach_point == gl3d::gl3d_wall_attach::start_point) {
                    w->start_point_attach.attach->start_point_attach.attach = n_w;
                }
                else {
                    w->start_point_attach.attach->end_point_attach.attach = n_w;
                }
                n_w->start_point_attach.attach = w->start_point_attach.attach;
                n_w->start_point_attach.attach_point = w->start_point_attach.attach_point;
                n_w->set_start_point_fixed(true);
                // renew old wall attach
                w->set_start_point_fixed(false);
                w->get_start_point_attach()->attach = NULL;
            }
            output_walls.insert(n_w);
            // now cross to wall1 end
            n_w = new gl3d_wall(cross_pt, w->get_end_point(), w->get_thickness(), w->get_hight());
            if (w->end_point_fixed) {
                if (w->end_point_attach.attach_point == gl3d::gl3d_wall_attach::start_point) {
                    w->end_point_attach.attach->start_point_attach.attach = n_w;
                }
                else {
                    w->end_point_attach.attach->end_point_attach.attach = n_w;
                }
                n_w->end_point_attach.attach = w->end_point_attach.attach;
                n_w->end_point_attach.attach_point = w->end_point_attach.attach_point;
                n_w->set_end_point_fixed(true);
                // renew old wall attach
                w->set_end_point_fixed(false);
                w->get_start_point_attach()->attach = NULL;
            }
            output_walls.insert(n_w);
            return true;
        }
    }

    return false;
}

bool gl3d_wall::delete_wall_in_wall(gl3d_wall *&wall_target, gl3d_wall *&wall_rmver) {
    // TODO : process near point and overlay walls
    line_2d target(wall_target->get_start_point(), wall_target->get_end_point());
    line_2d remver(wall_rmver->get_start_point(), wall_rmver->get_end_point());

    if (remver.point_on_line(target.a) && remver.point_on_line(target.b)) { // should not add wall
        delete wall_target;
        wall_target = NULL;
        return true;
    }

    // cross lines so return now
    if (glm::abs(glm::dot(glm::normalize(target.b - target.a), glm::normalize(remver.b - remver.a))) < 0.99f)
        return false;

    // target covered remover, so change target and then return
    if (target.point_on_line(remver.a) && target.point_on_line(remver.b)) {
        if (wall_target->end_point_fixed)
            wall_target->seperate(wall_target->end_point_attach);
        if (glm::length(remver.a - target.a) < glm::length(remver.b - target.a))
            wall_target->set_end_point(remver.a);
        else
            wall_target->set_end_point(remver.b);

        return false;
    }

    if (remver.point_on_line(target.a)) {
        if (wall_target->get_start_point_fixed())
            wall_target->seperate(wall_target->start_point_attach);
        if (glm::length(remver.b - target.b) < glm::length(remver.a - target.b))
            wall_target->set_start_point(remver.b);
        else
            wall_target->set_start_point(remver.a);
    }
    else if (remver.point_on_line(target.b)) {
        if (wall_target->get_end_point_fixed())
            wall_target->seperate(wall_target->end_point_attach);
        if (glm::length(remver.b - target.a) < glm::length(remver.a - target.a))
            wall_target->set_end_point(remver.b);
        else
            wall_target->set_end_point(remver.a);
    }

    return false;
}

bool gl3d_wall::set_material(std::string res_id) {
    Q_FOREACH(klm::Surface * sit, this->sfcs) {
            sit->setSurfaceMaterial(new klm::Surfacing(res_id));
        }

    return true;
}

void room::get_relate_surfaces(QSet<klm::Surface *> &surfaces) {
    // TODO : test get relate surfaces
    Q_FOREACH(gl3d_wall * wit, this->relate_walls) {
            for (int i = 0; i < (this->edge_points.size() - 1); i++) {
                if (math::point_near_point(math::convert_vec2_to_vec3(wit->get_start_point()),
                                           this->edge_points[i]) &&
                    math::point_near_point(math::convert_vec2_to_vec3(wit->get_end_point()),
                                           this->edge_points[i + 1])) {
                    // get left sfc
                    surfaces.insert(wit->get_sfcs()->at(0));
                }
                else if (math::point_near_point(math::convert_vec2_to_vec3(wit->get_start_point()),
                                                this->edge_points[i + 1]) &&
                         math::point_near_point(math::convert_vec2_to_vec3(wit->get_end_point()),
                                                this->edge_points[i])) {
                    // get right sfc
                    surfaces.insert(wit->get_sfcs()->at(1));
                }
                else if (math::point_near_point(math::convert_vec2_to_vec3(wit->get_start_point()),
                                                this->edge_points.last()) &&
                         math::point_near_point(math::convert_vec2_to_vec3(wit->get_end_point()),
                                                this->edge_points.first())) {
                    // get left sfc
                    surfaces.insert(wit->get_sfcs()->at(0));
                }
                else if (math::point_near_point(math::convert_vec2_to_vec3(wit->get_start_point()),
                                                this->edge_points.first()) &&
                         math::point_near_point(math::convert_vec2_to_vec3(wit->get_end_point()),
                                                this->edge_points.last())) {
                    // get right sfc
                    surfaces.insert(wit->get_sfcs()->at(1));
                }
            }
        }

    return;
}

void room::clear_relate_surfaces(QSet<klm::Surface *> &surfaces) {
    surfaces.clear();
    return;;
}

void room::set_material(std::string res_id) {
    QSet<klm::Surface *> ss;
    this->get_relate_surfaces(ss);
    Q_FOREACH(klm::Surface *sit, ss) {
            sit->setSurfaceMaterial(new klm::Surfacing(res_id));
        }

    return;
}

bool room::save_to_xml(pugi::xml_node &node) {
    // first attribute type is room
    node.append_attribute("type").set_value("room");
    // room name
    node.append_attribute("room_name").set_value(this->name.c_str());
    // add ground
    pugi::xml_node tmp = node.append_child("ground");
    this->ground->save(tmp);
    // add ceil
    tmp = node.append_child("ceil");
    this->ceil->save(tmp);
    // save edge points
    for (int i = 0; i < this->edge_points.size(); i++) {
        tmp = node.append_child("edge_point");
        gl3d::xml::save_vec_to_xml(this->edge_points.at(i), tmp);
        tmp.append_attribute("seq_id").set_value(i);
    }
    // relate wall id
    Q_FOREACH(gl3d_wall * wit, this->relate_walls) {
            tmp = node.append_child("relate_wall_id");
            tmp.append_attribute("wall_id").set_value(wit->get_id());
        }

    return true;
}

room* room::load_from_xml(pugi::xml_node &node) {
    // check type
    if (std::string(node.attribute("type").value()) != std::string("room")) {
        return NULL;
    }
    QMap<int , glm::vec3> edgept_map;
    // load edge
    auto nset = node.children("edge_point");
    for (auto xit = nset.begin();
            xit != nset.end();
            xit++) {
        int seq_id = xit->attribute("seq_id").as_int();
        glm::vec3 inspt;
        pugi::xml_node tmp = *xit;
        gl3d::xml::load_xml_to_vec(tmp, inspt);
        edgept_map.insert(seq_id, inspt);
    }
    QVector<glm::vec3> pts;
    for (int i = 0; i < edgept_map.size(); i++) {
        pts.push_back(edgept_map.value(i));
    }
    // room name
    room * n_r = new room(pts);
    n_r->name = std::string(node.attribute("room_name").value());
    // ground
    n_r->ground = new klm::Surface;
    n_r->ground->load(node.child("ground").first_child());
    n_r->ceil = new klm::Surface;
    n_r->ceil->load(node.child("ceil").first_child());
    // related walls
    nset = node.children("relate_wall_id");
    for (auto xit = nset.begin();
            xit != nset.end();
            xit++) {
        gl3d::scene * msc = (gl3d::scene * ) gl3d_global_param::shared_instance()->main_scene;
        int wallid = xit->attribute("wall_id").as_int();
        if (msc->get_attached_sketch()->get_objects()->contains(wallid)) {
            n_r->relate_walls.insert((gl3d_wall *) msc->get_attached_sketch()->get_obj(wallid));
        }
    }

    return n_r;
}

bool hole::save_to_xml(pugi::xml_node &node) {
    // set type
    node.append_attribute("type").set_value("gl3d_hole");
    // 2 points
    pugi::xml_node nd = node.append_child("pointa");
    gl3d::xml::save_vec_to_xml(this->pta, nd);
    nd = node.append_child("pointb");
    gl3d::xml::save_vec_to_xml(this->ptb, nd);
    // hole id
    node.append_attribute("hole_id").set_value(this->hole_id);
    // wall id
    node.append_attribute("on_wall_id").set_value(this->on_witch_wall->get_id());
    return true;
}

hole* hole::load_from_xml(pugi::xml_node node) {
    QString type(node.attribute("type").value());
    if (type != "gl3d_hole") {
        return NULL;
    }
    // search on witch wall
    int on_wall_id = node.attribute("on_wall_id").as_int();
    gl3d::scene * msc = (gl3d::scene *) gl3d_global_param::shared_instance()->main_scene;
    if (msc->get_attached_sketch()->get_objects()->contains(on_wall_id)) {
        gl3d_wall * w = (gl3d_wall *) msc->get_obj(on_wall_id);
        glm::vec3 pta;
        glm::vec3 ptb;
        xml::load_xml_to_vec(node.child("pointa"), pta);
        xml::load_xml_to_vec(node.child("pointb"), ptb);
        hole * h = new hole(w, pta, ptb, w->get_availble_hole_id());
        h->set_hole_id(node.attribute("hole_id").as_int());
        return h;
    }

    return NULL;
}

hole* hole::load_from_xml(pugi::xml_node node, gl3d_wall * wall) {
    QString type(node.attribute("type").value());
    if (type != "gl3d_hole") {
        return NULL;
    }
    // install on wall
    glm::vec3 pta;
    glm::vec3 ptb;
    xml::load_xml_to_vec(node.child("pointa"), pta);
    xml::load_xml_to_vec(node.child("pointb"), ptb);
    hole *h = new hole(wall, pta, ptb, node.attribute("hole_id").as_int());
    return h;
}

using namespace pugi;
bool gl3d_wall_attach::save_to_xml(pugi::xml_node &node) {
    // type
    node.append_attribute("type").set_value("gl3d_wall_attach");
    // attach wall
    node.append_attribute("attach_wall_id").set_value(this->attach->get_id());
    // attach point
    node.append_attribute("attach_point").set_value((int) this->attach_point);
    return true;
}

bool gl3d_wall_attach::load_from_xml(pugi::xml_node node) {
    QString type(node.attribute("type").value());
    if (type != "gl3d_wall_attach") {
        return false;
    }
    int aid = node.attribute("attach_wall_id").as_int();
    gl3d::scene * msc = (gl3d::scene *)gl3d_global_param::shared_instance()->main_scene;
    if (msc->get_attached_sketch()->get_objects()->contains(aid)) {
        this->attach = (gl3d_wall *) msc->get_obj(aid);
        this->attach_point = (attachment_point) node.attribute("attach_point").as_int();
        return true;
    }

    return false;
}

// TODO : test wall save
bool gl3d_wall::save_to_xml(pugi::xml_node &node) {
    // save type
    node.append_attribute("type").set_value("gl3d_wall");
    // object properties
    node.append_attribute("id").set_value(this->get_id());
    // geometry properties
    xml_node ndc = node.append_child("start_pt");
    xml::save_vec_to_xml(this->start_point, ndc);
    ndc = node.append_child("end_pt");
    xml::save_vec_to_xml(this->end_point, ndc);
    node.append_attribute("thickness").set_value(this->thickness);
    node.append_attribute("height").set_value(this->hight);
    // fix and attach
    node.append_attribute("st_fixed").set_value(this->start_point_fixed);
    node.append_attribute("ed_fixed").set_value(this->end_point_fixed);
    if (this->start_point_fixed) {
        ndc = node.append_child("start_attach");
        this->start_point_attach.save_to_xml(ndc);
    }
    if (this->end_point_fixed) {
        ndc = node.append_child("end_attach");
        this->end_point_attach.save_to_xml(ndc);
    }
    // sfcs
    for (int i = 0; i < this->sfcs.size(); i++) {
        ndc = node.append_child("wall_sfc");
        ndc.append_attribute("sid").set_value(i);
        this->sfcs.at(i)->save(ndc);
    }
    // holes
    Q_FOREACH(hole * hit, this->holes_on_this_wall) {
            ndc = node.append_child("wall_hole");
            hit->save_to_xml(ndc);
        }

    return true;
}

// TODO : test wall load
gl3d_wall* gl3d_wall::load_from_xml(pugi::xml_node node) {
    // validate type
    QString type(node.attribute("type").value());
    if (type != "gl3d_wall") {
        return NULL;
    }
    // load geometry properties
    glm::vec2 st_tmp;
    glm::vec2 ed_tmp;
    xml::load_xml_to_vec(node.child("start_pt"), st_tmp);
    xml::load_xml_to_vec(node.child("end_pt"), ed_tmp);
    gl3d_wall *w = new gl3d_wall(st_tmp, ed_tmp, node.attribute("thickness").as_float(),
                                 node.attribute("height").as_float());
    // obj properties
    w->set_id(node.attribute("id").as_int());
    w->set_obj_type(w->type_wall);
    w->get_property()->scale_unit = scale::length_unit::wall;
    // load attachment
    w->start_point_fixed = node.attribute("st_fixed").as_bool();
    w->end_point_fixed = node.attribute("ed_fixed").as_bool();
    if (w->start_point_fixed)
        if (!w->get_start_point_attach()->load_from_xml(node.child("start_attach")))
            w->set_start_point_fixed(false);
    if (w->end_point_fixed)
        if (!w->get_end_point_attach()->load_from_xml(node.child("end_attach")))
            w->set_end_point_fixed(false);

    // wall sfcs
    auto nset = node.children("wall_sfc");
    QMap<int , klm::Surface *> tmp_sfcs;
    for (auto it = nset.begin();
            it != nset.end();
            it++) {
        int sid = it->attribute("sid").as_int();
        klm::Surface * s = new klm::Surface();
        if (s->load(it->child("surface")))
            tmp_sfcs.insert(sid, s);
    }
    for (int i = 0; i < tmp_sfcs.size(); i++) {
        w->sfcs.push_back(tmp_sfcs.value(i));
    }

    // wall holes
    nset = node.children("wall_hole");
    for (auto it = nset.begin();
            it != nset.end();
            it++) {
        hole * h = hole::load_from_xml(*it, w);
        if (h->is_valid())
            w->holes_on_this_wall.insert(h->get_hole_id(), h);
        else {
            delete h;
        }
    }
    return w;
}

void gl3d::get_face_from_root_surface(klm::Surface *sfc, QVector<math::triangle_facet> &faces) {
    // process local verticles
    const QVector<Surface::Vertex *> *vertexes = sfc->getRenderingVertices();
    const QVector<GLushort> *indecis = sfc->getRenderingIndices();
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