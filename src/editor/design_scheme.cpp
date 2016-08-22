#include "editor/design_scheme.h"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <QtGui>
#include <QtCore>
#include "editor/surface.h"
#include "utils/gl3d_lock.h"

using namespace std;
typedef CGAL::Quotient<CGAL::MP_Float> Number_type;
typedef CGAL::Cartesian<Number_type> Kernel;
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
    if (!gl3d_lock::shared_instance()->scheme_lock.tryLock()) {
        return;
    }
    Q_FOREACH(gl3d::room *rit, this->rooms) {
            gl3d::surface_to_mesh(rit->ground, ms);
        }

    Q_FOREACH(gl3d::mesh *mit, ms) {
            mit->set_texture_repeat(true);
        }

    gl3d_lock::shared_instance()->scheme_lock.unlock();
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
    if (!gl3d_lock::shared_instance()->scheme_lock.tryLock()) {
        return;
    }
    gl3d::global_material_lib::shared_instance()->copy_mtls_pair_info(mt);
    gl3d_lock::shared_instance()->scheme_lock.unlock();
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

bool scheme::add_wall(gl3d::gl3d_wall *w, gl3d::gl3d_wall *&wall_to_end) {
    gl3d_lock::shared_instance()->scheme_lock.lock();
    // TODO : test should cross walls now
    QSet<gl3d_wall *> wall_insert;
    wall_insert.clear();
    wall_insert.insert(w);
    wall_to_end = NULL;
    this->attached_scene->delete_obj(w->get_id());
    int tid = w->get_id();
    // TODO : debug cross walls
    QVector<glm::vec2> cross_pts;
    Q_FOREACH(gl3d_wall *const &wit, this->walls) {
            // cut walls in scheme
            if (gl3d::gl3d_wall::wall_cross(w, wit, wall_insert)) {
                tid = wit->get_id();
                this->attached_scene->delete_obj(wit->get_id());
                this->walls.remove(wit);
                glm::vec2 ptin;
                math::get_cross(
                        math::line_2d(w->get_start_point(), w->get_end_point()),
                        math::line_2d(wit->get_start_point(), wit->get_end_point()),
                        ptin);
                cross_pts.push_back(ptin);
                delete wit;
            }
        }

    if (cross_pts.size() > 0) {
        // cut new wall
        wall_insert.remove(w);
        math::sort_vector_by_distance(cross_pts, w->get_start_point());
        cross_pts.insert(0, w->get_start_point());
        cross_pts.push_back(w->get_end_point());
        for (int i = 0; i < (cross_pts.size() - 1); i++) {
            gl3d_wall *n_w = new gl3d_wall(cross_pts[i], cross_pts[i + 1], w->get_thickness(), w->get_hight());
            if (i == 0) {
                // head with start
                if (w->get_start_point_fixed()) {
                    if (w->get_start_point_attach()->attach_point == gl3d::gl3d_wall_attach::start_point) {
                        w->get_start_point_attach()->attach->get_start_point_attach()->attach = n_w;
                    }
                    else {
                        w->get_start_point_attach()->attach->get_end_point_attach()->attach = n_w;
                    }
                    n_w->get_start_point_attach()->attach = w->get_start_point_attach()->attach;
                    n_w->get_start_point_attach()->attach_point = w->get_start_point_attach()->attach_point;
                    // renew old wall attach
                    w->set_start_point_fixed(false);
                    w->get_start_point_attach()->attach = NULL;
                }
            }
            if (i == (cross_pts.size() - 1)) {
                // tail with end
                if (w->get_end_point_fixed()) {
                    if (w->get_end_point_attach()->attach_point == gl3d::gl3d_wall_attach::start_point) {
                        w->get_end_point_attach()->attach->get_start_point_attach()->attach = n_w;
                    }
                    else {
                        w->get_end_point_attach()->attach->get_end_point_attach()->attach = n_w;
                    }
                    n_w->get_end_point_attach()->attach = w->get_end_point_attach()->attach;
                    n_w->get_end_point_attach()->attach_point = w->get_end_point_attach()->attach_point;
                    // renew old wall attach
                    w->set_end_point_fixed(false);
                    w->get_end_point_attach()->attach = NULL;
                }
            }
            wall_to_end = n_w;
            wall_insert.insert(n_w);
        }
        delete w;
    }

    Q_FOREACH(gl3d_wall *const &wit, wall_insert) {
            ++this->wall_id_start;
            wit->set_id(this->wall_id_start);
            this->attached_scene->add_obj(QPair<int, gl3d::abstract_object *>(this->wall_id_start, wit));
            this->walls.insert(wit);
        }

    this->recalculate_rooms();
    gl3d_lock::shared_instance()->scheme_lock.unlock();
    return true;
}

void scheme::del_wal(gl3d::gl3d_wall *w) {
    gl3d_lock::shared_instance()->scheme_lock.lock();
    this->walls.remove(w);
    this->recalculate_rooms();
    gl3d_lock::shared_instance()->scheme_lock.unlock();
    return;
}

void scheme::release_rooms() {
    gl3d_lock::shared_instance()->scheme_lock.lock();
    Q_FOREACH(gl3d::room *const &rit, this->rooms) {
            delete rit;
        }
    this->rooms.clear();
    Q_FOREACH(gl3d::gl3d_wall *const &wit, this->walls) {
            this->attached_scene->delete_obj(wit->get_id());
            delete wit;
        }
    this->walls.clear();

    gl3d_lock::shared_instance()->scheme_lock.unlock();
    return;
}

