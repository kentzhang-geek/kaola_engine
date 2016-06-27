//
//  gl3d_mesh.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/14.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d.hpp"
#include "gl3d_out_headers.h"

using namespace gl3d;

void gl3d::mesh::init() {
    memset(this, 0, sizeof(gl3d::mesh));
    // gen vbo
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->idx);
    this->bounding_value_max = glm::vec3(0.0f);
    this->bounding_value_min = glm::vec3(0.0f);
    this->texture_repeat = false;
}

gl3d::mesh::~mesh() {
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->idx);
    
    if (NULL != this->points_data) {
        free(this->points_data);
    }
    if (NULL != this->indecis) {
        free(this->indecis);
    }
}

gl3d::mesh::mesh(void * in_p) {
    aiMesh * as_mesh = (aiMesh *)in_p;
    this->init();
    
    if (as_mesh->mNumVertices >= 65535) {
        throw std::invalid_argument("init meshes");
        return;
    }
    
    if (as_mesh->GetNumColorChannels() > 0) {
        throw std::runtime_error("Doesn't support color in vertexes for now");
        return;
    }
    
    unsigned int i;
    // vertexes
    this->points_data = (obj_points *)malloc(sizeof(obj_points) * as_mesh->mNumVertices);
    this->num_pts = as_mesh->mNumVertices;
    bool has_normal = as_mesh->HasNormals();
    bool has_text = as_mesh->HasTextureCoords(0);
    for (i = 0 ; i < as_mesh->mNumVertices; i++) {
        this->points_data[i].vertex_x = as_mesh->mVertices[i].x;
        this->points_data[i].vertex_y = as_mesh->mVertices[i].y;
        this->points_data[i].vertex_z = as_mesh->mVertices[i].z;
        if (has_normal) {
            this->points_data[i].normal_x = as_mesh->mNormals[i].x;
            this->points_data[i].normal_y = as_mesh->mNormals[i].y;
            this->points_data[i].normal_z = as_mesh->mNormals[i].z;
        }
        // for bounding value
        this->bounding_value_max.x = (this->points_data[i].vertex_x > this->bounding_value_max.x) ? this->points_data[i].vertex_x : this->bounding_value_max.x;
        this->bounding_value_max.y = (this->points_data[i].vertex_y > this->bounding_value_max.y) ? this->points_data[i].vertex_y : this->bounding_value_max.y;
        this->bounding_value_max.z = (this->points_data[i].vertex_z > this->bounding_value_max.z) ? this->points_data[i].vertex_z : this->bounding_value_max.z;
        this->bounding_value_min.x = (this->points_data[i].vertex_x < this->bounding_value_min.x) ? this->points_data[i].vertex_x : this->bounding_value_min.x;
        this->bounding_value_min.y = (this->points_data[i].vertex_y < this->bounding_value_min.y) ? this->points_data[i].vertex_y : this->bounding_value_min.y;
        this->bounding_value_min.z = (this->points_data[i].vertex_z < this->bounding_value_min.z) ? this->points_data[i].vertex_z : this->bounding_value_min.z;
        if (has_text) {
            // KENT TODO : 暂时只支持每个顶点一个纹理坐标
            this->points_data[i].texture_x = as_mesh->mTextureCoords[0][i].x;
            this->points_data[i].texture_y = as_mesh->mTextureCoords[0][i].y;
            // 检测是否是平铺贴图
            if ((this->points_data[i].texture_x > 1.0) || (this->points_data[i].texture_x < 0.0)) {
                this->texture_repeat = true;
            }
            if ((this->points_data[i].texture_y > 1.0) || (this->points_data[i].texture_y < 0.0)) {
                this->texture_repeat = true;
            }
        }
    }
    
    // construct indecis
    this->indecis = (GLushort *)malloc(sizeof(GLushort) * 3 * as_mesh->mNumFaces);
    memset(this->indecis, 0, sizeof(GLushort) * 3 * as_mesh->mNumFaces);
    this->num_idx = as_mesh->mNumFaces * 3;
    for (i = 0; i < as_mesh->mNumFaces; i++) {
        this->indecis[3 * i + 0] = as_mesh->mFaces[i].mIndices[0];
        this->indecis[3 * i + 1] = as_mesh->mFaces[i].mIndices[1];
        this->indecis[3 * i + 2] = as_mesh->mFaces[i].mIndices[2];
        if (as_mesh->mFaces[i].mNumIndices != 3) {
            // 现在只支持三角形面，不支持其他任何
            log_c("Only support 3 indices per face ,and now is not 3");
            break;
        }
    }
    
    // texture index
    this->material_index = as_mesh->mMaterialIndex;
    
    return;
}

