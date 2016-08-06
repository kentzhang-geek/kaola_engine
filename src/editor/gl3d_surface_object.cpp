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
    const QVector<Surface::Vertex*> * vertexes = sfc->getRenderingVertices();
    pts_len = vertexes->size();
    pts = (gl3d::obj_points *)
            malloc(sizeof(gl3d::obj_points) * pts_len);
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        pts[i].vertex_x = vertexes->at(i)->x();
        pts[i].vertex_y = vertexes->at(i)->y();
        pts[i].vertex_z = vertexes->at(i)->z();
        pts[i].texture_x = vertexes->at(i)->w();
        pts[i].texture_y = 1.0f - vertexes->at(i)->h();
    }

    // create index buffer
    GLushort * idxes = NULL;
    int idx_len;
    const QVector<GLushort> * indecis = sfc->getRenderingIndices();
    idx_len = indecis->size();
    idxes = new GLushort[idx_len];
    for (int i = 0; i < idx_len; i++) {
        idxes[i] = indecis->at(i);
    }

    // create new mesh
    gl3d::mesh * m = new gl3d::mesh(pts, pts_len, idxes, idx_len);
    m->set_material_index(this->tmp_idx++);
    this->get_meshes()->push_back(m);

    // clean env
    delete pts;
    delete idxes;

    // have conective surface , then process meshes
    vertexes = sfc->getConnectiveVerticies();
    indecis = sfc->getConnectiveIndicies();
    if ((vertexes != NULL) && (indecis != NULL)) {
        pts = (obj_points *)malloc(sizeof(obj_points) * vertexes->size());
        memset(pts, 0, sizeof(obj_points) * vertexes->size());
        idxes = (GLushort *)malloc(sizeof(GLushort) * indecis->size());
        memset(idxes, 0, sizeof(GLushort) * indecis->size());
        pts_len = vertexes->size();
        idx_len = indecis->size();
        for (int j = 0; j < vertexes->size(); j++) {
            pts[j].vertex_x = vertexes->at(j)->x();
            pts[j].vertex_y = vertexes->at(j)->y();
            pts[j].vertex_z = vertexes->at(j)->z();
            pts[j].texture_x = vertexes->at(j)->w();
            pts[j].texture_y = 1.0f - vertexes->at(j)->h();
        }
        for (int j = 0; j < indecis->size(); j++) {
            idxes[j] = indecis->at(j);
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
