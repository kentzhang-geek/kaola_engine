#include "utils/gl3d_utils.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_post_process.h"
#include "utils/gl3d_post_process_template.h"

using namespace gl3d;

class simple_directional_light : public gl3d_post_processer {
public:
    gl3d_general_texture *rend_light_pic(gl3d_general_texture *src, scene *scene);
    gl3d_general_texture *compose_pic(gl3d_general_texture *src, gl3d_general_texture *light, scene *scene);
    gl3d_general_texture * process(gl3d_general_texture *src, scene *scene);
    void deel_with_src(gl3d::gl3d_general_texture * src) {};
};
GL3D_ADD_POST_PROCESSER(simple_directional_light);

gl3d_general_texture * simple_directional_light::process(
        gl3d_general_texture *src, scene *scene) {
    gl3d_general_texture * lr =
            this->rend_light_pic(src, scene);
    return this->compose_pic(src, lr, scene);
}

gl3d_general_texture * simple_directional_light::rend_light_pic(
        gl3d_general_texture *src, scene *scene) {
    // 创建绘制光线效果的帧
    gl3d_framebuffer fb_light(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                              src->get_size_x(),
                              src->get_size_y());
    gl3d_general_texture * light_pic =
            new gl3d_general_texture(src->get_type(),
                                     src->get_size_x(),
                                     src->get_size_y());
    fb_light.attach_color_text(light_pic->get_text_obj());

    // 进行绘制前的准备
    GLuint pro = GL3D_GET_SHADER("simple_directional_light")->getProgramID();
    gl3d::shader_param * current_shader_param =
            ::gl3d::shader_manager::sharedInstance()->get_param("simple_directional_light");
    current_shader_param->user_data.insert(string("scene"), scene);
    scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_ALL;
    scene->get_property()->global_shader = "simple_directional_light";
    scene->prepare_canvas(true);
    glDisable(GL_CULL_FACE);

    // 遍历所有光源进行绘制
    for (auto it = scene->get_light_srcs()->begin();
         it != scene->get_light_srcs()->end();
         it++) {
        // 设置光照参数
        glUseProgram(pro);
        GL3D_SET_VEC3(light_location, (*it)->get_location(), pro);
        GL3D_SET_VEC3(light_to, (*it)->get_direction(), pro);
        glUniform1f(glGetUniformLocation(pro, "light_angle"),
                    (*it)->get_light_angle());
        glUniform1f(glGetUniformLocation(pro, "light_lum"),
                    10.0);
        glUniform1f(glGetUniformLocation(pro, "light_low_factor"),
                    0.0);
        // 绘制合成用的图形
        scene->draw(true);
    }

    // 去除光照绘制环境
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    fb_light.attach_color_text(0);

    return light_pic;
}

gl3d_general_texture * simple_directional_light::compose_pic(
        gl3d_general_texture *src,
        gl3d_general_texture * light,
        scene *scene) {
    // 准备将光照合成到最终呈现
    special_obj * sobj = new special_obj(src);
    // 设置光照成果图
    sobj->get_mtls()->value(
                sobj->get_meshes()->at(0)->get_material_index()
                )->textures.insert(gl3d_material::specular, light);
    gl3d_general_texture * dst = new gl3d_general_texture(src->get_type(),
                                                          src->get_size_x(),
                                                          src->get_size_y());
    gl3d::gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                              src->get_size_x(),
                              src->get_size_y());
    fb.attach_color_text(dst->get_text_obj());
    fb.use_this_frame();
    gl3d::shader_param * current_shader_param =
            ::gl3d::shader_manager::sharedInstance()->get_param("light_compose");
    current_shader_param->user_data.insert(string("scene"), scene);
    scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SPECIAL;
    scene->get_property()->global_shader = "light_compose";
    scene->add_obj(QPair<int, object *>(2333, sobj));
    scene->prepare_canvas(true);
    glDisable(GL_CULL_FACE);
    scene->draw(true);
    current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
    scene->delete_obj(2333);

    // tear down render env
    fb.attach_color_text(0);
    delete sobj;

    return dst;
}

GL3D_LOAD_SHADER(simple_directional_light,
                 simple_directional_light.vdata,
                 simple_directional_light.fdata);
GL3D_SHADER_PARAM(simple_directional_light) {
    GLuint pro = GL3D_GET_SHADER("simple_directional_light")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    return true;

}
GL3D_LOAD_SHADER(light_compose,
                 light_compose.vdata,
                 light_compose.fdata);
GL3D_SHADER_PARAM(light_compose) {
    GLuint pro = GL3D_GET_SHADER("light_compose")->getProgramID();
    gl3d::object * obj = GL3D_GET_OBJ();
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    return true;
}
