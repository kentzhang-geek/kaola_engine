//
//  gl3d.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/2/17.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "assimp/mesh.h"
#include "assimp/Logger.hpp"
#include "assimp/DefaultLogger.hpp"
#include "kaola_engine/gl3d_framebuffer.hpp"
#include "utils/gl3d_lock.h"

std::string gl3d_sandbox_path;

// assimp log stream
class myStream :
        public Assimp::LogStream
{ public:
    // Constructor
    myStream() {
        // empty
    }
    // Destructor
    ~myStream()
    {
        // empty
    }
    // Write womethink using your own functionality
    void write(const char* message)
    {
        ::printf("ASSIMP : %s\n", message); }
};


using namespace gl3d;

/**
 *  @author Kent, 16-02-25 11:02:57
 *
 *  scene相关的接口
 */

/**
 初始化scene
 */
void scene::init() {
    this->watcher = new gl3d::viewer(this->height, this->width);
    memset(&this->this_property, 0, sizeof(scene_property));
    memset(&this->lights, 0, sizeof(light_property) * 4);
    this->objects.clear();
    this->shaders = ::shader_manager::sharedInstance();
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_ALL;
    //    using namespace Assimp;
    //    DefaultLogger::create("",Logger::VERBOSE);
    //    DefaultLogger::get()->attachStream(new myStream(), Logger::Debugging);
}

scene::scene(GLfloat h, GLfloat w) {
    log_c("screen h %f w %f", h, w);
    this->height = h;
    this->width = w;
    this->init();
    return;
}

scene::scene(GLfloat h, GLfloat w, scene_property * property) {
    this->height = h;
    this->width = w;
    this->init();
    this->init(property);
    return;
}

scene::~scene() {
    delete this->watcher;
    Assimp::DefaultLogger::kill();
    return;
}

bool scene::init(scene_property * property) {
    memcpy(&this->this_property, property, sizeof(scene_property));
    
    return true;
}

bool scene::add_obj(QPair<int, gl3d::abstract_object *> obj_key_pair) {
    this->objects.insert(obj_key_pair.first, obj_key_pair.second);
    obj_key_pair.second->set_id((GLuint) obj_key_pair.first);
    obj_key_pair.second->buffer_data();
    return true;
}

bool scene::delete_obj(int key) {
    if (!this->objects.contains(key))
        return true;
    abstract_object * obj = this->get_obj(key);
    if (obj->get_control_authority() & GL3D_OBJ_ENABLE_DEL) {
        this->objects.erase(this->objects.find(key));
        return true;
    }
    
    return false;
}

gl3d::abstract_object * scene::get_obj(int key) {
    return this->objects.value(key);
}

bool scene::prepare_buffer() {
    auto itera = this->objects.begin();
    gl3d::abstract_object * obj = NULL;
    for (; itera != this->objects.end(); itera++) {
        obj = itera.value();
        // buffer data
        obj->buffer_data();
    }
    
    return true;
}

bool scene::prepare_canvas(bool use_global_shader) {
    // clear and set scene
    if (use_global_shader) {
        GL3D_GL()->glClearColor(0.0, 0.0, 0.0, 1.0);
    }
    else {
        GL3D_GL()->glClearColor
                (this->this_property.background_color.x,
                 this->this_property.background_color.y,
                 this->this_property.background_color.z,
                 1.0);
    }
    GL3D_GL()->glEnable(GL_CULL_FACE);
    GL3D_GL()->glCullFace(GL_BACK);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
    GL3D_GL()->glDepthMask(GL_TRUE);
    GL3D_GL()->glDepthFunc(GL_LEQUAL);
    GL3D_GL()->glClearDepthf(1.0);
    GL3D_GL()->glCullFace(GL_BACK);
    GL3D_GL()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GL3D_GL()->glEnable(GL_BLEND);
    GL3D_GL()->glDisable(GL_STENCIL_TEST);  // 通常绘制不使用模板缓冲区
    GL3D_GL()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GL3D_GL()->glEnable(GL_DITHER);
    
    return true;
}

