//
//  gl3d.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/2/17.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_hpp
#define gl3d_hpp

/**
 *  @author Kent, 16-02-17 20:02:45
 *
 *  System Headers
 */
#include <stdio.h>
#include <OpenGL.h>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Program.hpp"
#include "log.h"
#include "shader_manager.hpp"
#include "gl3d_texture.hpp"
#include "gl3d_material.hpp"
#include "gl3d_obj_authority.h"
#include "gl3d_general_texture.hpp"
#include "gl3d_render_process.hpp"

namespace gl3d {
    enum error {
        UNDEFINED = 0,  //未定义错误
    };
    
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
        mesh(vector<mesh *>& meshes);
        
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
        map<string, void *> user_data;
        
        /**
         *  @author Kent, 16-03-16 16:03:55
         *
         *  各种材质对对应index的映射表
         */
        map<unsigned int, gl3d_material *> mtls;
        
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
        vector<mesh *> meshes;
        unsigned long long number_of_meshes;
        GLuint vao;
    };
    
    // 一个场景会有一个观察者
    class viewer {
    public:
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
    
    // 一个场景（scene）下有多个物件（object）与最多4个光源
    class scene {
    public:
        typedef struct scene_property {
            GLfloat screen_width;
            GLfloat screen_height;
            glm::vec3 background_color;
            string global_shader;
            GLuint64 current_draw_authority; // 当前绘制哪些obj，见gl3d_obj_authority.h
            //还有其他属性待加入
        } scene_property;
        
        typedef struct light_property {
            bool enable;
            glm::vec3 light_position;
            GLfloat ligth_brightness;
            //还有其他属性待加入
        }light_property;
        
        /**
         *  @author Kent, 16-02-25 11:02:38
         *
         *  用于控制摄像头的观察者
         */
        ::gl3d::viewer * watcher;
        
        scene(GLfloat h, GLfloat w);
        scene(GLfloat h, GLfloat w, scene_property * property);
        ~scene();
        
        bool init(scene_property * property);
        
        /**
         *  @author Kent, 16-02-17 21:02:19
         *
         *  在场景中加入物件，加入时需指定物件的唯一id
         *
         *  @param std::pair<int 物件的id
         *  @param obj_key_pair  物件本身的指针
         *
         *  @return 是否加入成功
         */
        bool add_obj(std::pair<int,gl3d::object *> obj_key_pair);

        /**
         *  @author Kent, 16-02-17 21:02:11
         *
         *  删除场景中的物件
         *
         *  @param key 物件的唯一id
         *
         *  @return 是否删除成功
         */
        bool delete_obj(int key);
        
        /**
         *  @author Kent, 16-02-17 21:02:16
         *
         *  根据key获取场景中的物件
         *
         *  @param key 物件的唯一id
         *
         *  @return 物件的指针
         */
        gl3d::object * get_obj(int key);
        
        /**
         *  @author Kent, 16-02-17 21:02:15
         *
         *  为渲染缓存数据
         *
         *  @return 是否缓存成功
         */
        bool prepare_buffer();
        
        /**
         *  @author Kent, 16-03-03 14:03:48
         *
         *  准备画布，设置背景色等等
         *
         *  @return 是否准备成功
         */
        bool prepare_canvas(bool use_global_shader);
        
        /**
         *  @author Kent, 16-02-17 21:02:16
         *
         *  进行数据渲染，这一步如果之前没有进行prepare_draw则什么也不做
         *
         *  @return 是否渲染成功
         */
        bool draw(bool use_global_shader);
        
        /**
         *  @author Kent, 16-02-17 21:02:29
         *
         *  设置场景的属性
         *
         *  @param property
         *
         *  @return 是否设置成功
         */
        bool set_property(scene_property * property);
        
        /**
         *  @author Kent, 16-02-17 21:02:55
         *
         *  获取场景的属性
         *
         *  @return 获取到的场景属性的指针
         */
        scene_property * get_property();
        
        /**
         *  @author Kent, 16-02-17 21:02:48
         *
         *  设置光源的属性
         *
         *  @param id_of_light 光源的id（0~3）
         *  @param property    光源的属性
         *
         *  @return 是否设置成功
         */
        bool set_light(int id_of_light, light_property * property);
        
        /**
         *  @author Kent, 16-02-17 21:02:35
         *
         *  获取光源的属性
         *
         *  @param id_of_light 光源的id
         *
         *  @return 光源属性的指针
         */
        light_property * get_light(int id_of_light);
        
        /**
         *  @author Kent, 16-02-18 15:02:29
         *
         *  根据坐标拾取object
         *
         *  @param x
         *  @param y
         *
         *  @return object的id
         */
        int get_object_id_by_coordination(int x, int y);
        
        /**
         *  @author Kent, 16-02-23 14:02:14
         *
         *  如果需要进行绘制，则pend_draw为true
         */
        bool pend_draw;
        
        /**
         *  @author Kent, 16-03-21 10:03:35
         *
         *  变更场景中某个对象的位置
         *
         *  @param obj        对象物品
         *  @param move_value 位置改变值
         *
         *  @return 是否变更成功
         */
        bool move_object(object * obj, glm::vec3 des_pos);
        
        /**
         *  @author Kent, 16-03-21 15:03:56
         *
         *  根据输入的屏幕坐标光线投射一个地面坐标
         *
         *  @param coord_in  输入的屏幕坐标
         *  @param coord_out 输出的地面坐标
         *  @param hight 输入的选取层高度
         */
        void coord_ground(glm::vec3 coord_in, glm::vec2 & coord_out);
        void coord_ground(glm::vec3 coord_in, glm::vec2 & coord_out, GLfloat hight);
        
        /**
         *  @author Kent, 16-03-22 15:03:26
         *
         *  返回当前点位置在obj上的高度
         *
         *  @param obj
         *  @param coord_in 输入屏幕坐标点
         *
         *  @return 点击位置的高度
         */
        GLfloat get_obj_hight(object * obj, glm::vec3 coord_in);
        
        /**
         *  @author Kent, 16-03-23 16:03:43
         *
         *  绘制阴影贴图，返回opengl贴图的句柄
         *
         *  @return opengl贴图的句柄
         */
        void draw_shadow_mask();
        
        /**
         *  @author Kent, 16-03-27 14:03:36
         *
         *  生成阴影贴图
         */
        void gen_shadow_texture();
        
        /**
         *  @author Kent, 16-03-27 14:03:42
         *
         *  释放阴影贴图
         */
        void delete_shadow_texture();
        
        /**
         *  @author Kent, 16-03-27 14:03:46
         *
         *  获取阴影贴图
         *
         *  @return 阴影贴图句柄
         */
        GLuint get_shadow_texture();
        
        /**
         *  @author Kent, 16-04-01 15:04:40
         *
         *  绘制模板
         */
        void draw_stencil();
        
        /**
         *  @author Kent, 16-02-17 21:02:42
         *
         *  下面都是私有数据
         */
    private:
        void init();
        void set_attribute(GLuint pro);
        void draw_object(gl3d::object *obj, GLuint pro);
        shader_manager * shaders;
        std::map<int,gl3d::object *> * objects;
        scene_property this_property;
        light_property lights[4];
        GLfloat width;
        GLfloat height;
//        GLuint shadow_text;
        gl3d_general_texture * shadow_text;
    };
}

// 工具宏
#define SHADER_LOCATE(x) [[[[NSBundle mainBundle] resourcePath]stringByAppendingString:[@"/Frameworks/kaola_engine.framework/" stringByAppendingString:[NSString stringWithUTF8String:(x)]]] UTF8String]

#define SHADER_SRC(x) [[NSString stringWithContentsOfFile: \
[NSString stringWithUTF8String : SHADER_LOCATE(x)] encoding: NSUTF8StringEncoding error:nil] UTF8String]

// 全局的沙盒位置
extern string gl3d_sandbox_path;
#define GL3D_INIT_SANDBOX_PATH(path) { \
gl3d_sandbox_path = std::string(path); }

#endif /* gl3d_hpp */
