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
    this->objects = new QMap<int,gl3d::object *>();
    this->shaders = ::shader_manager::sharedInstance();
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_ALL;
    using namespace Assimp;
    DefaultLogger::create("",Logger::VERBOSE);
    DefaultLogger::get()->attachStream(new myStream(), Logger::Debugging);
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

bool scene::add_obj(QPair<int,gl3d::object *> obj_key_pair) {
    this->objects->insert(obj_key_pair.first, obj_key_pair.second);
    obj_key_pair.second->this_property.id = (GLuint) obj_key_pair.first;
    return true;
}

bool scene::delete_obj(int key) {
    object * obj = this->get_obj(key);
    if (obj->this_property.authority & GL3D_OBJ_ENABLE_DEL) {
        this->objects->erase(this->objects->find(key));
        delete obj;
        return true;
    }
    
    return false;
}

gl3d::object * scene::get_obj(int key) {
    return this->objects->value(key);
}

bool scene::prepare_buffer() {
    auto itera = this->objects->begin();
    gl3d::object * obj = NULL;
    for (; itera != this->objects->end(); itera++) {
        obj = itera.value();
        // buffer data
        obj->buffer_data();
    }
    
    return true;
}

bool scene::prepare_canvas(bool use_global_shader) {
    // clear and set scene
    if (use_global_shader) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    else {
        glClearColor
        (this->this_property.background_color.x,
         this->this_property.background_color.y,
         this->this_property.background_color.z,
         1.0);
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepthf(1.0);
    glCullFace(GL_BACK);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);  // 通常绘制不使用模板缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DITHER);
    
    return true;
}

