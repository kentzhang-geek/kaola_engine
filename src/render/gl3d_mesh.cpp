//
//  gl3d_mesh.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/14.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "utils/gl3d_math.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/gl3d_out_headers.h"

using namespace gl3d;

void gl3d::mesh::init() {
    memset(this, 0, sizeof(gl3d::mesh));
    this->vbo = -1;
    this->idx = -1;
    this->bounding_value_max = glm::vec3(0.0f);
    this->bounding_value_min = glm::vec3(0.0f);
    this->texture_repeat = false;
    this->points_data = NULL;
    this->indecis = NULL;
    this->is_blink = false;
}

gl3d::mesh::~mesh() {
    if (this->data_buffered) {
        GL3D_GL()->glDeleteBuffers(1, &this->vbo);
        GL3D_GL()->glDeleteBuffers(1, &this->idx);
    }

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
    // init boundings
    this->bounding_value_max.x = as_mesh->mVertices[0].x;
    this->bounding_value_max.y = as_mesh->mVertices[0].y;
    this->bounding_value_max.z = as_mesh->mVertices[0].z;
    this->bounding_value_min.x = as_mesh->mVertices[0].x;
    this->bounding_value_min.y = as_mesh->mVertices[0].y;
    this->bounding_value_min.z = as_mesh->mVertices[0].z;
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
        // for bounding value , TODO : bounding box seems have BUG
        this->bounding_value_max.x = (this->points_data[i].vertex_x > this->bounding_value_max.x)
                                     ? this->points_data[i].vertex_x : this->bounding_value_max.x;
        this->bounding_value_max.y = (this->points_data[i].vertex_y > this->bounding_value_max.y)
                                     ? this->points_data[i].vertex_y : this->bounding_value_max.y;
        this->bounding_value_max.z = (this->points_data[i].vertex_z > this->bounding_value_max.z)
                                     ? this->points_data[i].vertex_z : this->bounding_value_max.z;
        this->bounding_value_min.x = (this->points_data[i].vertex_x < this->bounding_value_min.x)
                                     ? this->points_data[i].vertex_x : this->bounding_value_min.x;
        this->bounding_value_min.y = (this->points_data[i].vertex_y < this->bounding_value_min.y)
                                     ? this->points_data[i].vertex_y : this->bounding_value_min.y;
        this->bounding_value_min.z = (this->points_data[i].vertex_z < this->bounding_value_min.z)
                                     ? this->points_data[i].vertex_z : this->bounding_value_min.z;
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

// 特殊用途的mesh构造函数，仅仅一个材质
gl3d::mesh::mesh(obj_points * pts, int number_of_points,
                 unsigned short * idx_in, int num_of_indecis) {
    this->init();

    // set data
    this->points_data = (obj_points *) malloc(sizeof(obj_points) * number_of_points);
    memcpy(this->points_data, pts, sizeof(obj_points) * number_of_points);
    this->indecis = (GLushort *)malloc(sizeof(GLushort) * num_of_indecis);
    memcpy(this->indecis, idx_in, sizeof(GLushort) * num_of_indecis);

    this->num_pts = number_of_points;
    this->num_idx = num_of_indecis;

    // default only one material
    this->material_index = 0;
    this->texture_repeat = false;

    return;
}

// 测试用的加mesh构造函数
gl3d::mesh::mesh(obj_points * pts, int number_of_points,
     unsigned short * indecis, int num_of_indecis,
                 obj_texture * txtr, int number_of_textures) {
    this->init();
    throw QString::asprintf("should not call mesh constructor at %s : %d", __FILE__, __LINE__);
}

void gl3d::mesh::buffer_data() {
    if (this->data_buffered) {
        return;
    }
    // gen vbo
    GL3D_GL()->glGenBuffers(1, &this->vbo);
    GL3D_GL()->glGenBuffers(1, &this->idx);
    // buffer data
    GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idx);
    GL3D_GL()->glBufferData(GL_ARRAY_BUFFER, sizeof(obj_points) * this->num_pts, this->points_data, GL_STATIC_DRAW);
    GL3D_GL()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * this->num_idx, this->indecis, GL_STATIC_DRAW);
    GL3D_GL()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL3D_GL()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    free(this->points_data);
    this->points_data = NULL;
    free(this->indecis);
    this->indecis = NULL;

    this->data_buffered = true;
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

    // init boundings
    this->bounding_value_max = meshes.at(0)->bounding_value_max;
    this->bounding_value_min = meshes.at(0)->bounding_value_min;
    // 复制数据
    for (iter = meshes.begin(); iter != meshes.end(); iter++) {
        // 检测mesh的bouding box
        this->bounding_value_max.x = (this->bounding_value_max.x > (*iter)->bounding_value_max.x)
                                     ? this->bounding_value_max.x : (*iter)->bounding_value_max.x;
        this->bounding_value_max.y = (this->bounding_value_max.y > (*iter)->bounding_value_max.y)
                                     ? this->bounding_value_max.y : (*iter)->bounding_value_max.y;
        this->bounding_value_max.z = (this->bounding_value_max.z > (*iter)->bounding_value_max.x)
                                     ? this->bounding_value_max.z : (*iter)->bounding_value_max.z;
        this->bounding_value_min.x = (this->bounding_value_min.x < (*iter)->bounding_value_min.x)
                                     ? this->bounding_value_min.x : (*iter)->bounding_value_min.x;
        this->bounding_value_min.y = (this->bounding_value_min.y < (*iter)->bounding_value_min.y)
                                     ? this->bounding_value_min.y : (*iter)->bounding_value_min.y;
        this->bounding_value_min.z = (this->bounding_value_min.z < (*iter)->bounding_value_min.x)
                                     ? this->bounding_value_min.z : (*iter)->bounding_value_min.z;
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
    
    return;
}

