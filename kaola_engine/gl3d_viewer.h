#ifndef GL3D_VIEWER_H
#define GL3D_VIEWER_H

#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gl3d {
    // 一个场景会有一个观察者
    class viewer {
    public:
        enum _view_mode {
            normal_view = 0,
            top_view
        };
        // 设置普通视角
        void set_normal_view();
        // 设置顶视图
        void set_top_view();
        // 创建对象
        viewer(GLfloat height, GLfloat width);
        // 销毁对象
        ~viewer();
        // 旋转屏幕
        void screen_change(GLfloat height, GLfloat width);

        // 观察方向
        bool lookat(::glm::vec3 direction);
        // 头顶方向
        bool headto(::glm::vec3 direction);
        // 设置位置
        bool position(::glm::vec3 position);

        // 获取观察方向
        ::glm::vec3 * get_lookat();
        // 获取头顶方向
        ::glm::vec3 * get_headto();
        // 获取位置
        ::glm::vec3 * get_position();

        // 位移控制
        bool change_position(::glm::vec3 val);
        // 俯仰控制(单位为角度，最大+-90.0),抬头为正向
        bool go_raise(GLfloat angle);
        // 旋转控制(单位为角度，最大+-180.0),右转为正向
        bool go_rotate(GLfloat angle);

        friend class scene;
        ::glm::mat4 projection_matrix; // for test

    private:
        _view_mode view_mode;
        ::glm::vec3 look_direction;
        ::glm::vec3 head_direction;
        ::glm::vec3 current_position;
        GLfloat height;
        GLfloat width;

//        ::glm::mat4 projection_matrix;
        ::glm::mat4 viewing_matrix;
        ::glm::mat4 translationMatrix;
        void calculate_mat();
    };
}

#endif // GL3D_VIEWER_H
