#ifndef GL3D_OBJECT_H
#define GL3D_OBJECT_H

#ifndef GL3D_MESH_H
#error "you should include gl3d_mesh.h before include this"
#endif

#ifndef gl3d_material_hpp
#error "you should include gl3d_material.h before include this"
#endif

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

using namespace std;

namespace gl3d {
    /**
     *  @author Kent, 16-02-17 20:02:55
     *
     *  每一个模型，都有其特定属性，比如位置和旋转，是否反射是否镜面等
     */
    typedef struct obj_property {
        glm::vec3 position;
//        glm::vec3 rotate_axis;
//        GLfloat rotate_angle;
        GLfloat scale_range;
        GLuint id;
        glm::mat4 rotate_mat;
        GLuint64 authority;     //64种控制权限，在gl3d_obj_authority.h中查看
        GLuint64 draw_authority; // 64种绘制权限，在gl3d_obj_authority.h中查看
        glm::vec3 bounding_value_max;
        glm::vec3 bounding_value_min;
        //还有其他属性待加入
    } obj_property;

    // 每一个模型，都是一个物件（object）
    class object{
    public:
        /**
         *  @author Kent, 16-02-17 20:02:05
         *
         *  默认构造函数，除了创建对象之外什么也不做
         */
        object();
        ~object();

        /**
         *  @author Kent, 16-02-17 20:02:21
         *
         *  可以使用obj格式文件的数据创建object
         */
        object(char * path_to_obj_file);

        /**
         *  @author Kent, 16-02-17 20:02:26
         *
         *  也可以直接用obj创建object
         */
        object(obj_points * pts, int number_of_points,
               unsigned short * indecis, int num_of_indecis);

        /**
         *  @author Kent, 16-02-17 20:02:01
         *
         *  有贴图的情况下，创建时可以加入texture数据
         */
        object(obj_points * pts, int number_of_points,
               unsigned short * indecis, int num_of_indecis,
               obj_texture * textures, int number_of_textures);

        /**
         也可以创建obj之后用init进行初始化

         - returns: 是否初始化成功
         */
        bool init(char * path_to_obj_file);
        bool init(obj_points * pts, int number_of_points,
                  unsigned short * indecis, int num_of_indecis);
        bool init(obj_points * pts, int number_of_points,
                  unsigned short * indecis, int num_of_indecis,
                  obj_texture * textures, int number_of_textures);

        /**
         *  @author Kent, 16-02-17 20:02:55
         *
         *  设置当前obj的属性
         *
         *  @param property 设置的属性
         *
         *  @return 是否设置成功
         */
        bool set_property(obj_property * property);

        /**
         *  @author Kent, 16-02-17 20:02:34
         *
         *  获取当前obj的属性
         *
         *  @return 属性数据的指针
         */
        obj_property * get_property();

        /**
         *  @author Kent, 16-03-09 12:03:54
         *
         *  Object指定着色器的名字
         */
        std::string use_shader;

        /**
         *  @author Kent, 16-03-11 11:03:18
         *
         *  自己创建map时，可以指定各种数据
         */
        QMap<string, void *> user_data;

        /**
         *  @author Kent, 16-03-16 16:03:55
         *
         *  各种材质对对应index的映射表
         */
        QMap<unsigned int, gl3d_material *> mtls;

        /**
         *  @author Kent, 16-03-21 13:03:37
         *
         *  旋转当前物品
         *
         *  @param axis  旋转轴
         *  @param angle 旋转角度
         *
         *  @return 是否旋转成功
         */
        bool rotate(glm::vec3 axis, GLfloat angle);

        /**
         *  @author Kent, 16-03-30 10:03:00
         *
         *  设置使用新材质
         *
         *  @param mtl 新材质对象
         *
         *  @return 是否设置成功
         */
        bool use_mtl(string file_name, int mtl_id);

        /**
         *  @author Kent, 16-04-11 14:04:04
         *
         *  设置当前object是否使用全局纹理贴图都重复，
         *  通常用于天空盒贴图
         *
         *  @param repeat 是否重复
         */
        void set_repeat(bool repeat);

        /**
         *  @author Kent, 16-04-14 13:04:59
         *
         *  合并顶点组
         */
        void merge_meshes();

        // 重新计算法向量，使得每个面都为平滑面，输入参数为允许平滑的夹角，
        // 当原法向量与平滑法向量夹角的余弦大于给定值时，使用平滑法向量
        // 小于给定值时，使用重算法向量，默认给定值为cos(45)~0.707
        void recalculate_normals(float cos_angle = 0.707);

        /**
         *  @author Kent, 16-02-17 20:02:41
         *
         *  下面的都是私有数据块，不被外部调用者所控制
         */
        friend class scene;
        friend class model_manager;
        GLfloat param_x;

    private:
        void init();
        void buffer_data();
        obj_property this_property;
        bool data_buffered;
        QVector<mesh *> meshes;
        unsigned long long number_of_meshes;
        GLuint vao;
    };
}

#endif // GL3D_OBJECT_H
