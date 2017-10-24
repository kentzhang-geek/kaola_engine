//
// Created by Kent on 2017/10/24.
//

#include "kaola_engine/SharedObject.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_object.h"

bool SharedObject::is_data_changed() {
    return sharedModel->is_data_changed();
}

bool SharedObject::is_visible() {
    return true;
}

glm::vec3 SharedObject::getMaxBoundry() {
    return sharedModel->getMaxBoundry();
}

glm::vec3 SharedObject::getMinBoundry() {
    return sharedModel->getMinBoundry();
}

glm::mat4 SharedObject::get_translation_mat() {
    return transMat;
}

glm::mat4 SharedObject::get_rotation_mat() {
    return rotationMat;
}

glm::mat4 SharedObject::get_scale_mat() {
    return scaleMat;
}

void SharedObject::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    sharedModel->get_abstract_meshes(ms);
}

void SharedObject::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    sharedModel->clear_abstract_meshes(ms);
}

void SharedObject::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    sharedModel->get_abstract_mtls(mt);
}

void SharedObject::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    sharedModel->clear_abstract_mtls(mt);
}

void SharedObject::set_translation_mat(const glm::mat4 &trans) {
    glm::vec3 newpos = glm::vec3(trans * glm::vec4(0.0, 0.0, 0.0, 1.0));
    this->transMat = glm::translate(glm::mat4(1.0f), newpos);
}

SharedObject::SharedObject(gl3d::object *srcObj, glm::mat4 trans, glm::mat4 rotation, glm::mat4 scale) {
    this->sharedModel = srcObj;
    this->transMat = trans;
    this->rotationMat = rotation;
    this->scaleMat = scale;
    return;
}

SharedObject* SharedObject::fork(gl3d::object *srcObj, glm::mat4 trans, glm::mat4 rotation, glm::mat4 scale) {
    return new SharedObject(srcObj, trans, rotation, scale);
}