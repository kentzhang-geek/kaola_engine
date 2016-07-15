#ifndef GL3D_POST_PROCESS_TEMPLATE_H
#define GL3D_POST_PROCESS_TEMPLATE_H

#include "kaola_engine/gl3d_post_process.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/shader_manager.hpp"

using namespace gl3d;

static gl3d::obj_points post_rect[4] = {
    {-1.0, 1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
        0.0, 1.0,     // text
        0},
    {-1.0, -1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
        0.0, 0.0,     // text
        0},
    {1.0, 1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
        1.0, 1.0,     // text
        0},
    {1.0, -1.0, 0.0,  // vt
     0.0, 0.0, 0.0,   // normal
     0.0, 0.0, 0.0, 0.0, // color
        1.0, 0.0,     // text
        0},
};

static GLushort post_indexes[6] = {
    0, 2, 1,
    1, 2, 3,
};

class special_obj : public gl3d::object {
public:
    special_obj(gl3d_general_texture * p_tex) : object(post_rect, 4, post_indexes, 6) {
        gl3d_material * p_mat = new gl3d_material(p_tex);
        this->get_meshes()->at(0)->set_bounding_value_max(glm::vec3(1.0, 1.0, 0.0));
        this->get_meshes()->at(0)->set_bounding_value_max(glm::vec3(-1.0, -1.0, 0.0));
        this->get_meshes()->at(0)->set_material_index(0);
        this->get_property()->authority = GL3D_OBJ_ENABLE_DEL;
        this->get_mtls()->insert(0, p_mat);
        this->get_property()->draw_authority = GL3D_SCENE_DRAW_SPECIAL;
    }
};

#define GL3D_POST_PROCESS_IMPLEMENT(name) \
class gl3d_post_#name : public gl3d_post_processer { \
public: \
    gl3d_general_texture * process(gl3d_general_texture *src, scene *one_scene) { \
        special_obj * sobj = new special_obj(src); \
        gl3d_general_texture * dst = new gl3d_general_texture(src->get_type(), \
                                                              src->get_size_x(), \
                                                              src->get_size_y()); \
        gl3d::gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF, \
                                  src->get_size_x(), \
                                  src->get_size_y());  \
        fb.attach_color_text(dst->get_text_obj()); \
        gl3d::shader_param * current_shader_param = GL3D_GET_PARAM(##name); \
        current_shader_param->user_data.insert(string("scene"), one_scene); \
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SPECIAL; \
        one_scene->get_property()->global_shader = string(##name); \
        one_scene->add_obj(QPair<int, object *>(2333, sobj)); \
        one_scene->prepare_canvas(true); \
        glDisable(GL_CULL_FACE); \
        one_scene->draw(true); \
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene"))); \
        one_scene->delete_obj(2333); \
        delete sobj; \
        return dst; \
    } \
    void deel_with_src(gl3d_general_texture * src) {}; \
}; \
GL3D_ADD_POST_PROCESSER(gl3d_post_#name); \
GL3D_LOAD_SHADER(name, \
                 name#.vdata, \
                 name#.fdata); \
GL3D_SHADER_PARAM(name) { \
    GLuint pro = GL3D_GET_SHADER(##name)->getProgramID(); \
    gl3d::object * obj = GL3D_GET_OBJ(); \
    glUniform1f(glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3); \
    glUniform1f(glGetUniformLocation(pro, "shininess"), 0.5); \
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_ambient"), 0); \
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1); \
    glUniform1i(glGetUniformLocation(pro, "gl3d_texture_specular"), 2); \
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0); \
    GL3D_SET_VEC3(light_vector, light, pro); \
    return true; \
}


#endif // GL3D_POST_PROCESS_TEMPLATE_H
