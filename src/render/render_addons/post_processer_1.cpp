#include "utils/gl3d_post_process_template.h"

//GL3D_POST_PROCESS_IMPLEMENT(post_process_gauss);
//GL3D_POST_PROCESS_IMPLEMENT(hdr_test);

#if 1

class hdr_test : public gl3d_post_processer {
public:
    float get_lum(GLubyte * pixs) {
        return 0.0;
    }

    gl3d_general_texture * process(gl3d_general_texture *src, scene *one_scene){
        special_obj * sobj = new special_obj(src);

        // get lum_max and lum_min
        GLubyte pix[4];
        const int steps = 16;
        float lum_max = 0.0;
        float lum_min = 1.0;
        gl3d::gl3d_framebuffer tmp_fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                                      src->get_size_x(),
                                      src->get_size_y());
        tmp_fb.attach_color_text(src->get_text_obj());
        tmp_fb.use_this_frame();
        float lum;
        for (int x = 0; x < steps; x++) {
            for (int y = 0; y < steps; y++) {
                GL3D_GL()->glReadPixels(x / steps * src->get_size_x(),
                             y / steps * src->get_size_y(),
                             1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pix);
                lum = this->get_lum(pix);
            }
        }

        gl3d_general_texture * dst = new gl3d_general_texture(src->get_type(),
                                                              src->get_size_x(),
                                                              src->get_size_y());
        gl3d::gl3d_framebuffer fb(GL3D_FRAME_HAS_DEPTH_BUF | GL3D_FRAME_HAS_STENCIL_BUF,
                                  src->get_size_x(),
                                  src->get_size_y());
        fb.attach_color_text(dst->get_text_obj());
        gl3d::shader_param * current_shader_param = ::gl3d::shader_manager::sharedInstance()->get_param("hdr_test");
        current_shader_param->user_data.insert(string("scene"), one_scene);
        one_scene->get_property()->current_draw_authority = GL3D_SCENE_DRAW_SPECIAL;
        one_scene->get_property()->global_shader = "hdr_test";
        one_scene->add_obj(2333, sobj);
        one_scene->prepare_canvas(true);
        GL3D_GL()->glDisable(GL_CULL_FACE);
        one_scene->draw(true);
        current_shader_param->user_data.erase(current_shader_param->user_data.find(string("scene")));
        one_scene->delete_obj(2333);
        delete sobj;
        return dst;
    } ;
    void  deel_with_src(gl3d_general_texture * src) {};
};
GL3D_ADD_POST_PROCESSER(hdr_test);
GL3D_LOAD_SHADER(hdr_test, hdr_test.vdata, hdr_test.fdata);

GL3D_SHADER_PARAM(hdr_test) {
    GLuint pro = GL3D_GET_SHADER("hdr_test")->getProgramID();
    gl3d::abstract_object * obj = GL3D_GET_OBJ();
    GL3D_GL()->glUniform1f(GL3D_GL()->glGetUniformLocation(pro, "mtlSpecularExponent"), 0.3);
    GL3D_GL()->glUniform1f(GL3D_GL()->glGetUniformLocation(pro, "shininess"), 0.5);
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_texture_ambient"), 0);
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_texture_diffuse"), 1);
    GL3D_GL()->glUniform1i(GL3D_GL()->glGetUniformLocation(pro, "gl3d_texture_specular"), 2);
    glm::vec3 light = glm::vec3(0.0, 1.0, -1.0);
    GL3D_SET_VEC3(light_vector, light, pro);
    glm::vec2 canvas_size(2.0 * gl3d_global_param::shared_instance()->canvas_width, 2.0 * gl3d_global_param::shared_instance()->canvas_height);
    GL3D_GL()->glUniform2fv(GL3D_GL()->glGetUniformLocation(pro, "canvas_size"), 1, glm::value_ptr(canvas_size));
    return true;
}

#endif