void scheme::delete_room(gl3d::room *r) {
    gl3d_lock::shared_instance()->scheme_lock.lock();
    this->rooms.remove(r);
    // get all the rooms in scheme
    QSet<gl3d_wall *> wls;
    wls.clear();
    Q_FOREACH(gl3d_wall *const &wit, r->relate_walls) {
            wls.insert(wit);
        }
    // delete room
    delete r;
    // check is the wall need to delete
    Q_FOREACH(gl3d_wall *const &wit, wls) {
            if (wit->get_relate_rooms()->size() == 0) {
                this->attached_scene->delete_obj(wit->get_id());
                this->walls.remove(wit);
                delete wit;
            }
        }

    this->recalculate_rooms();
    gl3d_lock::shared_instance()->scheme_lock.unlock();
    return;
}

gl3d::room *scheme::get_room(glm::vec2 coord_on_screen) {
    // get ground coordinate
    glm::vec2 coord_grd;
    this->attached_scene->coord_ground(coord_on_screen, coord_grd);
    // check is point in surface
    QVector<math::triangle_facet> faces;
    Q_FOREACH(room *rit, this->rooms) {
            faces.clear();
            gl3d::get_faces_from_surface(rit->ground, faces);
            Q_FOREACH(math::triangle_facet fit, faces) {
                    if (fit.is_point_in_facet(math::convert_vec2_to_vec3(coord_grd))) {
                        return rit;
                    }
                }
        }

    return NULL;
}

// TODO : convert point to vec
static inline glm::vec3 arr_point_to_vec3(Point_2 pt) {
    double tx = CGAL::to_double(pt.x());
    double ty = CGAL::to_double(pt.y());
    glm::vec2 tmp((float) tx, (float) ty);
    return math::convert_vec2_to_vec3(tmp);
}

void scheme::recalculate_rooms() {
    // TODO : test recalculate rooms
    // release old rooms
    Q_FOREACH(room *const &rit, this->rooms) {
            delete rit;
        }
    this->rooms.clear();
    // clear relate room for walls
    Q_FOREACH(gl3d_wall *const &wit, this->walls) {
            wit->get_relate_rooms()->clear();
        }

    // use CGAL to calculate all the areas
    Arrangement_2 arr;
    QVector<math::line_2d> lns;
    Q_FOREACH(gl3d_wall *const &wit, this->walls) {
            Segment_2 seg_l = Segment_2(Point_2(wit->get_start_point().x, wit->get_start_point().y),
                                        Point_2(wit->get_end_point().x, wit->get_end_point().y));
            lns.push_back(math::line_2d(wit->get_start_point(), wit->get_end_point()));
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
            QVector<glm::vec3> tmp_test; // TODO : remove this
            do {
                glm::vec3 pt = arr_point_to_vec3(cucir->target()->point());
                if ((grd.size() > 2) && (pt == grd.at(grd.size() - 2))) {
                    // TODO : process isolated lines
                    grd.removeLast();
                }
                else {
                    grd.push_back(pt);
                }
                tmp_test.push_back(pt);
                cucir++;
            } while (cucir != cir);
            if (grd.size() >= 3) {
                room *r = new room();
                r->ground = new klm::Surface(grd);
                r->ground->setSurfaceMaterial(new klm::Surfacing("mtl000000"));
                Q_FOREACH(glm::vec3 pit, grd) {
                        Q_FOREACH(gl3d_wall *wit, this->walls) {
                                if (math::line_2d(wit->get_start_point(),
                                                  wit->get_end_point()).point_on_line(
                                        glm::vec2(pit.x, pit.z))) { // now find related wall and room
                                    wit->get_relate_rooms()->insert(r);
                                    r->relate_walls.insert(wit);
                                }
                            }
                    }
                this->rooms.insert(r);
            }
            if (fit->number_of_holes() > 0) {
                // TODO : process holes
            }
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
    QVector<Segment_2> cvs;
    cvs.push_back(Segment_2(Point_2(0.0, 0.0), Point_2(0.0, 4.0)));
    cvs.push_back(Segment_2(Point_2(0.0, 0.0), Point_2(4.0, 0.0)));
    cvs.push_back(Segment_2(Point_2(0.0, 4.0), Point_2(4.0, 0.0)));
    cvs.push_back(Segment_2(Point_2(1.0, 1.0), Point_2(1.0, 2.0)));
    cvs.push_back(Segment_2(Point_2(1.0, 1.0), Point_2(2.0, 1.0)));
    cvs.push_back(Segment_2(Point_2(1.0, 2.0), Point_2(2.0, 1.0)));
    CGAL::insert(arr, cvs.begin(), cvs.end());

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
            if (fit->number_of_holes() > 0) {
                cout << " has hole : ";
                for (Arrangement_2::Hole_const_iterator hit = fit->holes_begin();
                        hit != fit->holes_end();
                        hit++) {
                    Arrangement_2::Ccb_halfedge_const_circulator hcir = *hit;
                    Arrangement_2::Ccb_halfedge_const_circulator hcicur = hcir;
                    do {
                        cout << "(" << hcicur->target()->point() << ") ";
                        hcicur--;
                    } while (hcicur != hcir);
                }
            }
            cout << endl;
        }
    }

//    for (Arrangement_2::Edge_const_iterator eit = arr.edges_begin();
//         eit != arr.edges_end();
//         eit++) {
//        if (!eit->face()->is_unbounded()) {
//            cout << "(" << eit->source()->point() << ") (" << eit->target()->point() << ") " << endl;
//        }
//    }

    return 0;
}

#endif