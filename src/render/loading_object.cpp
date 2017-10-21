#include "kaola_engine/loading_object.h"

using namespace gl3d;
using namespace klm;

void loading_object::init() {
    // load material
    QImage img(QString(GL3D_PATH_BASE) + GL3D_PATH_SEPRATOR + "loading.jpg");
    gl3d_general_texture *gtext = new gl3d_general_texture(gl3d_general_texture::texture_type::GL3D_RGBA, img.width(),
                                                           img.height());
    gtext->buffer_data(img.bits());
    this->loading_mtl = new gl3d_material(gtext);
    // init properties
    this->position = glm::vec3(0.0f);
    this->roll_mat = glm::mat4(1.0f);
    this->bounds = glm::vec3(1.0f);
    this->real_res_id.clear();
    this->sfcs.clear();
    this->is_picked = false;
    // set abobj property
    this->set_obj_type(this->type_loading_object);
    this->set_pick_flag(false);
    this->set_data_buffered(false);
    this->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
    this->set_control_authority(
            GL3D_OBJ_ENABLE_MOVE | GL3D_OBJ_ENABLE_ROTATE | GL3D_OBJ_ENABLE_PICKING | GL3D_OBJ_ENABLE_DEL);
}

loading_object::loading_object(glm::vec3 xyz) {
    // check size
    for (int i = 0; i < xyz.length(); i++) {
        if (xyz[i] < 0.0f) {
            GL3D_UTILS_THROW("loading object size should not less than zero, %f %f %f", xyz.x, xyz.y, xyz.z);
        }
    }
    this->init();

    // remember text
    this->bounds = xyz;
    // create surfaces
    glm::vec3 size = this->bounds * 0.5f;
    size.y = size.y * 2.0f;
    klm::Surface * s;
    QVector<glm::vec3> pts;
    // top
    pts.clear();
    pts.push_back(glm::vec3(-size.x, size.y, -size.z));
    pts.push_back(glm::vec3(size.x, size.y, -size.z));
    pts.push_back(glm::vec3(size.x, size.y, size.z));
    pts.push_back(glm::vec3(-size.x, size.y, size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);
    // bottom
    pts.clear();
    pts.push_back(glm::vec3(-size.x, 0.0f, -size.z));
    pts.push_back(glm::vec3(-size.x, 0.0f, size.z));
    pts.push_back(glm::vec3(size.x, 0.0f, size.z));
    pts.push_back(glm::vec3(size.x, 0.0f, -size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);
    // left
    pts.clear();
    pts.push_back(glm::vec3(-size.x, 0.0f, -size.z));
    pts.push_back(glm::vec3(-size.x, size.y, -size.z));
    pts.push_back(glm::vec3(-size.x, size.y, size.z));
    pts.push_back(glm::vec3(-size.x, 0.0f, size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);
    // right
    pts.clear();
    pts.push_back(glm::vec3(size.x, 0.0f, -size.z));
    pts.push_back(glm::vec3(size.x, 0.0f, size.z));
    pts.push_back(glm::vec3(size.x, size.y, size.z));
    pts.push_back(glm::vec3(size.x, size.y, -size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);
    // front
    pts.clear();
    pts.push_back(glm::vec3(-size.x, 0.0f, -size.z));
    pts.push_back(glm::vec3(size.x, 0.0f, -size.z));
    pts.push_back(glm::vec3(size.x, size.y, -size.z));
    pts.push_back(glm::vec3(-size.x, size.y, -size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);
    // back
    pts.clear();
    pts.push_back(glm::vec3(-size.x, 0.0f, size.z));
    pts.push_back(glm::vec3(-size.x, size.y, size.z));
    pts.push_back(glm::vec3(size.x, size.y, size.z));
    pts.push_back(glm::vec3(size.x, 0.0f, size.z));
    s = new klm::Surface(pts);
    this->sfcs.push_back(s);

    // set sfc material
    Q_FOREACH(klm::Surface * sit, this->sfcs) {
            sit->setSurfaceMaterial(new Surfacing("mtl000001"));
        }

    return;
}

loading_object::~loading_object() {
    // delete material
    delete this->loading_mtl;
    // delete surfaces
    Q_FOREACH(auto sit, this->sfcs) {
            delete sit;
        }
    this->sfcs.clear();
    return;
}

bool loading_object::is_data_changed() {
    return true;
}

bool loading_object::is_visible() {
    return true;
}

glm::mat4 loading_object::get_translation_mat() {
    return glm::translate(glm::mat4(1.0f), this->position) * this->roll_mat;
}

glm::mat4 loading_object::get_scale_mat() {
    return glm::mat4(1.0f);
}

void loading_object::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    Q_FOREACH(auto sit, this->sfcs) {
            gl3d::surface_to_mesh(sit, ms);
        }

    Q_FOREACH(gl3d::mesh * mit, ms) {
            mit->set_material_index(1);
            if (this->is_picked) {
                mit->set_is_blink(true);
            }
            else {
                mit->set_is_blink(false);
            }
        }
    return;
}

void loading_object::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    Q_FOREACH(auto mit, ms) {
            delete mit;
        }
    ms.clear();
}

void loading_object::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    mt.remove(1);
    mt.insert(1, this->loading_mtl);
    return;
}

void loading_object::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    mt.clear();
    return;
}

void loading_object::set_translation_mat(const glm::mat4 &trans) {
    glm::vec4 pt(0.0f);
    pt.w = 1.0f;
    pt = trans * pt;
    pt = pt / pt.w;
    this->position = glm::vec3(pt);
    this->roll_mat = glm::inverse(glm::translate(glm::mat4(1.0f), this->position)) * trans;
    return;
}

glm::mat4 loading_object::get_rotation_mat() {
    return this->roll_mat;
}