void scene::set_attribute(GLuint pro) {
    // enable attribute
    GL3D_GL()->glEnableVertexAttribArray(GL3D_GL()->glGetAttribLocation(pro, "vertex_pos"));
    GL3D_GL()->glEnableVertexAttribArray(GL3D_GL()->glGetAttribLocation(pro, "vertex_normal"));
    GL3D_GL()->glEnableVertexAttribArray(GL3D_GL()->glGetAttribLocation(pro, "vertex_color"));
    GL3D_GL()->glEnableVertexAttribArray(GL3D_GL()->glGetAttribLocation(pro, "vertex_tex_coord"));
    
    // set vertex
    GLint lct;
    lct = GL3D_GL()->glGetAttribLocation(pro, "vertex_pos");
    GL3D_GL()->glVertexAttribPointer
            (lct,
             3,
             GL_FLOAT,
             GL_FALSE,
             sizeof(gl3d::obj_points),
             (GLvoid *) (GLvoid *) &((gl3d::obj_points *)NULL)->vertex_x);
    
    // set normal
    lct = GL3D_GL()->glGetAttribLocation(pro, "vertex_normal");
    GL3D_GL()->glVertexAttribPointer
            (lct,
             3,
             GL_FLOAT,
             GL_FALSE,
             sizeof(gl3d::obj_points),
             (GLvoid *) &((gl3d::obj_points *)NULL)->normal_x);
    
    // set color
    lct = GL3D_GL()->glGetAttribLocation(pro, "vertex_color");
    GL3D_GL()->glVertexAttribPointer
            (lct,
             4,
             GL_FLOAT,
             GL_FALSE,
             sizeof(gl3d::obj_points),
             (GLvoid *) &((gl3d::obj_points *)NULL)->color_r);
    
    // set texture coordinate
    lct = GL3D_GL()->glGetAttribLocation(pro, "vertex_tex_coord");
    GL3D_GL()->glVertexAttribPointer
            (GL3D_GL()->glGetAttribLocation(pro, "vertex_tex_coord"),
             2,
             GL_FLOAT,
             GL_FALSE,
             sizeof(gl3d::obj_points),
             (GLvoid *) &((gl3d::obj_points *)NULL)->texture_x);
}

bool scene::draw(bool use_global_shader) {
    GLuint pro = 0;
    if (this->shaders->shaders.size() == 0) {
        return false;
    }
    Program * use_shader = NULL;
    abstract_object * current_obj = NULL;
    shader_param * param;
    
    // 遍历object去渲染物体
    auto iter_objs = this->objects.begin();     // 遍历所有object
    while (iter_objs != this->objects.end()) {
        current_obj = iter_objs.value();
        int cuid_tmp = iter_objs.key();
        //        cout << "current obj id " << (*iter_objs).first << endl;
        // do not support specify shader now, global shader only
        use_shader = GL3D_GET_SHADER(this->this_property.global_shader.c_str());
        if (NULL != use_shader) {
            GL3D_GL()->glUseProgram(use_shader->getProgramID());

            // globla shader only, do not support specific shader now
            param = GL3D_GET_PARAM(this->this_property.global_shader.c_str());

            if (current_obj->get_render_authority() & this->this_property.current_draw_authority) {
                if (NULL != param) {
                    // set object to user data before set param
                    param->user_data.insert(string("object"), (void *)current_obj);
                    param->set_param();
                    // then delete param
                    param->user_data.erase(param->user_data.find(string("object")));
                }
                this->draw_object(current_obj, use_shader->getProgramID());
            }
            iter_objs++;
        }
        else {
            GL3D_UTILS_WARN("object id %d use shader %s not found\n", iter_objs.key(), this->this_property.global_shader.c_str());
            // TODO : 这里删除的话会报错
            iter_objs = this->objects.erase(iter_objs);  // 不再绘制当前未找到shader的物件
        }
    }
    GL3D_GL()->glBindVertexArray(0);
    
    return true;
}

