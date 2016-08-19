#include "editor/design_scheme.h"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <QtGui>
#include <QtCore>
#include "editor/surface.h"

using namespace std;
//typedef CGAL::Quotient<float> Number_type;
typedef CGAL::Cartesian<float> Kernel;
typedef CGAL::Arr_segment_traits_2<Kernel> Traits_2;
typedef Traits_2::Point_2 Point_2;
typedef Traits_2::X_monotone_curve_2 Segment_2;
typedef CGAL::Arrangement_2<Traits_2> Arrangement_2;

using namespace gl3d;
using namespace std;
using namespace klm;
using namespace klm::design;

void design::scheme::init() {
    this->attached_scene = NULL;
    this->rooms.clear();
    this->walls.clear();
    this->drawing_walls.clear();
    this->set_render_authority(GL3D_SCENE_DRAW_GROUND | GL3D_SCENE_DRAW_SHADOW);
    this->set_control_authority(GL3D_OBJ_ENABLE_DEL
                                | GL3D_OBJ_ENABLE_CHANGEMTL
                                | GL3D_OBJ_ENABLE_PICKING);
    this->set_id(0);
    this->set_obj_type(this->type_scheme);
    this->wall_id_start = 10000;        // start wall id at 10000
}

design::scheme::scheme(gl3d::scene *sc) {
    this->init();
    this->attached_scene = sc;
    return;
}

bool scheme::is_data_changed() {
    return true;
}

bool scheme::is_visible() {
    return true;
}

glm::mat4 scheme::get_rotation_mat() {
    return glm::mat4(1.0f);
}

glm::mat4 scheme::get_translation_mat() {
    return glm::mat4(1.0f);
}

glm::mat4 scheme::get_scale_mat() {
    return glm::mat4(1.0f);
}

void scheme::get_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    for (auto it = this->rooms.begin();
         it != this->rooms.end();
         it++) {
        gl3d::surface_to_mesh((*it)->ground, ms);
    }

    return;
}

void scheme::clear_abstract_meshes(QVector<gl3d::mesh *> &ms) {
    for (auto it = ms.begin();
         it != ms.end();
         it++) {
        delete *it;
    }
    ms.clear();
    return;;
}

void scheme::get_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    gl3d::global_material_lib::shared_instance()->copy_mtls_pair_info(mt);
    return;
}

void scheme::clear_abstract_mtls(QMap<unsigned int, gl3d_material *> &mt) {
    mt.clear();
    return;
}

void scheme::set_translation_mat(const glm::mat4 &trans) {
    // should not use this
    cout << "should not use this" << endl;
    return;
}

bool scheme::add_wall(gl3d::gl3d_wall *w) {
    // TODO : test should cross walls now
    QSet<gl3d_wall *> wall_insert;
    wall_insert.clear();
    wall_insert.insert(w);
    Q_FOREACH(gl3d_wall *const &wit, this->walls) {
            if (gl3d::gl3d_wall::wall_cross(w, wit, wall_insert)) {
                this->attached_scene->delete_obj(wit->get_id());
                delete wit;
                wall_insert.remove(w);
                this->walls.remove(wit);
            }
        }

    Q_FOREACH(gl3d_wall *const &wit, wall_insert) {
            ++this->wall_id_start;
            wit->set_id(this->wall_id_start);
            this->attached_scene->add_obj(QPair<int, gl3d::abstract_object *>(this->wall_id_start, wit));
            this->walls.insert(wit);
        }

    this->recalculate_rooms();
    return true;
}

void scheme::del_wal(gl3d::gl3d_wall *w) {
    this->walls.remove(w);
    this->recalculate_rooms();
    return;
}

void scheme::release_rooms() {
    Q_FOREACH(gl3d::room * const &rit, this->rooms) {
            delete rit;
        }
    this->rooms.clear();
    Q_FOREACH(gl3d::gl3d_wall * const &wit, this->walls) {
            this->attached_scene->delete_obj(wit->get_id());
            delete wit;
        }
    this->walls.clear();

    return;
}

void scheme::delete_room(gl3d::room *r) {
    this->rooms.remove(r);
    // get all the rooms in scheme
    QSet<gl3d_wall *> wls;
    wls.clear();
    Q_FOREACH(gl3d_wall * const & wit, r->relate_walls) {
            wls.insert(wit);
        }
    // delete room
    delete r;
    // check is the wall need to delete
    Q_FOREACH(gl3d_wall * const &wit, wls) {
            if (wit->get_relate_rooms()->size() == 0) {
                this->attached_scene->delete_obj(wit->get_id());
                this->walls.remove(wit);
                delete wit;
            }
        }

    this->recalculate_rooms();
    return;
}

gl3d::room *scheme::get_room(glm::vec2 coord_on_screen) {
    // get ground coordinate
    glm::vec2 coord_grd;
    this->attached_scene->coord_ground(coord_on_screen, coord_grd);
    // check is point in surface
    QVector<math::triangle_facet> faces;
    for (auto it = this->rooms.begin();
         it != this->rooms.end();
         it++) {
        faces.clear();
        gl3d::get_faces_from_surface((*it)->ground, faces);
        for (auto fit = faces.begin();
             fit != faces.end();
             fit++) {
            if (fit->is_point_in_facet(math::convert_vec2_to_vec3(coord_grd))) {
                return *it;
            }
        }
    }

    return NULL;
}

