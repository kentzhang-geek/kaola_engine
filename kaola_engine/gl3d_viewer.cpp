//
//  gl3d_viewer.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/3.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d.hpp"

using namespace gl3d;

/**
 *  @author Kent, 16-02-25 11:02:31
 *
 *  viewer相关的接口
 */
viewer::viewer(GLfloat h, GLfloat w) {
    // init with directions
    this->look_direction = ::glm::vec3(1.0, 0.0, 0.0);
    this->head_direction = ::glm::vec3(0.0, 0.0, 1.0);
    this->current_position = ::glm::vec3(0.0, 0.0, 0.0);
    this->projection_matrix = ::glm::mat4(0.0);
    this->viewing_matrix = ::glm::mat4(0.0);
    this->translationMatrix = ::glm::mat4(1.0);
    this->height = h;
    this->width = w;
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

::glm::vec3 * viewer::get_lookat() {
    return &this->look_direction;
}

::glm::vec3 * viewer::get_headto() {
    return &this->head_direction;
}

::glm::vec3 * viewer::get_position() {
    return &this->current_position;
}

bool viewer::change_position(::glm::vec3 val) {
    // 前进值
    ::glm::vec3 f = val.y * ::glm::normalize(::glm::vec3(this->look_direction.x, this->look_direction.y, 0.0));
    // 平移值
    ::glm::vec3 r = glm::normalize(::glm::cross(::glm::vec3(this->look_direction.x, this->look_direction.y, 0.0), glm::vec3(0.0, 0.0, 1.0))) * val.x;
    this->current_position += (f + r);
    this->calculate_mat();
    return true;
}

bool viewer::go_raise(GLfloat angle) {
    // 旋转向量
    glm::vec3 tmp = glm::cross(this->look_direction, this->head_direction);
    glm::mat4 tmp_rotate_mat = glm::rotate(glm::mat4(1.0), glm::radians(-angle), tmp);
    glm::vec4 lookvec4 = glm::vec4(this->look_direction, 1.0);
    lookvec4 = lookvec4 * tmp_rotate_mat;
    this->look_direction = glm::vec3(lookvec4);
    lookvec4 = glm::vec4(this->head_direction, 1.0);
    lookvec4 = lookvec4 * tmp_rotate_mat;
    this->head_direction = glm::vec3(lookvec4);
    
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
//    log_c("look at %f %f %f", this->look_direction.x, this->look_direction.y, this->look_direction.z);
    this->viewing_matrix = ::glm::lookAt(this->current_position, this->current_position + this->look_direction, this->head_direction);
    this->projection_matrix = glm::perspective(45.0f, (float)(this->width/this->height), 1.0f, 1000.0f);
}