static inline bool check_bouding(glm::vec3 xyzmax, glm::vec3 xyzmin, glm::mat4 pvm) {
    glm::vec3 bounding_box[8];
    for (int ii = 0; ii < 4; ii++) {
        bounding_box[ii] = xyzmax;
    }
    for (int ii = 4; ii < 8; ii++) {
        bounding_box[ii] = xyzmin;
    }
    bounding_box[1].x = xyzmin.x;
    bounding_box[2].y = xyzmin.y;
    bounding_box[3].z = xyzmin.z;
    bounding_box[5].x = xyzmax.x;
    bounding_box[6].y = xyzmax.y;
    bounding_box[7].z = xyzmax.z;
    bool traw_able = false;
    glm::vec4 pos[8];
    for (int ii = 0; ii < 8; ii++) {
        pos[ii] = pvm * glm::vec4(bounding_box[ii], 1.0f);
        pos[ii] = pos[ii]/pos[ii].w;
        //        if ((abs(pos[ii].x) <= 1.0f) &&
        //            (abs(pos[ii].y) <= 1.0f) &&
        //            (pos[ii].z >= 0.0f)) {
        //            traw_able = true;
        //        }
    }
    for (int j = 0; (j < 8) && (!traw_able); j++) {
        for (int k = 0; (k < j) && (!traw_able); k++) {
            if (j != k) {
                // a or b is in rect
                if ((abs(pos[j].x) <= 1.0f) &&
                        (abs(pos[j].y) <= 1.0f) &&
                        (pos[j].z >= 0)) {
                    traw_able = true;
                    break;
                }
                if ((abs(pos[k].x) <= 1.0f) &&
                        (abs(pos[k].y) <= 1.0f) &&
                        (pos[k].z >= 0)) {
                    traw_able = true;
                    break;
                }
                
                // between a and b has a pt in rect
                glm::vec4 dir = pos[k] - pos[j];
                if (dir.x != 0.0) {
                    float u = (-1.0 - pos[j].x)/dir.x;
                    float v = (1.0 - pos[j].x)/dir.x;
                    if (u <= 1.0) {
                        float w = pos[j].y + (u * dir).y;
                        if ((w < 1.0) && (w > -1.0)) {
                            traw_able = true;
                            break;
                        }
                    }
                    if (v <= 1.0) {
                        float w = pos[j].y + (v * dir).y;
                        if ((w < 1.0) && (w > -1.0)) {
                            traw_able = true;
                            break;
                        }
                    }
                }
                if (dir.y != 0.0) {
                    float u = (-1.0 - pos[j].y)/dir.y;
                    float v = (1.0 - pos[j].y)/dir.y;
                    if (u <= 1.0) {
                        float w = pos[j].x + (u * dir).x;
                        if ((w < 1.0) && (w > -1.0)) {
                            traw_able = true;
                            break;
                        }
                    }
                    if (v <= 1.0) {
                        float w = pos[j].x + (v * dir).x;
                        if ((w < 1.0) && (w > -1.0)) {
                            traw_able = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return traw_able;
}

void scene::draw_object(gl3d::abstract_object *obj, GLuint pro) {
    // set vao
    GL3D_GL()->glBindVertexArray(obj->get_vao());
    this->set_attribute(pro);
    
    // TODO : set matrix
    // set MVP
    ::glm::mat4 pvm =
            this->watcher->projection_matrix *
            this->watcher->viewing_matrix;
    
    // set model matrix
    ::glm::mat4 trans = obj->get_translation_mat() * obj->get_rotation_mat() * obj->get_scale_mat();
    // set norMtx
    ::glm::mat4 norMtx = obj->get_rotation_mat();
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    trans = ::glm::scale(glm::mat4(1.0), glm::vec3(s_range)) * trans;
    pvm *= trans;    // final MVP
    glm::mat4 unpvm = glm::inverse(pvm);

    GL3D_GL()->glUniformMatrix4fv
            (GL3D_GL()->glGetUniformLocation
             (pro, "unpvm"),
             1, GL_FALSE, glm::value_ptr(unpvm));
    GL3D_GL()->glUniformMatrix4fv
            (GL3D_GL()->glGetUniformLocation
             (pro, "pvmMatrix"),
             1, GL_FALSE, glm::value_ptr(pvm));
    GL3D_GL()->glUniformMatrix4fv
            (GL3D_GL()->glGetUniformLocation
             (pro, "translationMatrix"),
             1, GL_FALSE, glm::value_ptr(trans));
    GL3D_GL()->glUniformMatrix4fv
            (GL3D_GL()->glGetUniformLocation
             (pro, "viewingMatrix"),
             1, GL_FALSE, glm::value_ptr(this->watcher->viewing_matrix));
    GL3D_GL()->glUniform3fv(GL3D_GL()->glGetUniformLocation(pro, "eye_pos"), 1, glm::value_ptr(this->watcher->current_position));
    GL3D_GL()->glUniform3fv(GL3D_GL()->glGetUniformLocation(pro, "eye_look_at"), 1, glm::value_ptr(this->watcher->look_direction));
    GL3D_GL()->glUniformMatrix3fv
            (GL3D_GL()->glGetUniformLocation
             (pro, "normalMtx"),
             1, GL_FALSE, glm::value_ptr(glm::mat3(norMtx)));
    if (obj->get_pick_flag()) {  // obj 拾取要做的一些事
        GL3D_GL()->glUniform1f(GL3D_GL()->glGetUniformLocation
                               (pro, "param_x"), 0.5);
    }
    else {
        GL3D_GL()->glUniform1f(GL3D_GL()->glGetUniformLocation
                               (pro, "param_x"), 1.0);
    }

    QVector<mesh *> mss;
    mss.clear();
    obj->get_abstract_meshes(mss);
    QMap<unsigned int, gl3d_material *>  mts;
    auto iter = mss.begin();
    gl3d::mesh *p_mesh;
    while (iter != mss.end()) {
        p_mesh = *iter;
        if (!p_mesh->data_buffered) {
            p_mesh->buffer_data();
        }
        // set buffers
        GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, p_mesh->vbo);
        GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_mesh->idx);
        
        // 多重纹理的绑定与绘制
        try {
            mts.clear();
            obj->get_abstract_mtls(mts);
            mts.value(p_mesh->material_index)->use_this(pro);
            gl3d_texture::set_parami(p_mesh->texture_repeat);
        } catch (std::out_of_range & not_used_smth) {
            //log_c("material_index is %d and out of range", p_mesh->material_index);
        }
        this->set_attribute(pro);
        GL3D_GL()->glDrawElements(GL_TRIANGLES,
                                  p_mesh->num_idx,
                                  GL_UNSIGNED_SHORT,
                                  (GLvoid *)NULL);
        iter++;
    }
    
    GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL3D_GL()->glBindVertexArray(0);
}

bool scene::set_property(scene_property * property) {
    memcpy(&this->this_property, property, sizeof(scene_property));
    return true;
}

gl3d::scene::scene_property * scene::get_property() {
    return &this->this_property;
}

bool scene::set_light(int id_of_light, light_property * property) {
    memcpy(&this->lights[id_of_light], property, sizeof(light_property));
    return true;
}

gl3d::scene::light_property * scene::get_light(int id_of_light) {
    return &this->lights[id_of_light];
}
void scene::draw_object_picking_mask() {
    GLubyte pixelColor[4];

    this->picking_frame->use_this_frame();

    // KENT WARN : 有时间架构下渲染绘制流程，加入一个用texture绘制obj的id的招数
    this->this_property.current_draw_authority =
            GL3D_SCENE_DRAW_NORMAL |
            GL3D_SCENE_DRAW_GROUND |
            GL3D_SCENE_DRAW_WALL;
    this->this_property.global_shader = string("picking_mask");
    this->prepare_canvas(true);
    GL3D_GL()->glDisable(GL_CULL_FACE);
    this->draw(true);
    //    GL3D_GL()->glEnable(GL_CULL_FACE);

    this->picking_frame->unbind_this_frame();

    return;
}

int scene::get_object_id_by_coordination(int x, int y) {
    GLubyte pixelColor[4];

    // lock render
    gl3d_lock::shared_instance()->render_lock.lock();
    // pikcing mask
    this->picking_frame = new gl3d_framebuffer(GL3D_FRAME_HAS_ALL, width, height);

    this->draw_object_picking_mask();

    // use picking mask
    this->picking_frame->use_this_frame();

    // get pixel
    // 这里临时处理了下，似乎整个画面倒过来了？
    GL3D_GL()->glReadPixels(x, this->height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
    //    this->picking_frame->save_to_file
    //            ("D:\\User\\Desktop\\KLM\\qt_opengl_engine\\test.jpg");

    // unbind picking mask
    this->picking_frame->unbind_this_frame();
    delete this->picking_frame;
    this->picking_frame = NULL;

    // unlock render
    gl3d_lock::shared_instance()->render_lock.unlock();

    int obj_id = -1;
    if (pixelColor[3] == 0xff) {
        obj_id = pixelColor[0] + (pixelColor[1]<<8) + (pixelColor[2]<<16);
    }
    
    // 检测是否可拾取
    if (obj_id > 0) {
        if (this->objects.contains(obj_id)) {
            if (!(this->objects.value(obj_id)->get_control_authority() & GL3D_OBJ_ENABLE_PICKING)) {
                obj_id = -1;
            }
        }
        else {
            obj_id = -1;
        }
    }
    
    return obj_id;
}

bool scene::move_object(gl3d::abstract_object *obj, glm::vec3 des_pos) {
    if (obj->get_control_authority() & GL3D_OBJ_ENABLE_MOVE) {
        obj->set_translation_mat(
                    glm::translate(glm::mat4(1.0), des_pos)
                    );
        return true;
    }
    return false;
}

void scene::coord_ground(glm::vec2 coord_in, glm::vec2 & coord_out, GLfloat hight) {
    this->watcher->coord_ground(coord_in, coord_out, hight);
    return;
}

void scene::coord_ground(glm::vec2 coord_in, glm::vec2 & coord_out) {
    this->coord_ground(coord_in, coord_out, 0.0);
}

GLfloat scene::get_obj_hight(abstract_object * obj, glm::vec2 coord_in) {
    GLfloat hight_ret = 0.0;
    glm::vec2 grd_coord;
    glm::vec3 cam_coord;
    glm::vec3 obj_coord;
    
    // 获取一些坐标
    this->coord_ground(coord_in, grd_coord);
    cam_coord = this->watcher->get_current_position();
    obj_coord = glm::vec3(obj->get_translation_mat() * glm::vec4(0.0, 0.0, 0.0, 1.0));
    
    glm::vec3 c_to_o = obj_coord - cam_coord;
    glm::vec3 c_to_g = glm::vec3(grd_coord, 0.0) - cam_coord;
    c_to_o.y = 0.0;
    c_to_g.y = 0.0;
    
    GLfloat prop = glm::dot(glm::normalize(c_to_g), c_to_o);
    prop = (glm::length(c_to_g) - prop)/glm::length(c_to_g);
    
    log_c("get param %f and hight %f", prop, cam_coord.y * prop);
    
    return cam_coord.y * prop;
}

// KENT WARN
// 注意这里的一定要调用glviewport，否则会有大坑！并且注意高清阴影贴图对内存的消耗！
#define TEXTURE_WIDTH 2048
#define TEXTURE_HEIGHT 2048
void scene::gen_shadow_texture() {
    this->shadow_text = new gl3d_general_texture(gl3d_general_texture::GL3D_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    //    glTexStorage2DEXT(GL_TEXTURE_2D, 0, GL_RGBA8_, TEXTURE_WIDTH, TEXTURE_HEIGHT);
}

void scene::delete_shadow_texture() {
    delete this->shadow_text;
}

gl3d_general_texture *scene::get_shadow_texture() {
    return this->shadow_text;
}

void scene::draw_shadow_mask() {
    // clean shadow text data
    this->shadow_text->clean_data();

    // create a frame for draw shadow
    gl3d_framebuffer * frame = new gl3d_framebuffer(0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    
    // 绘制阴影贴图
    frame->attach_depth_text(this->shadow_text->get_text_obj());
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_SHADOW;
    this->this_property.global_shader = string("shadow_mask");
    frame->use_this_frame();
    this->prepare_canvas(true);
    // 绘制阴影贴图的时候所有东西都不透明
    GL3D_GL()->glDisable(GL_BLEND);
    //    gl3d_win_gl_functions->glPolygonOffset(1.0, 0.0);
    //    glEnable(GL_POLYGON_OFFSET_FILL);
    this->draw(true);
    //    glDisable(GL_POLYGON_OFFSET_FILL);
    GL3D_GL()->glEnable(GL_BLEND);

    frame->unbind_this_frame();
    delete frame;
    
    return;
}

void scene::draw_stencil() {
    /* Don't update color or depth. */
    GL3D_GL()->glDisable(GL_DEPTH_TEST);
    GL3D_GL()->glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    /* Draw 1 into the stencil buffer. */
    GL3D_GL()->glEnable(GL_STENCIL_TEST);
    GL3D_GL()->glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    GL3D_GL()->glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
    
    /* Now drawing the floor just tags the floor pixels
     as stencil value 1. */
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    this->this_property.global_shader = string("default");
    this->draw(true);
    
    /* Re-enable update of color and depth. */
    GL3D_GL()->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GL3D_GL()->glEnable(GL_DEPTH_TEST);
}
