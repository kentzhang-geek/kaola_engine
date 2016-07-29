#include "editor/gl3d_surface_object.h"

using namespace gl3d;

surface_object::surface_object(Surface *sfc) : object() {
    this->tmp_idx = 0;
    this->iter_surface(sfc);

    this->get_mtls()->clear();
    this->set_repeat(true);

    this->get_property()->scale_unit = gl3d::scale::m;
    this->get_property()->authority = GL3D_OBJ_ENABLE_ALL & (~GL3D_OBJ_ENABLE_CULLING);
    this->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;
}

void surface_object::iter_surface(Surface *sfc) {
    int cntSubsfc = sfc->getSubSurfaceCnt();
    if (cntSubsfc > 0) {
        for (int i = 0; i < cntSubsfc; i++) {
            this->iter_surface(sfc->getSubSurface(i));
        }
    }

    // process local verticles
    // get trans mat
    glm::mat4 trans_mat(1.0);
    sfc->getTransFromParent(trans_mat);

    // create vertex buffer
    GLfloat * tmp_data = NULL;
    int pts_len;
    gl3d::obj_points * pts = NULL;
    sfc->getRenderingVertices(tmp_data, pts_len);
    pts_len = pts_len / klm::Vertex::VERTEX_SIZE;
    pts = (gl3d::obj_points *)
            malloc(sizeof(gl3d::obj_points) * pts_len);
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        glm::vec4 tmp_vert = glm::vec4(
                    tmp_data[i * 5 + 0],
                tmp_data[i * 5 + 1],
                tmp_data[i * 5 + 2],
                1.0f);
        tmp_vert = trans_mat * tmp_vert;
        tmp_vert = tmp_vert / tmp_vert.w;
        pts[i].vertex_x = tmp_vert.x;
        pts[i].vertex_y = tmp_vert.y;
        pts[i].vertex_z = tmp_vert.z;
        pts[i].texture_x = tmp_data[i * 5 + 3];
        pts[i].texture_y = 1.0f - tmp_data[i * 5 + 4];
    }

    // create index buffer
    GLushort * idxes = NULL;
    int idx_len;
    sfc->getRenderingIndicies(idxes, idx_len);

    if (idxes == NULL) {
        cout << "aaaaa";

    }

    // create new mesh
    gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
    m->set_material_index(this->tmp_idx++);
    this->get_meshes()->push_back(m);

    // clean env
    free(pts);
    free(idxes);

    // process conective surface
    QVector<klm::Vertex *> vertexes;
    QVector<GLushort> indecis;
    sfc->getConnectiveVerticies(vertexes);
    sfc->getConnectiveIndicies(indecis);
    trans_mat = glm::mat4(1.0);
    if (sfc->getParent() != NULL) {
        sfc->getParent()->getTransFromParent(trans_mat);
    }
    // have conective surface , then process meshes
    if ((vertexes.size() > 0) && (indecis.size() > 0)) {
        pts = (obj_points *)malloc(sizeof(obj_points) * vertexes.size());
        memset(pts, 0, sizeof(obj_points) * vertexes.size());
        idxes = (GLushort *)malloc(sizeof(GLushort) * indecis.size());
        memset(idxes, 0, sizeof(GLushort) * indecis.size());
        pts_len = vertexes.size();
        idx_len = indecis.size();
        for (int j = 0; j < vertexes.size(); j++) {
            glm::vec4 tmp_vert = glm::vec4(
                        vertexes.at(j)->getX(),
                        vertexes.at(j)->getY(),
                        vertexes.at(j)->getZ(),
                        1.0f);
            tmp_vert = trans_mat * tmp_vert;
            tmp_vert = tmp_vert / tmp_vert.w;
            pts[j].vertex_x = tmp_vert.x;
            pts[j].vertex_y = tmp_vert.y;
            pts[j].vertex_z = tmp_vert.z;
            pts[j].texture_x = vertexes.at(j)->getW();
            pts[j].texture_y = 1.0f - vertexes.at(j)->getH();
        }
        for (int j = 0; j < indecis.size(); j++) {
            idxes[j] = indecis.at(j);
        }
        // create new mesh
        gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
        m->set_material_index(this->tmp_idx++);
        this->get_meshes()->push_back(m);
        // clean env
        free(pts);
        free(idxes);
    }
    return;
}
