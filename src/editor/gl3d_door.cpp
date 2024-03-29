#include "editor/gl3d_door.h"
#include "editor/surface.h"

using namespace gl3d;
using namespace std;

std::string gl3d_door::default_model_res_id = "door000000";

void gl3d_door::init() {
    this->attached_hole_id = -1;
    this->attached_wall_id = -1;
    this->pre_translate_mat = glm::mat4(1.0f);
    this->trans_mat = glm::mat4(1.0f);
    this->rotate_mat = glm::mat4(1.0f);
    this->pre_scale_mat = glm::mat4(1.0f);
    this->width = 0.0f;
    this->height = 0.0f;
    this->thickness = 0.0f;

    this->set_obj_type(this->type_door);
    this->set_id(0);
    this->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
    this->set_render_authority(GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_NORMAL);
    return;
}

gl3d_door::gl3d_door(string model_res) {
    this->init();
    this->door_model = new gl3d::object(
            (char *) klm::resource::manager::shared_instance()->get_res_item(model_res).c_str(),
            klm::resource::manager::get_base_path_by_resid(model_res));
    this->door_model->set_res_id(model_res);
    this->door_model->set_obj_type(abstract_object::type_door);
    // set model properties
    this->door_model->get_property()->scale_unit = gl3d::scale::mm;
    this->door_model->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
    this->door_model->set_render_authority(GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_NORMAL);
//  this->door_model->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
//  this->door_model->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    this->door_model->pre_scale();
    this->door_model->merge_meshes();
    this->door_model->recalculate_normals();
    this->door_model->convert_left_hand_to_right_hand();
    // pre calculate mat
    this->door_model->recalculate_boundings();
    glm::vec3 bottom_center = this->door_model->get_property()->bounding_value_min + this->door_model->get_property()->bounding_value_max;
    bottom_center = bottom_center / 2.0f;
    bottom_center.y = 0.0f; // right hand coordinates
    this->pre_translate_mat = glm::translate(glm::mat4(1.0f), -bottom_center);
//    this->pre_translate_mat = glm::mat4(1.0f);
}

gl3d_door::~gl3d_door() {
    // delete hole
    if (this->attached_wall_id > 0)
        if (this->attached_hole_id >= 0) {
            gl3d::scene * msc = (gl3d::scene *) gl3d::gl3d_global_param::shared_instance()->main_scene;
            gl3d_wall * wall = (gl3d_wall *) msc->get_attached_sketch()->get_obj(this->attached_wall_id);
            if ((wall != NULL) && (wall->get_obj_type() == wall->type_wall)) {
                hole * h = wall->holes_on_this_wall.value(this->attached_hole_id);
                wall->holes_on_this_wall.remove(this->attached_hole_id);
                delete h;
                wall->calculate_mesh();
            }
        }
    // delete model
    delete this->door_model;
    return;
}

bool gl3d_door::install_to_wall(gl3d_wall *wall, glm::vec2 center_point, float t_width, float t_height) {
    math::line_2d w_line(wall->get_start_point(), wall->get_end_point());
    if (!w_line.point_on_line(center_point)) {
        // center not on wall so install failed
        return false;
    }

    // set properties
    this->width = t_width;
    this->height = t_height;
    this->center_pt = center_point;
    this->start_pt = center_point + glm::normalize(wall->get_start_point() - center_point) * t_width * 0.5;
    this->end_pt = center_point + glm::normalize(wall->get_end_point() - center_point) * t_width * 0.5;

    gl3d::hole * n_h = new hole(wall, math::convert_vec2_to_vec3(center_point), t_width, 0.0f, t_height, wall->get_availble_hole_id());
    if (n_h->is_valid()) {
        this->attached_hole_id = n_h->get_hole_id();
        this->attached_wall_id = wall->get_id();
        // calculate mat
        this->trans_mat = glm::translate(glm::mat4(1.0f), math::convert_vec2_to_vec3(this->center_pt));
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
        this->scale_to_install(wall->get_thickness());
        // wall recalculate mesh
        wall->calculate_mesh();
        return true;
    }
}

void gl3d_door::scale_to_install(float thick) {
    glm::vec3 b_max = this->door_model->get_property()->bounding_value_max;
    glm::vec3 b_min = this->door_model->get_property()->bounding_value_min;
    glm::vec3 bounding = b_max - b_min;
    glm::vec3 hole_bounding = glm::vec3(
            this->width,
            this->height,
            thick);
    this->pre_scale_mat = glm::scale(glm::mat4(1.0f), hole_bounding / bounding);

    return;
}

