#ifndef GL3D_MESH_H
#define GL3D_MESH_H

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
    /**
     *  @author Kent, 16-02-17 20:02:29
     *
     *  顶点数据的分布结构，每个顶点有12个float以及1个int，分别是
     *  xyz坐标，xyz法向量，rgba颜色以及透明度，xy贴图坐标，以及贴图资源id
     */
    typedef struct obj_points {
        GLfloat vertex_x;
        GLfloat vertex_y;
        GLfloat vertex_z;
        GLfloat normal_x;
        GLfloat normal_y;
        GLfloat normal_z;
        GLfloat color_r;
        GLfloat color_g;
        GLfloat color_b;
        GLfloat color_a;
        GLfloat texture_x;
        GLfloat texture_y;
        GLint texture_id;
    } obj_points;

    /**
     *  @author Kent, 16-02-17 20:02:39
     *
     *  贴图需要使用rgba格式的png文件，读取数据或者解析出数据之后可以用这个结构体设置贴图图片。每个贴图数据包含图片id，对应顶点数据的贴图id，以及图片数据大小（字节数），以及具体的图片数据
     */
    typedef struct obj_texture {
        GLint texture_id;
        GLint texture_length;
        GLchar * texture_data;
    } obj_texture;

    class mesh {
    public:
        friend class object;
        friend class scene;
    private:
        obj_points * points_data;
        GLushort * indecis;
        unsigned int num_idx;
        unsigned int num_pts;
        bool data_buffered;
        GLuint vbo;
        GLuint idx;
        int material_index;
        bool texture_repeat;
        glm::vec3 bounding_value_max;
        glm::vec3 bounding_value_min;

        mesh(void * as_mesh);

        // 多个mesh合并成一个mesh
        mesh(QVector<mesh *>& meshes);

        // 重新计算法向量，使得每个面都为平滑面
        void recalculate_normals(float cos_angle);

        // 从左手系转换到右手系
        void convert_left_hand_to_right_hand();

        // 测试用的加mesh构造函数
        mesh(obj_points * pts, int number_of_points,
             unsigned short * idx_in, int num_of_indecis);
        mesh(obj_points * pts, int number_of_points,
         unsigned short * indecis, int num_of_indecis,
         obj_texture * txtr, int number_of_textures);
        void buffer_data();
        ~mesh();
        void init();
    };
}

#endif // GL3D_MESH_H