// 测试用的加mesh构造函数
gl3d::mesh::mesh(obj_points * pts, int number_of_points,
                 unsigned short * idx_in, int num_of_indecis) {
    this->init();
}

// 测试用的加mesh构造函数
gl3d::mesh::mesh(obj_points * pts, int number_of_points,
     unsigned short * indecis, int num_of_indecis,
                 obj_texture * txtr, int number_of_textures) {
    this->init();
}

void gl3d::mesh::buffer_data() {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(obj_points) * this->num_pts, this->points_data, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * this->num_idx, this->indecis, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    free(this->points_data);
    this->points_data = NULL;
    free(this->indecis);
    this->indecis = NULL;
}

// KENT TODO : 测试mesh的合并算法
gl3d::mesh::mesh(QVector<mesh *> &meshes) {
    this->init();

    int _num_of_vert = 0;
    int _num_of_index = 0;
    int _vert_offset_for_index = 0;
    int _index_offset = 0;
    
    if (meshes.size() == 0) {
        log_c("error input mesh group!");
        throw std::out_of_range("mesh group");
        return;
    }
    
    auto iter = meshes.begin();
    // 统计点数量和索引数量
    for (; iter != meshes.end(); iter++) {
        _num_of_vert += (*iter)->num_pts;
        _num_of_index += (*iter)->num_idx;
    }
    if ((_num_of_vert >= 65535) || (_num_of_index >= 65535)) {
        log_c("meshes merger get over large points");
        return;
    }
    // 创造内存空间
    this->points_data = (gl3d::obj_points *)malloc(sizeof(gl3d::obj_points) * _num_of_vert);
    this->indecis = (GLushort *)malloc(sizeof(GLushort) * _num_of_index);
    //设置材质索引值
    this->material_index = meshes[0]->material_index;
    this->texture_repeat = meshes[0]->texture_repeat;
    this->num_pts = _num_of_vert;
    this->num_idx = _num_of_index;

    // 复制数据
    for (iter = meshes.begin(); iter != meshes.end(); iter++) {
        // 检测mesh的bouding box
        this->bounding_value_max.x = (this->bounding_value_max.x > (*iter)->bounding_value_max.x) ? this->bounding_value_max.x : (*iter)->bounding_value_max.x;
        this->bounding_value_max.y = (this->bounding_value_max.y > (*iter)->bounding_value_max.y) ? this->bounding_value_max.y : (*iter)->bounding_value_max.y;
        this->bounding_value_max.z = (this->bounding_value_max.z > (*iter)->bounding_value_max.x) ? this->bounding_value_max.z : (*iter)->bounding_value_max.z;
        this->bounding_value_min.x = (this->bounding_value_min.x < (*iter)->bounding_value_min.x) ? this->bounding_value_min.x : (*iter)->bounding_value_min.x;
        this->bounding_value_min.y = (this->bounding_value_min.y < (*iter)->bounding_value_min.y) ? this->bounding_value_min.y : (*iter)->bounding_value_min.y;
        this->bounding_value_min.z = (this->bounding_value_min.z < (*iter)->bounding_value_min.x) ? this->bounding_value_min.z : (*iter)->bounding_value_min.z;
        // 复制点数据
        memcpy
        (this->points_data + _vert_offset_for_index,
         (*iter)->points_data,
         (*iter)->num_pts * sizeof(gl3d::obj_points));
        
        // 复制索引数据
        memcpy
        (this->indecis + _index_offset,
         (*iter)->indecis,
         (*iter)->num_idx * sizeof(GLushort));
        
        // 索引数据需要重新偏移
        for (int i = 0; i < (*iter)->num_idx; i++) {
            this->indecis[_index_offset + i] += _vert_offset_for_index;
        }
        
        // 偏移量再次偏移
        _index_offset += (*iter)->num_idx;
        _vert_offset_for_index += (*iter)->num_pts;
    }
    
//    log_c("now merge output %d points and %d indecis", this->num_pts, this->num_idx);
    return;
}