static inline glm::vec3 arr_point_to_vec3(Point_2 pt) {
    glm::vec2 tmp(pt.x(), pt.y());
    return math::convert_vec2_to_vec3(tmp);
}

void scheme::recalculate_rooms() {
    // TODO : test recalculate rooms
    // release old walls
    Q_FOREACH(room * const &rit, this->rooms) {
            delete rit;
        }
    // clear relate room for walls
    Q_FOREACH(gl3d_wall * const &wit, this->walls) {
            wit->get_relate_rooms()->clear();
    }

    // use CGAL to calculate all the areas
    Arrangement_2 arr;
    Q_FOREACH(gl3d_wall * const &wit, this->walls) {
            Segment_2 seg_l = Segment_2(Point_2(wit->get_start_point().x, wit->get_start_point().y),
                                        Point_2(wit->get_end_point().x, wit->get_end_point().y));
            CGAL::insert(arr, seg_l);
        }

    // generate rooms
    for (Arrangement_2::Face_const_iterator fit = arr.faces_begin();
         fit != arr.faces_end();
         fit++) {
        if (!fit->is_unbounded()) {
            // has bounded, now get all vertex to construct the surface
            Arrangement_2::Ccb_halfedge_const_circulator cir = fit->outer_ccb();
            Arrangement_2::Ccb_halfedge_const_circulator cucir = cir;
            QVector<glm::vec3> grd;
            grd.clear();
            do {
                grd.push_back(arr_point_to_vec3(cucir->target()->point()));
                cucir++;
            } while (cucir != cir);
            room *r = new room();
            r->ground = new klm::Surface(grd);
            r->ground->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
            for (auto it = grd.begin();
                 it != grd.end();
                 it++) {
                for (auto wit = this->walls.begin();
                     wit != this->walls.end();
                     wit++) {
                    if (math::line_2d((*wit)->get_start_point(),
                                      (*wit)->get_end_point()).point_on_line(
                            glm::vec2(it->x, it->z))) { // now find related wall and room
                        (*wit)->get_relate_rooms()->insert(r);
                        r->relate_walls.insert(*wit);
                    }
                }
            }
            this->rooms.insert(r);
        }
    }

    return;
}

// test code
#if 0

void print_ccb(Arrangement_2::Ccb_halfedge_const_circulator circ) {
    Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
    std::cout << "(" << curr->source()->point() << ")";
    do {
        Arrangement_2::Ccb_halfedge_const_circulator he = curr;
        std::cout << " [" << he->curve() << "] "
                  << "(" << he->target()->point() << ")";
    } while (++curr != circ);
    std::cout << std::endl;
}

void print_face(Arrangement_2::Face_const_handle f) {
    // Print the outer boundary.
    if (f->is_unbounded())
        std::cout << "Unbounded face. " << std::endl;
    else {
        std::cout << "Outer boundary: ";
        print_ccb(f->outer_ccb());
    }
    // Print the boundary of each of the holes.
    Arrangement_2::Hole_const_iterator hi;
    int index = 1;
    for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
        std::cout << " Hole #" << index << ": ";
        print_ccb(*hi);
    }
    // Print the isolated vertices.
    Arrangement_2::Isolated_vertex_const_iterator iv;
    for (iv = f->isolated_vertices_begin(), index = 1;
         iv != f->isolated_vertices_end(); ++iv, ++index) {
        std::cout << " Isolated vertex #" << index << ": "
                  << "(" << iv->point() << ")" << std::endl;
    }
}

int main(int argc, char **argv) {
    Arrangement_2 arr;
    Segment_2 cv[4];
    Point_2 p1(0.0f, 0.0f), p2(0.0f, 4.0f), p3(4.0f, 0.0f);
    cv[0] = Segment_2(p1, p2);
    cv[1] = Segment_2(p2, p3);
    cv[2] = Segment_2(p3, p1);
    cv[3] = Segment_2(Point_2(-1.0f, 2.0f), Point_2(4.0f, 2.0f));
    CGAL::insert(arr, cv[0]);
    CGAL::insert(arr, cv[1]);
    CGAL::insert(arr, cv[2]);
    CGAL::insert(arr, cv[3]);

    for (Arrangement_2::Face_const_iterator fit = arr.faces_begin();
         fit != arr.faces_end();
         fit++) {
        if (!fit->is_unbounded()) {
            // has bounded
            Arrangement_2::Ccb_halfedge_const_circulator cir = fit->outer_ccb();
            Arrangement_2::Ccb_halfedge_const_circulator cucir = cir;
            do {
                cout << "(" << cucir->target()->point() << ") ";
                cucir++;
            } while (cucir != cir);
            cout << endl;
        }
    }

    for (Arrangement_2::Edge_const_iterator eit = arr.edges_begin();
         eit != arr.edges_end();
         eit++) {
        if (!eit->face()->is_unbounded()) {
            cout << "(" << eit->source()->point() << ") (" << eit->target()->point() << ") " << endl;
        }
    }

    return 0;
}

#endif
