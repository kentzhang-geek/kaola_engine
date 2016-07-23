//
//  gl3d_viewer.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/3.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d.hpp"

using namespace gl3d;

/**
 *  @author Kent, 16-02-25 11:02:31
 *
 *  viewer相关的接口
 */
viewer::viewer(GLfloat h, GLfloat w) {
    // init with directions
    this->look_direction = ::glm::vec3(1.0, 0.0, 0.0);
    this->head_direction = ::glm::vec3(0.0, 1.0, 0.0);
    this->current_position = ::glm::vec3(0.0, 0.0, 0.0);
    this->projection_matrix = ::glm::mat4(0.0);
    this->viewing_matrix = ::glm::mat4(0.0);
    this->translationMatrix = ::glm::mat4(1.0);
    this->height = h;
    this->width = w;
    this->view_mode = viewer::normal_view;
    this->top_view_size = 30.0;
}

viewer::~viewer() {
}

void viewer::screen_change(GLfloat h, GLfloat w) {
    this->height = h;
    this->width = w;
    this->calculate_mat();
}

bool viewer::lookat(::glm::vec3 direction) {
    this->look_direction = direction;
    this->calculate_mat();
    return true;
}

bool viewer::headto(::glm::vec3 direction) {
    this->head_direction = direction;
    this->calculate_mat();
    return false;
}

bool viewer::position(::glm::vec3 position) {
    this->current_position = position;
    this->calculate_mat();
    return false;
}

bool viewer::change_position(::glm::vec3 val) {
    if (this->view_mode == viewer::normal_view) {
        // 前进值
        ::glm::vec3 f = val.y * ::glm::normalize(::glm::vec3(this->look_direction.x, 0.0, this->look_direction.z));
        // 平移值
        ::glm::vec3 r = glm::normalize(::glm::cross(::glm::vec3(this->look_direction.x, 0.0, this->look_direction.z), glm::vec3(0.0, 1.0, 0.0))) * val.x;
        this->current_position += (f + r);
    }
    if (this->view_mode == viewer::top_view) {
        val.x = -val.x;
        val.z = val.y;
        val.y = 0.0;
        this->current_position += val;
    }
    this->calculate_mat();
    return true;
}

bool viewer::go_raise(GLfloat angle) {
    // 旋转向量
    glm::vec3 tmp = glm::cross(this->look_direction, this->head_direction);
    glm::mat4 tmp_rotate_mat = glm::rotate(glm::mat4(1.0), glm::radians(-angle), tmp);
    glm::vec4 lookvec4 = glm::vec4(this->look_direction, 1.0);
    lookvec4 = lookvec4 * tmp_rotate_mat;
    lookvec4 = lookvec4 / lookvec4.w;
    this->look_direction = glm::vec3(lookvec4);
//    lookvec4 = glm::vec4(this->head_direction, 1.0);
//    lookvec4 = lookvec4 * tmp_rotate_mat;
    this->head_direction = glm::vec3(0.0, 1.0, 0.0);
    
    this->calculate_mat();
    return true;
}

bool viewer::go_rotate(GLfloat angle) {
    // 旋转向量
    glm::mat4 tmp_rotate_mat = glm::rotate(glm::mat4(1.0), glm::radians(angle), this->head_direction);
    glm::vec4 lookvec4 = glm::vec4(this->look_direction, 1.0);
    lookvec4 = lookvec4 * tmp_rotate_mat;
    this->look_direction = glm::vec3(lookvec4);
    
    this->calculate_mat();
    return false;
}

/**
 *  @author Kent, 16-03-01 11:03:38
 *
 *  通过向量计算出矩阵
 */
void viewer::calculate_mat() {
    glm::vec3 location =
            this->get_current_position() *
            gl3d::scale::shared_instance()->get_global_scale();
    if (this->view_mode == viewer::normal_view) {
        this->viewing_matrix = ::glm::lookAt(location, location + this->look_direction, this->head_direction);
        this->projection_matrix = glm::perspective(glm::radians(40.0f), (float)(this->width/this->height), 1.0f, 1000.0f);
    }
    if (this->view_mode == viewer::top_view) {
        ::glm::vec3 cp = location;
        cp.y = this->top_view_hight;
        auto lp = cp;
        lp.y = 0.0;
        this->viewing_matrix = ::glm::lookAt(cp, lp, glm::vec3(0.0, 0.0, 1.0));
        this->projection_matrix = glm::ortho
                (-this->top_view_size,
                 this->top_view_size,
                 -this->top_view_size / gl3d_global_param::shared_instance()->canvas_width *  gl3d_global_param::shared_instance()->canvas_height,
                 this->top_view_size / gl3d_global_param::shared_instance()->canvas_width *  gl3d_global_param::shared_instance()->canvas_height,
                 -100.0f, 500.0f);
    }
}

void viewer::set_normal_view() {
    this->view_mode = viewer::normal_view;
    this->calculate_mat();
}

void viewer::set_top_view() {
    this->view_mode = viewer::top_view;
    this->calculate_mat();
}

glm::vec3 viewer::get_scaled_position() {
    return this->get_current_position()
            * scale::shared_instance()->get_global_scale();

}

void viewer::coord_ground_project(glm::vec2 coord_input, glm::vec2 & coord_out, GLfloat hight) {
    glm::vec3 coord_in = glm::vec3(coord_input.x, coord_input.y, 0.0);
    coord_in.z = 1.0;
    glm::vec3 txxx = glm::unProject(coord_in,
                                    glm::mat4(1.0),
                                    this->projection_matrix * this->viewing_matrix,
                                    glm::vec4(0.0, 0.0, this->width,
                                              this->height));
    coord_in.z = 0.1;
    txxx = txxx - glm::unProject(coord_in,
                                 glm::mat4(1.0),
                                 this->projection_matrix * this->viewing_matrix,
                                 glm::vec4(0.0, 0.0, this->width, this->height));

    glm::vec3 near_pt = this->get_current_position();
    txxx = glm::normalize(txxx);
    glm::vec3 reallazer = txxx;  // 真实射线向量计算OK

    GLfloat param = (hight - near_pt.y) / reallazer.y;
    glm::vec3 tgt = near_pt + param * reallazer;
    //    log_c("we have tgt \n %f \n %f \n %f ", tgt.x, tgt.y, tgt.z);
    coord_out = glm::vec2(tgt.x, tgt.z);

    return;
}

void viewer::coord_ground_ortho(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight) {
    glm::vec4 tmp = glm::inverse(this->projection_matrix * this->viewing_matrix) *
            glm::vec4(coord_in.x, coord_in.y, 0.0, 1.0);
    coord_in = coord_in * 2.0f - 1.0f; // from (0,1) to (-1,1)
    coord_in.x = coord_in.x * this->top_view_size / scale::shared_instance()->get_global_scale();
    coord_in.y = coord_in.y
            * this->top_view_size
            / gl3d_global_param::shared_instance()->canvas_width
            * gl3d_global_param::shared_instance()->canvas_height
            / scale::shared_instance()->get_global_scale();
    coord_out = glm::vec2(this->current_position.x - coord_in.x, this->current_position.z - coord_in.y) ;
}

void viewer::coord_ground(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight) {
    if (this->view_mode == this->top_view) {
        this->coord_ground_ortho(coord_in, coord_out, hight);
    }
    else {
        this->coord_ground_project(coord_in, coord_out, hight);
    }
}
