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

#include "utils/gl3d_utils.h"
#include "utils/gl3d_global_param.h"

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

        // 位移控制
        bool change_position(::glm::vec3 val);
        // 俯仰控制(单位为角度，最大+-90.0),抬头为正向
        bool go_raise(GLfloat angle);
        // 旋转控制(单位为角度，最大+-180.0),右转为正向
        bool go_rotate(GLfloat angle);

        friend class scene;

        // location, and directions
        GL3D_UTILS_PROPERTY(current_position, ::glm::vec3);
        GL3D_UTILS_PROPERTY(head_direction, ::glm::vec3);
        GL3D_UTILS_PROPERTY(look_direction, ::glm::vec3);

        // properties
        GL3D_UTILS_PROPERTY(height, GLfloat);
        GL3D_UTILS_PROPERTY(width, GLfloat);
        GL3D_UTILS_PROPERTY_GET_POINTER(projection_matrix,
                                        ::glm::mat4);
        GL3D_UTILS_PROPERTY_GET_POINTER(viewing_matrix,
                                        ::glm::mat4);
        GL3D_UTILS_PROPERTY_GET_POINTER(translationMatrix,
                                        ::glm::mat4);
        GL3D_UTILS_PROPERTY(top_view_size, GLfloat);
        GL3D_UTILS_PROPERTY(view_mode, _view_mode);

        // const value
        const float top_view_hight = 10.0f;

        // get scaled postion
        glm::vec3 get_scaled_position();

        // pick up
        void coord_ground_project(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight);
        void coord_ground_ortho(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight);
        void coord_ground(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight);

    private:
        void calculate_mat();
    };
}

#endif // GL3D_VIEWER_H
