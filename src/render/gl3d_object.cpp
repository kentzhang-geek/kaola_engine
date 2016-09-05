//
//  gl3d_object.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/3.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_material.hpp"
#include <QVector>

using namespace gl3d;
using namespace Assimp;
using namespace std;

/**
 *  @author Kent, 16-02-25 11:02:46
 *
 *  Object相关的接口
 */
void object::init() {
    this->set_obj_type(this->type_default);
    this->this_property.position = glm::vec3(0.0);
    this->this_property.rotate_mat = glm::mat4(1.0);
    this->this_property.bounding_value_max = glm::vec3(0.0f);
    this->this_property.bounding_value_min = glm::vec3(0.0f);
    this->param_x = 1.0;
    this->meshes.clear();
    this->data_buffered = false;
    this->pre_scaled = false;
    this->use_shader = std::string("multiple_text_vector_shadow"); // use multiple_text_vector_shadow shader as default
    this->mtls.clear();
    this->set_control_authority(GL3D_OBJ_ENABLE_ALL); //使能所有权限
    // 注意默认绘制权限为普通+阴影+倒影。天空盒与地面是特殊绘制
    this->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    this->obj_file_name.clear();
    this->res_id.clear();
}

object::object() {
    this->init();
}

object::~object() {
    if (0 != meshes.size()) {
        auto iter = meshes.begin();
        while (iter != meshes.end()) {
            delete *iter;
            iter++;
        }
        meshes.clear();
    }
    
    if (0 != mtls.size()) {
        auto iter2 = mtls.begin();
        while (iter2 != mtls.end()) {
            delete iter2.value();
            iter2++;
        }
        mtls.clear();
    }
}

object::object(char * path_to_obj_file) {
    if (!this->init(path_to_obj_file))
        log_c("faild to init from file %s", path_to_obj_file);
    this->obj_file_name = QString(path_to_obj_file);
    return;
}

object::object(obj_points * pts, int number_of_points,
               unsigned short * idx_in, int num_of_indecis)  {
    if (!this->init(pts, number_of_points, idx_in, num_of_indecis)) {
        log_c("fail to init from pts");
    }
    return;
}

object::object(obj_points * pts, int number_of_points,
               unsigned short * indecis, int num_of_indecis,
               obj_texture * textures, int number_of_textures) {
    
}

// try assimp
bool object::init(char * filename) {
    this->init();
    // Create a logger instance
    Assimp::Importer importer;
    string pFile = string(filename);
    
    // 设置最多每个mesh最多65000个点, 法线平滑角度80度
    importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65000);
    importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 20000);
//    importer.SetPropertyInteger(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 30);
    // 读取文件
    const aiScene* scene = importer.ReadFile
    (pFile,
     aiProcess_OptimizeMeshes |
//     aiProcess_SortByPType |
//     aiProcess_OptimizeGraph |   // KENT WARN : 这个选项开了之后会容易出现破面！
     aiProcess_Triangulate |
//     aiProcess_GenNormals |
     aiProcess_GenSmoothNormals |
//     aiProcess_JoinIdenticalVertices | // 合并相同的顶点，降低内存压力约30M
     aiProcess_SplitLargeMeshes | // 把过大的mesh分割成小mesh
     aiProcess_FlipUVs
     );
    // If the import failed, report it
    if (!scene)
    {
        cout << "Import Error : " << importer.GetErrorString() << endl;
        return false;
    }
    
    // Now we can access the file's contents.
    unsigned int num_mesh = scene->mNumMeshes;
    unsigned int num_mtl = scene->mNumMaterials;
    unsigned int i;

    if (0 >= num_mesh) {
        // 没有图形，直接挂掉
        log_c("there is no mesh in model");
        return false;
    }
    
    this->number_of_meshes = scene->mNumMeshes;
    
    // process meshes
    for (i = 0; i < scene->mNumMeshes; i++) {
        this->meshes.push_back(new gl3d::mesh((void *)scene->mMeshes[i]));
    }
    
    // process boundings
    this->this_property.bounding_value_max = this->meshes.at(0)->bounding_value_max;
    this->this_property.bounding_value_min = this->meshes.at(0)->bounding_value_min;
    for (auto it = this->meshes.begin(); it != this->meshes.end(); it++) {
        this->this_property.bounding_value_max.x = (this->this_property.bounding_value_max.x >
                                                    (*it)->bounding_value_max.x)
                                                   ? this->this_property.bounding_value_max.x
                                                   : (*it)->bounding_value_max.x;
        this->this_property.bounding_value_max.y = (this->this_property.bounding_value_max.y >
                                                    (*it)->bounding_value_max.y)
                                                   ? this->this_property.bounding_value_max.y
                                                   : (*it)->bounding_value_max.y;
        this->this_property.bounding_value_max.z = (this->this_property.bounding_value_max.z >
                                                    (*it)->bounding_value_max.z)
                                                   ? this->this_property.bounding_value_max.z
                                                   : (*it)->bounding_value_max.z;
        this->this_property.bounding_value_min.x = (this->this_property.bounding_value_min.x <
                                                    (*it)->bounding_value_min.x)
                                                   ? this->this_property.bounding_value_min.x
                                                   : (*it)->bounding_value_min.x;
        this->this_property.bounding_value_min.y = (this->this_property.bounding_value_min.y <
                                                    (*it)->bounding_value_min.y)
                                                   ? this->this_property.bounding_value_min.y
                                                   : (*it)->bounding_value_min.y;
        this->this_property.bounding_value_min.z = (this->this_property.bounding_value_min.z <
                                                    (*it)->bounding_value_min.z)
                                                   ? this->this_property.bounding_value_min.z
                                                   : (*it)->bounding_value_min.z;
    }
    
    int tex_cnt = scene->mNumMaterials;
    aiMaterial *mtl;
    for (i = 0; i < tex_cnt; i++) {
        mtl = scene->mMaterials[i];
        this->mtls.insert(i, new gl3d_material(mtl));
    }

    GL3D_GL()->glBindVertexArray(0);

    return true;
}