// 重新计算法向量
void gl3d::mesh::recalculate_normals(float cos_angle) {
//    QVector<float> tmpvector;
    // only support triangles
    Q_ASSERT(this->num_idx%3 == 0);

    // 创建新点云集缓冲区
    obj_points * new_points = (obj_points *)malloc(sizeof(obj_points) * this->num_idx);
    // 创建新索引缓冲区
    GLushort * new_indecis = (GLushort *)malloc(sizeof(GLushort) * this->num_idx);

    for (int i = 0; i < (this->num_idx/3); i++) {
        // get 3 vertexs of a triangle
        obj_points * point_a = this->points_data + this->indecis[i * 3 + 0];
        obj_points * point_b = this->points_data + this->indecis[i * 3 + 1];
        obj_points * point_c = this->points_data + this->indecis[i * 3 + 2];
        glm::vec3 p1(point_a->vertex_x,
                point_a->vertex_y,
                point_a->vertex_z);
        glm::vec3 p2(point_b->vertex_x,
                point_b->vertex_y,
                point_b->vertex_z);
        glm::vec3 p3(point_c->vertex_x,
                point_c->vertex_y,
                point_c->vertex_z);
        // recalculate its normal
        glm::vec3 normal = glm::cross((p2 - p1), (p3 - p1));
        normal = glm::normalize(normal);
        // set new vertexs
        memcpy(new_points + i * 3 + 0, point_a, sizeof(obj_points));
        memcpy(new_points + i * 3 + 1, point_b, sizeof(obj_points));
        memcpy(new_points + i * 3 + 2, point_c, sizeof(obj_points));
        for (int j = 0; j < 3; j++) {
            // 判断是否需要新法线
            glm::vec3 ori_normal = glm::vec3(new_points[i * 3 + j].normal_x,
                    new_points[i * 3 + j].normal_y,
                    new_points[i * 3 + j].normal_z);
            ori_normal = glm::normalize(ori_normal);
            // 需要则设置新法线
//            tmpvector.push_back(glm::dot(ori_normal, normal));
            if (glm::dot(ori_normal, normal) < cos_angle) {
                new_points[i * 3 + j].normal_x = normal.x;
                new_points[i * 3 + j].normal_y = normal.y;
                new_points[i * 3 + j].normal_z = normal.z;
            }
            new_indecis[i * 3 + j] = i * 3 + j;
        }
    }

    // 释放旧数据
    free(this->indecis);
    free(this->points_data);

    // 调整数量
    this->points_data = new_points;
    this->indecis = new_indecis;
    this->num_pts = this->num_idx;
}

void mesh::convert_left_hand_to_right_hand() {
    obj_points * pt = this->points_data;
    for (int i = 0; i < this->num_pts; i++) {
        auto tmp = pt[i].vertex_z;
        pt[i].vertex_z = pt[i].vertex_y;
        pt[i].vertex_y = tmp;
    }
}

bool mesh::scale_model(glm::vec3 xyz, bool keep_texture_ratio) {
    if (this->data_buffered) {
        GL3D_UTILS_THROW("can not scale after buffer data in 3ds models");
        return false;
    }

    glm::mat4 sc = glm::scale(glm::mat4(1.0f), xyz);
    for (int i = 0; i < this->num_pts; ++i) {
        glm::vec3 point(this->points_data[i].vertex_x,
                        this->points_data[i].vertex_y,
                        this->points_data[i].vertex_z);
        glm::vec4 tmp = glm::vec4(point, 1.0f);
        tmp = sc * tmp;
        tmp = tmp / tmp.w;
        this->points_data[i].vertex_x = tmp.x;
        this->points_data[i].vertex_y = tmp.y;
        this->points_data[i].vertex_z = tmp.z;
        if (keep_texture_ratio) {
            tmp = glm::vec4(this->points_data[i].texture_x,
                            this->points_data[i].texture_y,
                            0.0f,
                            1.0f);
            tmp = sc * tmp;
            tmp = tmp / tmp.w;
            this->points_data[i].texture_x = tmp.x;
            this->points_data[i].texture_y = tmp.y;
        }
    }

    return true;
}

bool mesh::recalculate_boundings() {
    if (this->data_buffered)
        return false;

    glm::vec3 b_max = trans_point_vertex_to_vec3(this->points_data[0]);
    glm::vec3 b_min = trans_point_vertex_to_vec3(this->points_data[0]);
    for (int i = 0; i < this->num_pts; ++i) {
        glm::vec3 tmp = trans_point_vertex_to_vec3(this->points_data[i]);
        b_max = math::max_vec_every_element(b_max, tmp);
        b_min = math::min_vec_every_element(b_min, tmp);
    }

    this->bounding_value_max = b_max;
    this->bounding_value_min = b_min;

    return true;
}

glm::vec3 mesh::trans_point_vertex_to_vec3(obj_points pts) {
    return glm::vec3(pts.vertex_x, pts.vertex_y, pts.vertex_z);
}
