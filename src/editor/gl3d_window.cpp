#include "editor/gl3d_window.h"
#include "editor/surface.h"

using namespace gl3d;
using namespace std;

std::string gl3d_window::default_model_res_id = "window000000";

void gl3d_window::init() {
    this->attached_hole_id = -1;
    this->attached_wall_id = -1;
    this->pre_translate_mat = glm::mat4(1.0f);
    this->trans_mat = glm::mat4(1.0f);
    this->rotate_mat = glm::mat4(1.0f);
    this->pre_scale_mat = glm::mat4(1.0f);
    this->width = 0.0f;
    this->height_max = 0.0f;
    this->height_min = 0.0f;
    this->thickness = 0.0f;

    this->set_obj_type(this->type_window);
    this->set_id(0);
    this->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
    this->set_render_authority(GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_NORMAL);
    return;
}

gl3d_window::gl3d_window(string model_res) {
    this->init();
    this->window_model = new gl3d::object(
            (char *) klm::resource::manager::shared_instance()->get_res_item(model_res).c_str());
    this->window_model->set_res_id(model_res);
    this->window_model->set_obj_type(abstract_object::type_window);
    // set model properties
    this->window_model->get_property()->scale_unit = gl3d::scale::mm;
    this->window_model->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
    this->window_model->set_render_authority(GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_NORMAL);
//  this->window_model->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
//  this->window_model->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    this->window_model->pre_scale();
    this->window_model->merge_meshes();
    this->window_model->recalculate_normals();
    this->window_model->convert_left_hand_to_right_hand();
    // pre calculate mat
    this->window_model->recalculate_boundings();
    glm::vec3 bottom_center = this->window_model->get_property()->bounding_value_min + this->window_model->get_property()->bounding_value_max;
    bottom_center = bottom_center / 2.0f;
    bottom_center.y = 0.0f; // right hand coordinates
    this->pre_translate_mat = glm::translate(glm::mat4(1.0f), -bottom_center);
//    this->pre_translate_mat = glm::mat4(1.0f);
}

gl3d_window::~gl3d_window() {
    // delete hole
    if (this->attached_wall_id > 0)
        if (this->attached_hole_id > 0) {
            gl3d::scene * msc = (gl3d::scene *) gl3d::gl3d_global_param::shared_instance()->main_scene;
            gl3d_wall * wall = (gl3d_wall *) msc->get_attached_sketch()->get_obj(this->attached_wall_id);
            if (wall->get_obj_type() == wall->type_wall) {
                hole * h = wall->holes_on_this_wall.value(this->attached_hole_id);
                wall->holes_on_this_wall.remove(this->attached_hole_id);
                delete h;
                wall->calculate_mesh();
            }
        }
    // delete model
    delete this->window_model;
    return;
}

bool gl3d_window::install_to_wall(gl3d_wall *wall, glm::vec2 center_point, float t_width, float height_ceil,
                                  float height_floor) {
    math::line_2d w_line(wall->get_start_point(), wall->get_end_point());
    if (!w_line.point_on_line(center_point)) {
        // center not on wall so isntall failed
        return false;
    }

    // set properties
    this->width = t_width;
    this->height_max = height_ceil;
    this->height_min = height_floor;
    this->center_pt = center_point;
    this->start_pt = center_point + glm::normalize(wall->get_start_point() - center_point) * t_width * 0.5;
    this->end_pt = center_point + glm::normalize(wall->get_end_point() - center_point) * t_width * 0.5;

    gl3d::hole * n_h = new hole(wall, math::convert_vec2_to_vec3(center_point), t_width, height_floor, height_ceil, wall->get_availble_hole_id());
    if (n_h->is_valid()) {
        this->attached_hole_id = n_h->get_hole_id();
        this->attached_wall_id = wall->get_id();
        // calculate mat
        glm::vec3 ins_pos = math::convert_vec2_to_vec3(this->center_pt);
        ins_pos.y = this->height_min;
        this->trans_mat = glm::translate(glm::mat4(1.0f), ins_pos);
        glm::vec3 dir_model = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 dir_wall = math::convert_vec2_to_vec3(wall->get_end_point() - wall->get_start_point());
        dir_wall.y = 0.0f;
        dir_wall = glm::normalize(dir_wall);
        glm::vec3 tmp = glm::cross(dir_model, dir_wall);
        float rot_radian = glm::dot(dir_model, dir_wall);
        rot_radian = glm::acos(rot_radian);
        if (tmp.y < 0)
            rot_radian = -rot_radian;
        this->rotate_mat = glm::rotate(glm::mat4(1.0f), rot_radian, glm::vec3(0.0f, 1.0f, 0.0f));
        // scale model
        this->thickness = wall->get_thickness();
        this->scale_to_install(this->thickness);
        // wall recalculate mesh
        wall->calculate_mesh();
        return true;
    }
}

void gl3d_window::scale_to_install(float tkness) {
    glm::vec3 b_max = this->window_model->get_property()->bounding_value_max;
    glm::vec3 b_min = this->window_model->get_property()->bounding_value_min;
    glm::vec3 bounding = b_max - b_min;
    glm::vec3 hole_bounding = glm::vec3(
            this->width,
            this->height_max - this->height_min,
            tkness);
    this->pre_scale_mat = glm::scale(glm::mat4(1.0f), hole_bounding / bounding);

    return;
}

