#ifndef GL3D_SCENE_H
#define GL3D_SCENE_H

#ifndef GL3D_OBJECT_H
#error "you should include gl3d_object.h befor this"
#endif

#ifndef gl3d_general_texture_hpp
#error "you should include gl3d_general_texture.hpp befor this"
#endif

#ifndef GL3D_VIEWER_H
#error "you should include gl3d_viewer.hpp befor this"
#endif

#ifndef shader_manager_hpp
#error "you should include shader_manager.hpp befor this"
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

// utils
#include "utils/gl3d_utils.h"

namespace gl3d {
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
        bool add_obj(QPair<int, object *> obj_key_pair);

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
        gl3d_general_texture * get_shadow_texture();

        /**
         *  @author Kent, 16-04-01 15:04:40
         *
         *  绘制模板
         */
        void draw_stencil();

        // 屏幕长宽
        GL3D_UTILS_PROPERTY(width, GLfloat);
        GL3D_UTILS_PROPERTY(height, GLfloat);

    private:
        void init();
        void set_attribute(GLuint pro);
        void draw_object(gl3d::object *obj, GLuint pro);
        shader_manager * shaders;
        QMap<int,gl3d::object *> * objects;
        scene_property this_property;
        light_property lights[4];
//        GLuint shadow_text;
        gl3d_general_texture * shadow_text;
    };
}

#endif // GL3D_SCENE_H
