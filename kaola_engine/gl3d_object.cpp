//
//  gl3d_object.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/3.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d.hpp"
#include "gl3d_out_headers.h"
#include "gl3d_material.hpp"

using namespace gl3d;
using namespace Assimp;
using namespace std;

/**
 *  @author Kent, 16-02-25 11:02:46
 *
 *  Object相关的接口
 */
void object::init() {
    memset(this, 0, sizeof(object));
    this->this_property.position = glm::vec3(0.0);
    this->this_property.rotate_mat = glm::mat4(1.0);
    this->this_property.bounding_value_max = glm::vec3(0.0f);
    this->this_property.bounding_value_min = glm::vec3(0.0f);
    this->param_x = 1.0;
    this->meshes.clear();
    this->data_buffered = false;
    this->use_shader = std::string("multiple_text_vector_shadow"); // use multiple_text_vector_shadow shader as default
    this->mtls.clear();
    this->this_property.authority = GL3D_OBJ_ENABLE_ALL; //使能所有权限
    // 注意默认绘制权限为普通+阴影+倒影。天空盒与地面是特殊绘制
    this->this_property.draw_authority = GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW;
    // gen vao
    glGenVertexArrays(1, &this->vao);
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
            delete (*iter2).second;
            iter2++;
        }
        mtls.clear();
    }
    
    // delete vao
    glDeleteVertexArrays(1, &this->vao);
}

object::object(char * path_to_obj_file) {
    if (!this->init(path_to_obj_file))
        log_c("faild to init from file %s", path_to_obj_file);
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
    importer.SetPropertyInteger(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80);
    // 读取文件
    const aiScene* scene = importer.ReadFile
    (pFile,
     aiProcess_OptimizeMeshes |
//     aiProcess_SortByPType |
//     aiProcess_OptimizeGraph |   // KENT WARN : 这个选项开了之后会容易出现破面！
     aiProcess_Triangulate |
//     aiProcess_GenNormals |
     aiProcess_GenSmoothNormals |
     aiProcess_JoinIdenticalVertices | // 合并相同的顶点，降低内存压力约30M
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
    glBindVertexArray(this->vao);
    for (i = 0; i < scene->mNumMeshes; i++) {
        this->meshes.push_back(new gl3d::mesh((void *)scene->mMeshes[i]));
    }
    
    // process boundings
    for (auto it = this->meshes.begin(); it != this->meshes.end(); it++) {
        this->this_property.bounding_value_max.x = (this->this_property.bounding_value_max.x > (*it)->bounding_value_max.x) ? this->this_property.bounding_value_max.x : (*it)->bounding_value_max.x;
        this->this_property.bounding_value_max.y = (this->this_property.bounding_value_max.y > (*it)->bounding_value_max.y) ? this->this_property.bounding_value_max.y : (*it)->bounding_value_max.y;
        this->this_property.bounding_value_max.z = (this->this_property.bounding_value_max.z > (*it)->bounding_value_max.z) ? this->this_property.bounding_value_max.z : (*it)->bounding_value_max.z;
        this->this_property.bounding_value_min.x = (this->this_property.bounding_value_min.x < (*it)->bounding_value_min.x) ? this->this_property.bounding_value_min.x : (*it)->bounding_value_min.x;
        this->this_property.bounding_value_min.y = (this->this_property.bounding_value_min.y < (*it)->bounding_value_min.y) ? this->this_property.bounding_value_min.y : (*it)->bounding_value_min.y;
        this->this_property.bounding_value_min.z = (this->this_property.bounding_value_min.z < (*it)->bounding_value_min.z) ? this->this_property.bounding_value_min.z : (*it)->bounding_value_min.z;
    }
    
    int tex_cnt = scene->mNumMaterials;
    aiMaterial *mtl;
    for (i = 0; i < tex_cnt; i++) {
        mtl = scene->mMaterials[i];
        this->mtls.insert(pair<unsigned int, gl3d_material *>(i, new gl3d_material(mtl)));
    }

    glBindVertexArray(0);

    return true;
}

bool object::init(obj_points * pts, int number_of_points,
                  unsigned short * idx_in, int num_of_indecis) {
    this->init();
    
    // only on mesh for test
    glBindVertexArray(this->vao);
    this->meshes.push_back(new mesh(pts, number_of_points, idx_in, num_of_indecis));
    glBindVertexArray(0);
    
    return true;
}

bool object::init(obj_points * pts, int number_of_points,
                  unsigned short * indecis, int num_of_indecis,
                  obj_texture * txtr, int number_of_textures) {
    this->init();
    
    // only one mesh for test
    glBindVertexArray(this->vao);
    this->meshes.push_back(new mesh(pts, number_of_points, indecis, num_of_indecis, txtr, number_of_textures));
    glBindVertexArray(0);
    
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
    
    // bind vao
    glBindVertexArray(this->vao);

    gl3d::mesh * p_mesh;
    auto iter = this->meshes.begin();
    while (iter != this->meshes.end()) {
        p_mesh = *iter;
        p_mesh->buffer_data();
        iter++;
    }
    
    glBindVertexArray(0);
    
    data_buffered = true;
}

bool object::rotate(glm::vec3 axis, GLfloat angle) {
    if (this->this_property.authority & GL3D_OBJ_ENABLE_ROTATE) {
        this->this_property.rotate_mat = glm::rotate(this->this_property.rotate_mat, glm::radians(angle), axis);
        return true;
    }
    return false;
}

bool object::use_mtl(string file_name, int mtl_id) {
    if (this->this_property.authority & GL3D_OBJ_ENABLE_CHANGEMTL) {
        gl3d_material * mtl = new gl3d_material(file_name);
        this->mtls.erase(mtl_id);
        this->mtls.insert(pair<int, gl3d::gl3d_material *>(mtl_id, mtl));
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
    vector<mesh *> will_merge_mesh;
    vector<mesh *> _new_meshes;
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
                iter++;
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