bool gl3d_window::is_valid() {
    if (this->attached_wall_id > 0)
    if (this->attached_hole_id > 0) {
        gl3d::scene * msc = (gl3d::scene *) gl3d::gl3d_global_param::shared_instance()->main_scene;
        gl3d_wall * wall = (gl3d_wall *) msc->get_attached_sketch()->get_obj(this->attached_wall_id);
        if (wall->get_obj_type() == wall->type_wall) {
            math::line_2d w_ln(wall->get_start_point(), wall->get_end_point());
            if (w_ln.point_on_line(this->center_pt)) {
                return true;
            }
        }
    }

    return false;
}

bool gl3d_window::is_data_changed() {
    return false;
}

bool gl3d_window::is_visible() {
    return true;
}

glm::mat4 gl3d_window::get_translation_mat() {
    return this->trans_mat;
}

glm::mat4 gl3d_window::get_rotation_mat() {
    return this->rotate_mat * this->pre_scale_mat * this->pre_translate_mat;
}

void gl3d_window::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    this->window_model->buffer_data();
    this->window_model->get_abstract_meshes(ms);
    return ;
}

void gl3d_window::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    this->window_model->buffer_data();
    this->window_model->get_abstract_mtls(mt);
    return;
}

void gl3d_window::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    this->window_model->clear_abstract_meshes(ms);
    return;
}

void gl3d_window::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    this->window_model->clear_abstract_mtls(mt);
    return;
}

glm::mat4 gl3d_window::get_scale_mat() {
    return glm::mat4(1.0f);
}

// can not move use this
void gl3d_window::set_translation_mat(const glm::mat4 &trans) {
    return;
}

using namespace pugi;
bool gl3d_window::save_to_xml(pugi::xml_node &node) {
    node.append_attribute("type").set_value("gl3d_window");
    // properties
    node.append_attribute("height_min").set_value(this->height_min);
    node.append_attribute("height_max").set_value(this->height_max);
    node.append_attribute("width").set_value(this->width);
    node.append_attribute("thickness").set_value(this->thickness);
    node.append_attribute("attached_wall_id").set_value(this->attached_wall_id);
    node.append_attribute("attached_hole_id").set_value(this->attached_hole_id);
    pugi::xml_node nd;
    nd = node.append_child("start_pt");
    xml::save_vec_to_xml(this->start_pt, nd);
    nd = node.append_child("end_pt");
    xml::save_vec_to_xml(this->end_pt, nd);
    nd = node.append_child("center_pt");
    xml::save_vec_to_xml(this->center_pt, nd);
    nd = node.append_child("pre_translate_mat");
    xml::save_mat_to_xml(this->pre_translate_mat, nd);
    nd = node.append_child("trans_mat");
    xml::save_mat_to_xml(this->trans_mat, nd);
    nd = node.append_child("rotate_mat");
    xml::save_mat_to_xml(this->rotate_mat, nd);
    nd = node.append_child("window_model");
    this->window_model->save_to_xml(nd);
    node.append_attribute("model_res_id").set_value(this->window_model->get_res_id().c_str());
    return true;
}

gl3d_window* gl3d_window::load_from_xml(pugi::xml_node node) {
    QString type(node.attribute("type").value());
    if (type != "gl3d_window")
        return NULL;
    // properties
    std::string res_id = std::string(node.attribute("model_res_id").value());
    gl3d_window * win = new gl3d_window(res_id);
    win->height_max = node.attribute("height_max").as_float();
    win->height_min = node.attribute("height_min").as_float();
    win->width = node.attribute("width").as_float();
    win->thickness = node.attribute("thickness").as_float();
    win->attached_wall_id = node.attribute("attached_wall_id").as_int();
    win->attached_hole_id = node.attribute("attached_hole_id").as_int();
    xml::load_xml_to_vec(node.child("start_pt"), win->start_pt);
    xml::load_xml_to_vec(node.child("end_pt"), win->end_pt);
    xml::load_xml_to_vec(node.child("center_pt"), win->center_pt);
    xml::load_xml_to_mat(node.child("pre_translate_mat"), win->pre_translate_mat);
    xml::load_xml_to_mat(node.child("trans_mat"), win->trans_mat);
    xml::load_xml_to_mat(node.child("rotate_mat"), win->rotate_mat);
    win->window_model->set_obj_type(win->window_model->type_window);

    // install
    scene * scs = (scene *)gl3d_global_param::shared_instance()->main_scene;
    gl3d_wall * w = (gl3d_wall *)scs->get_attached_sketch()->get_obj(win->attached_wall_id);
    if (w->holes_on_this_wall.contains(win->attached_hole_id)) {
        delete w->holes_on_this_wall.value(win->attached_hole_id);
        w->holes_on_this_wall.remove(win->attached_hole_id);
    }
    win->install_to_wall(w, win->center_pt, win->width, win->height_max, win->height_min);
    return win;
}