bool gl3d_door::is_valid() {
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

bool gl3d_door::is_data_changed() {
    return false;
}

bool gl3d_door::is_visible() {
    return true;
}

glm::mat4 gl3d_door::get_translation_mat() {
    return this->trans_mat;
}

glm::mat4 gl3d_door::get_rotation_mat() {
    return this->rotate_mat * this->pre_scale_mat * this->pre_translate_mat;
}

void gl3d_door::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    this->door_model->buffer_data();
    this->door_model->get_abstract_meshes(ms);
    Q_FOREACH(gl3d::mesh * mit, ms) {
            mit->set_is_blink(this->get_pick_flag());
        }
    return ;
}

void gl3d_door::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    this->door_model->buffer_data();
    this->door_model->get_abstract_mtls(mt);
    return;
}

void gl3d_door::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    this->door_model->clear_abstract_meshes(ms);
    return;
}

void gl3d_door::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    this->door_model->clear_abstract_mtls(mt);
    return;
}

glm::mat4 gl3d_door::get_scale_mat() {
    return glm::mat4(1.0f);
}

// can not move use this
void gl3d_door::set_translation_mat(const glm::mat4 &trans) {
    return;
}

using namespace pugi;
bool gl3d_door::save_to_xml(pugi::xml_node &node) {
    node.append_attribute("type").set_value("gl3d_door");
    // properties
    node.append_attribute("obj_id").set_value(this->get_id());
    node.append_attribute("height").set_value(this->height);
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
    nd = node.append_child("door_model");
    this->door_model->save_to_xml(nd);
    node.append_attribute("model_res_id").set_value(this->door_model->get_res_id().c_str());
    return true;
}

gl3d_door* gl3d_door::load_from_xml(pugi::xml_node node) {
    QString type(node.attribute("type").value());
    if (type != "gl3d_door")
        return NULL;
    // properties
    std::string res_id = std::string(node.attribute("model_res_id").value());
    gl3d_door * door = new gl3d_door(res_id);
    door->set_id(node.attribute("obj_id").as_int());
    door->height = node.attribute("height").as_float();
    door->width = node.attribute("width").as_float();
    door->thickness = node.attribute("thickness").as_float();
    door->attached_wall_id = node.attribute("attached_wall_id").as_int();
    door->attached_hole_id = node.attribute("attached_hole_id").as_int();
    xml::load_xml_to_vec(node.child("start_pt"), door->start_pt);
    xml::load_xml_to_vec(node.child("end_pt"), door->end_pt);
    xml::load_xml_to_vec(node.child("center_pt"), door->center_pt);
    xml::load_xml_to_mat(node.child("pre_translate_mat"), door->pre_translate_mat);
    xml::load_xml_to_mat(node.child("trans_mat"), door->trans_mat);
    xml::load_xml_to_mat(node.child("rotate_mat"), door->rotate_mat);
    door->door_model->set_obj_type(door->door_model->type_door);

    // install
    scene * scs = (scene *)gl3d_global_param::shared_instance()->main_scene;
    gl3d_wall * w = (gl3d_wall *)scs->get_attached_sketch()->get_obj(door->attached_wall_id);
    if (w->holes_on_this_wall.contains(door->attached_hole_id)) {
        delete w->holes_on_this_wall.value(door->attached_hole_id);
        w->holes_on_this_wall.remove(door->attached_hole_id);
    }
    door->install_to_wall(w, door->center_pt, door->width, door->height);

    return door;
}

void gl3d_door::change_model(std::string res_id) {
    object * o_del = this->door_model;
    // load new model
    this->door_model = new gl3d::object(
            (char *) klm::resource::manager::shared_instance()->get_res_item(res_id).c_str(),
            klm::resource::manager::get_base_path_by_resid(res_id));
    this->door_model->set_res_id(res_id);
    this->door_model->set_obj_type(abstract_object::type_door);
    // set model properties
    this->door_model->get_property()->scale_unit = gl3d::scale::mm;
    this->door_model->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
    this->door_model->set_render_authority(GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_NORMAL);
    this->door_model->pre_scale();
    this->door_model->merge_meshes();
    this->door_model->recalculate_normals();
    this->door_model->convert_left_hand_to_right_hand();
    // pre calculate mat
    this->door_model->recalculate_boundings();
    glm::vec3 bottom_center = this->door_model->get_property()->bounding_value_min + this->door_model->get_property()->bounding_value_max;
    bottom_center = bottom_center / 2.0f;
    bottom_center.y = 0.0f; // right hand coordinates
    this->pre_translate_mat = glm::translate(glm::mat4(1.0f), -bottom_center);
    this->scale_to_install(this->thickness);
}