bool object::init(obj_points * pts, int number_of_points,
                  unsigned short * idx_in, int num_of_indecis) {
    this->init();
    
    // only on mesh for test
    this->meshes.push_back(new mesh(pts, number_of_points, idx_in, num_of_indecis));

    return true;
}

bool object::init(obj_points * pts, int number_of_points,
                  unsigned short * indecis, int num_of_indecis,
                  obj_texture * txtr, int number_of_textures) {
    this->init();
    
    // only one mesh for test
    this->meshes.push_back(new mesh(pts, number_of_points, indecis, num_of_indecis, txtr, number_of_textures));

    return true;
}

bool object::set_property(obj_property * property) {
    memcpy(&this->this_property, property, sizeof(obj_property));
    return true;
}

obj_property * object::get_property() {
    return &this->this_property;
}

void object::buffer_data() {
    // KENT TODO : 已缓存过顶点就不再缓存顶点，目前暂不考虑改变顶点数据的情况
    if (data_buffered) {
        return;
    }
    
    // pre process points data
    this->pre_scale();

    // bind vao
    GLuint tmpvao;
    GL3D_GL()->glGenVertexArrays(1, &tmpvao);
    this->set_vao(tmpvao);
    GL3D_GL()->glBindVertexArray(this->get_vao());

    gl3d::mesh * p_mesh;
    auto iter = this->meshes.begin();
    while (iter != this->meshes.end()) {
        p_mesh = *iter;
        p_mesh->buffer_data();
        iter++;
    }
    
    GL3D_GL()->glBindVertexArray(0);
    
    data_buffered = true;
}

bool object::rotate(glm::vec3 axis, GLfloat angle) {
    if (this->get_control_authority() & GL3D_OBJ_ENABLE_ROTATE) {
        this->this_property.rotate_mat = glm::rotate(this->this_property.rotate_mat, glm::radians(angle), axis);
        return true;
    }
    return false;
}

bool object::use_mtl(string file_name, int mtl_id) {
    if (this->get_control_authority() & GL3D_OBJ_ENABLE_CHANGEMTL) {
        gl3d_material * mtl = new gl3d_material(file_name);
        this->mtls.erase(this->mtls.find(mtl_id));
        this->mtls.insert(mtl_id, mtl);
        auto iter = this->meshes.begin();
        for (; iter != this->meshes.end(); iter++) {
            (*iter)->material_index = mtl_id;
        }
        return true;
    }
    return false;
}

void object::set_repeat(bool repeat) {
    auto iter = this->meshes.begin();
    for (; iter != this->meshes.end(); iter++) {
        (*iter)->texture_repeat = repeat;
    }
}

