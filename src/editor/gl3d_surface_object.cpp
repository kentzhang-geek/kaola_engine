#include "editor/gl3d_surface_object.h"

using namespace gl3d;

surface_object::surface_object(Surface *sfc) : object() {
    this->tmp_idx = 0;
    this->iter_surface(sfc);

    this->get_mtls()->clear();
    this->set_repeat(true);

    this->get_property()->scale_unit = gl3d::scale::m;
    this->set_control_authority(GL3D_OBJ_ENABLE_ALL & (~GL3D_OBJ_ENABLE_CULLING));
    this->set_render_authority(GL3D_SCENE_DRAW_NORMAL);
    this->set_obj_type(this->type_special);
}

void surface_object::iter_surface(Surface *sfc) {
    int cntSubsfc = sfc->getSurfaceCnt();
    if (cntSubsfc > 0) {
        for (int i = 0; i < cntSubsfc; i++) {
            this->iter_surface(sfc->getSubSurface(i));
        }
    }

    // create vertex buffer
    int pts_len;
    gl3d::obj_points * pts = NULL;
    pts_len = sfc->getRenderingVertices()->size();
    pts = (gl3d::obj_points *)
            malloc(sizeof(gl3d::obj_points) * pts_len);
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        pts[i].vertex_x = sfc->getRenderingVertices()->at(i)->x();
        pts[i].vertex_y = sfc->getRenderingVertices()->at(i)->y();
        pts[i].vertex_z = sfc->getRenderingVertices()->at(i)->z();
        pts[i].texture_x = sfc->getRenderingVertices()->at(i)->w();
        pts[i].texture_y = 1.0f - sfc->getRenderingVertices()->at(i)->h();
    }

    // create index buffer
    GLushort * idxes = NULL;
    int idx_len;
    idx_len = sfc->getRenderingIndices()->size();
    idxes = new GLushort[idx_len];
    for (int i = 0; i < idx_len; i++) {
        idxes[i] = sfc->getRenderingIndices()->at(i);
    }

    // create new mesh
    gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
    m->set_material_index(this->tmp_idx++);
    this->get_meshes()->push_back(m);

    // clean env
    delete pts;
    delete idxes;

    // have conective surface , then process meshes
    QVector<Surface::Vertex*> * vertexes = sfc->getConnectiveVerticies();
    QVector<GLushort> * indecis = sfc->getConnectiveIndicies();
    if ((vertexes != NULL) && (indecis != NULL)) {
        pts = (obj_points *)malloc(sizeof(obj_points) * vertexes->size());
        memset(pts, 0, sizeof(obj_points) * vertexes->size());
        idxes = (GLushort *)malloc(sizeof(GLushort) * indecis->size());
        memset(idxes, 0, sizeof(GLushort) * indecis->size());
        pts_len = vertexes->size();
        idx_len = indecis->size();
        for (int j = 0; j < vertexes->size(); j++) {
            glm::vec4 tmp_vert = glm::vec4(
                        vertexes->at(j)->getX(),
                        vertexes->at(j)->getY(),
                        vertexes->at(j)->getZ(),
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
