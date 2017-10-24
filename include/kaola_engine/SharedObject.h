//
// Created by Kent on 2017/10/24.
//

#ifndef QTTEST_SHAREDOBJECT_H
#define QTTEST_SHAREDOBJECT_H

#include "gl3d_abstract_object.h"

namespace gl3d {
    class object;
    class SharedObject : public gl3d::abstract_object {
    public:
        bool is_data_changed() override;
        bool is_visible() override;
        glm::vec3 getMaxBoundry() override;
        glm::vec3 getMinBoundry() override;
        glm::mat4 get_translation_mat() override;
        glm::mat4 get_rotation_mat() override;
        glm::mat4 get_scale_mat() override;
        void get_abstract_meshes(QVector<gl3d::mesh *> &ms) override;
        void clear_abstract_meshes(QVector<gl3d::mesh *> &ms) override;
        void get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) override;
        void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) override;
        void set_translation_mat(const glm::mat4 &trans) override;

        SharedObject(gl3d::object * srcObj, glm::mat4 trans, glm::mat4 rotation, glm::mat4 scale);
        static SharedObject * fork(gl3d::object * srcObj, glm::mat4 trans, glm::mat4 rotation, glm::mat4 scale);

    private:
        gl3d::object * sharedModel;
        glm::mat4 transMat;
        glm::mat4 rotationMat;
        glm::mat4 scaleMat;
    };
}

#endif //QTTEST_SHAREDOBJECT_H