void object::merge_meshes() {
    QVector<mesh *> will_merge_mesh;
    QVector<mesh *> _new_meshes;
    int mtl_idx;
    
    will_merge_mesh.clear();
    _new_meshes.clear();
    
    int _vert_num = 0;
    int _idx_num = 0;
    
    auto iter = this->meshes.begin();
    while (this->meshes.size()> 0) {
        // 初始化当次遍历
        _vert_num = 0;
        _idx_num = 0;

        // 遍历每个mesh去提取
        mtl_idx = this->meshes[0]->material_index;
        for (iter = this->meshes.begin(); iter != this->meshes.end();) {
            // 提取材质相同的mesh
            if ((*iter)->material_index == mtl_idx) {
                // 检测是否超出每个mesh的最大顶点数量
                if((_vert_num > 60000) || (_idx_num > 60000)) {
                    break;
                }
                if (((_vert_num + (*iter)->num_pts) >= 65535)
                    || ((_idx_num + (*iter)->num_idx) >= 65535)) {
                    break;
                }
                // 添加顶点到合并组
                will_merge_mesh.push_back((*iter));
                // 更新合并组顶点数量和索引数量
                _vert_num += (*iter)->num_pts;
                _idx_num += (*iter)->num_idx;

                // 删除原顶点组
                this->meshes.erase(iter);
            }
            else {
                ++iter;
            }
        }
        
        // 新建合并后的mesh并加入
        _new_meshes.push_back(new mesh(will_merge_mesh));
        
        // 释放之前的mesh
        for (int i = 0; i < will_merge_mesh.size(); i++) {
            delete will_merge_mesh[i];
        }
        will_merge_mesh.clear();
    }
    
    this->meshes = _new_meshes;
    this->number_of_meshes = _new_meshes.size();
    return;
}

// 重新计算法向量
void object::recalculate_normals(float cos_angle) {
    auto iter = this->meshes.begin();
    for (; iter != this->meshes.end(); iter++) {
        (*iter)->recalculate_normals(cos_angle);
    }
}

void object::convert_left_hand_to_right_hand() {
    auto iter = this->meshes.begin();
    for (; iter != this->meshes.end(); iter++) {
        (*iter)->convert_left_hand_to_right_hand();
    }
}

void object::pre_scale() {
    if (this->pre_scaled) {
        return;
    }
    scale * sc = scale::shared_instance();
    for (QVector<mesh *>::iterator it = this->meshes.begin();
         it != this->meshes.end(); it++) {
        // KENT TODO : this need operator support
        mesh * p_mesh = *it;
        for(int i = 0; i < p_mesh->get_num_pts(); i++) {
            p_mesh->get_points_data()[i].vertex_x *=
                    sc->get_length_unit_to_scale_factor()->value(this->this_property.scale_unit);
            p_mesh->get_points_data()[i].vertex_y *=
                    sc->get_length_unit_to_scale_factor()->value(this->this_property.scale_unit);
            p_mesh->get_points_data()[i].vertex_z *=
                    sc->get_length_unit_to_scale_factor()->value(this->this_property.scale_unit);
        }
    }
    this->pre_scaled = true;
    this->this_property.scale_unit = sc->m;
}

bool object::is_data_changed() {
    return !(this->data_buffered);
}

bool object::is_visible() {
    return true;
}

glm::mat4 object::get_translation_mat() {
    return glm::translate(glm::mat4(1.0), this->this_property.position);
}

glm::mat4 object::get_rotation_mat() {
    return this->this_property.rotate_mat;
}

glm::mat4 object::get_scale_mat() {
    return glm::mat4(1.0);
}

void object::get_abstract_meshes(QVector<gl3d::mesh *> & ms) {
    ms.clear();
    ms = *(this->get_meshes());
    return;
}

void object::get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) {
    mt.clear();
    mt = *(this->get_mtls());
    return;
}

void object::set_translation_mat(const glm::mat4 & trans) {
    this->this_property.position = glm::vec3(trans * glm::vec4(0.0, 0.0, 0.0, 1.0));
}

void object::clear_abstract_meshes(QVector<gl3d::mesh *> & ms) {
    ms.clear();
    return;
}

void object::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) {
    mt.clear();
    return;
}

bool object::save_to_xml(pugi::xml_node &node) {
    return false;
}

object* object::load_from_xml(pugi::xml_node node) {
    return NULL;
}

#if 1
using namespace pugi;
int main() {
    xml_document doc;
    doc.load_file("mat.xml");
    xml_node tmp = doc.root().child("mat_test");
    glm::mat4 ppp(0.0f);
    gl3d::xml::load_xml_to_mat(tmp, ppp);
    return 0;
}
#endif