void scene::set_attribute(GLuint pro) {
    // enable attribute
    glEnableVertexAttribArray(glGetAttribLocation(pro, "vertex_pos"));
    glEnableVertexAttribArray(glGetAttribLocation(pro, "vertex_normal"));
    glEnableVertexAttribArray(glGetAttribLocation(pro, "vertex_color"));
    glEnableVertexAttribArray(glGetAttribLocation(pro, "vertex_tex_coord"));
    
    // set vertex
    GLint lct;
    lct = glGetAttribLocation(pro, "vertex_pos");
    glVertexAttribPointer
    (lct,
     3,
     GL_FLOAT,
     GL_FALSE,
     sizeof(gl3d::obj_points),
     (GLvoid *) (GLvoid *) &((gl3d::obj_points *)NULL)->vertex_x);
    
    // set normal
    lct = glGetAttribLocation(pro, "vertex_normal");
    glVertexAttribPointer
    (lct,
     3,
     GL_FLOAT,
     GL_FALSE,
     sizeof(gl3d::obj_points),
     (GLvoid *) &((gl3d::obj_points *)NULL)->normal_x);
    
    // set color
    lct = glGetAttribLocation(pro, "vertex_color");
    glVertexAttribPointer
    (lct,
     4,
     GL_FLOAT,
     GL_FALSE,
     sizeof(gl3d::obj_points),
     (GLvoid *) &((gl3d::obj_points *)NULL)->color_r);
    
    // set texture coordinate
    lct = glGetAttribLocation(pro, "vertex_tex_coord");
    glVertexAttribPointer
    (glGetAttribLocation(pro, "vertex_tex_coord"),
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
    object * current_obj = NULL;
    shader_param * param;
    
    // 遍历object去渲染物体
    auto iter_objs = this->objects->begin();     // 遍历所有object
    while (iter_objs != this->objects->end()) {
        current_obj = iter_objs.value();
        int cuid_tmp = iter_objs.key();
        //        cout << "current obj id " << (*iter_objs).first << endl;
        if (use_global_shader) {
            use_shader = GL3D_GET_SHADER(this->this_property.global_shader.c_str());
        }
        else {
            use_shader = GL3D_GET_SHADER(current_obj->use_shader);
        }
        if (NULL != use_shader) {
            glUseProgram(use_shader->getProgramID());
            if (use_global_shader) {
                param = GL3D_GET_PARAM(this->this_property.global_shader.c_str());
            }
            else {
                param = GL3D_GET_PARAM(current_obj->use_shader);
            }
            if (current_obj->get_property()->draw_authority & this->this_property.current_draw_authority) {
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
            log_c("object id %d use shader %s not found\n", iter_objs.key(), current_obj->use_shader.c_str());
            // TODO : 这里删除的话会报错
            iter_objs = this->objects->erase(iter_objs);  // 不再绘制当前未找到shader的物件
        }
    }
    glBindVertexArray(0);
    
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

void scene::draw_object(gl3d::object *obj, GLuint pro) {
    // set vao
    glBindVertexArray(obj->vao);
    this->set_attribute(pro);
    
    // TODO : set matrix
    // set MVP
    ::glm::mat4 pvm =
    this->watcher->projection_matrix *
    this->watcher->viewing_matrix;
    
    // set model matrix
    ::glm::mat4 trans;
    // set norMtx
    ::glm::mat4 norMtx;
    trans = ::glm::translate(trans, obj->get_property()->position);
    trans = trans * obj->get_property()->rotate_mat;
    GLfloat s_range = gl3d::scale::shared_instance()->get_scale_factor(
                obj->get_property()->scale_unit,
                gl3d::gl3d_global_param::shared_instance()->canvas_width);
    // KENT TODO : 这里似乎加上View MTX之后就会变得像手电筒一样
    //    norMtx = this->watcher->viewing_matrix * trans;
    norMtx = trans;
    trans = ::glm::scale(trans, glm::vec3(s_range));
    pvm *= trans;    // final MVP
    glm::mat4 unpvm = glm::inverse(pvm);
        
    glUniformMatrix4fv
    (glGetUniformLocation
     (pro, "unpvm"),
     1, GL_FALSE, glm::value_ptr(unpvm));
    glUniformMatrix4fv
    (glGetUniformLocation
     (pro, "pvmMatrix"),
     1, GL_FALSE, glm::value_ptr(pvm));
    glUniformMatrix4fv
    (glGetUniformLocation
     (pro, "translationMatrix"),
     1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv
    (glGetUniformLocation
     (pro, "viewingMatrix"),
     1, GL_FALSE, glm::value_ptr(this->watcher->viewing_matrix));
    glUniform3fv(glGetUniformLocation(pro, "eye_pos"), 1, glm::value_ptr(this->watcher->current_position));
    glUniform3fv(glGetUniformLocation(pro, "eye_look_at"), 1, glm::value_ptr(this->watcher->look_direction));
    glUniformMatrix3fv
    (glGetUniformLocation
     (pro, "normalMtx"),
     1, GL_FALSE, glm::value_ptr(glm::mat3(norMtx)));
    glUniform1f(glGetUniformLocation
                (pro, "param_x"), obj->param_x);
    
    auto iter = obj->meshes.begin();
    gl3d::mesh *p_mesh;
    while (iter != obj->meshes.end()) {
        p_mesh = *iter;
        // set buffers
        glBindBuffer(GL_ARRAY_BUFFER, p_mesh->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_mesh->idx);
        
        // bouding box test
        if ((check_bouding(p_mesh->bounding_value_max, p_mesh->bounding_value_min, pvm) == true) || !(obj->this_property.authority & GL3D_OBJ_ENABLE_CULLING)) {
            // 多重纹理的绑定与绘制
            try {
                obj->mtls.value(p_mesh->material_index)->use_this(pro);
                gl3d_texture::set_parami(p_mesh->texture_repeat);
            } catch (std::out_of_range & not_used_smth) {
                //log_c("material_index is %d and out of range", p_mesh->material_index);
            }
            this->set_attribute(pro);
            glDrawElements(GL_TRIANGLES,
                           p_mesh->num_idx,
                           GL_UNSIGNED_SHORT,
                           (GLvoid *)NULL);
        }
        iter++;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

int scene::get_object_id_by_coordination(int x, int y) {
    GLuint colorRenderbuffer;
    GLuint framebuffer;
    GLuint depthRenderBuffer;
    GLubyte pixelColor[4];
    
    // fb
    gl3d_framebuffer * frame = new gl3d_framebuffer(GL3D_FRAME_HAS_ALL, width, height);
    frame->use_this_frame();
        
    // KENT WARN : 有时间架构下渲染绘制流程，加入一个用texture绘制obj的id的招数
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_ALL;
    this->this_property.global_shader = string("picking_mask");
    this->prepare_canvas(true);
    this->draw(true);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);

    delete frame;
    
    int obj_id = -1;
    if (pixelColor[3] == 0xff) {
        obj_id = pixelColor[0] + (pixelColor[1]<<8) + (pixelColor[2]<<16);
    }
    //    log_c("Get Obj Id is %d", obj_id);
    
    // 检测是否可拾取
    if (obj_id > 0) {
        if (!(this->objects->value(obj_id)->get_property()->authority & GL3D_OBJ_ENABLE_PICKING)) {
            obj_id = -1;
        }
    }
    
    return obj_id;
}

bool scene::move_object(object * obj, glm::vec3 des_pos) {
    if (obj->get_property()->authority & GL3D_OBJ_ENABLE_MOVE) {
        obj->get_property()->position = des_pos;
        return true;
    }
    return false;
}

void scene::coord_ground(glm::vec3 coord_in, glm::vec2 & coord_out, GLfloat hight) {
    coord_in.z = 1.0;
    glm::vec3 txxx = glm::unProject(coord_in,
                                    glm::mat4(1.0),
                                    this->watcher->projection_matrix * this->watcher->viewing_matrix,
                                    glm::vec4(0.0, 0.0, this->width,
                                              this->height));
    coord_in.z = 0.1;
    txxx = txxx - glm::unProject(coord_in,
                                 glm::mat4(1.0),
                                 this->watcher->projection_matrix * this->watcher->viewing_matrix,
                                 glm::vec4(0.0, 0.0, this->width, this->height));
    
    glm::vec3 near_pt = *this->watcher->get_position();
    txxx = glm::normalize(txxx);
    glm::vec3 reallazer = txxx;  // 真实射线向量计算OK
    
    GLfloat param = (hight - near_pt.z) / reallazer.z;
    glm::vec3 tgt = near_pt + param * reallazer;
    //    log_c("we have tgt \n %f \n %f \n %f ", tgt.x, tgt.y, tgt.z);
    coord_out = glm::vec2(tgt);
    
    return;
}

void scene::coord_ground(glm::vec3 coord_in, glm::vec2 & coord_out) {
    this->coord_ground(coord_in, coord_out, 0.0);
    GLfloat depth = 0.0;
    glReadPixels(coord_in.x, coord_in.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    //    log_c("Get Depth %f", depth);
}

GLfloat scene::get_obj_hight(object * obj, glm::vec3 coord_in) {
    GLfloat hight_ret = 0.0;
    glm::vec2 grd_coord;
    glm::vec3 cam_coord;
    glm::vec3 obj_coord;
    
    // 获取一些坐标
    this->coord_ground(coord_in, grd_coord);
    cam_coord = *this->watcher->get_position();
    obj_coord = obj->get_property()->position;
    
    glm::vec3 c_to_o = obj_coord - cam_coord;
    glm::vec3 c_to_g = glm::vec3(grd_coord, 0.0) - cam_coord;
    c_to_o.z = 0.0;
    c_to_g.z = 0.0;
    
    GLfloat prop = glm::dot(glm::normalize(c_to_g), c_to_o);
    prop = (glm::length(c_to_g) - prop)/glm::length(c_to_g);
    
    log_c("get param %f and hight %f", prop, cam_coord.z * prop);
    
    return cam_coord.z * prop;
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

//    this->shadow_text->bind(GL_TEXTURE0);
//    unsigned char * test_data = (unsigned char *)malloc(4 * 2048 * 2048);
//    memset(test_data, 0, 4 * 2048 * 2048);
//    gl3d_win_gl_functions->glGetTexImage
//    (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, test_data);
//    QImage shadow_out(test_data, 2048, 2048, QImage::Format_RGBA8888);
//    if (!shadow_out.save("D:\\User\\Desktop\\KLM\\testb.png")) {
//        throw std::runtime_error("save failed");
//    }
    
    // create a frame for draw shadow
    gl3d_framebuffer * frame = new gl3d_framebuffer(0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    
    // 绘制阴影贴图
    frame->attach_depth_text(this->shadow_text->get_gl_obj());
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_SHADOW;
    this->this_property.global_shader = string("shadow_mask");
    frame->use_this_frame();
    this->prepare_canvas(true);
    // 绘制阴影贴图的时候所有东西都不透明
    glDisable(GL_BLEND);
//    gl3d_win_gl_functions->glPolygonOffset(1.0, 0.0);
//    glEnable(GL_POLYGON_OFFSET_FILL);
    this->draw(true);
//    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_BLEND);

//    this->shadow_text->bind(GL_TEXTURE0);
//    memset(test_data, 0, 4 * 2048 * 2048);
//    gl3d_win_gl_functions->glGetTexImage
//    (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, test_data);
//    QImage shadow_out_after(test_data, 2048, 2048, QImage::Format_RGBA8888);
//    if (!shadow_out_after.save("D:\\User\\Desktop\\KLM\\testa.png")) {
//        throw std::runtime_error("save failed");
//    }
//    free(test_data);

    frame->unbind_this_frame();
    delete frame;
    
    return;
}

void scene::draw_stencil() {
    /* Don't update color or depth. */
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    /* Draw 1 into the stencil buffer. */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
    
    /* Now drawing the floor just tags the floor pixels
     as stencil value 1. */
    this->this_property.current_draw_authority = GL3D_SCENE_DRAW_GROUND;
    this->this_property.global_shader = string("default");
    this->draw(true);
    
    /* Re-enable update of color and depth. */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}
