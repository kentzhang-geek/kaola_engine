#include "editor/design_scheme.h"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include <QtGui>
#include <QtCore>
using namespace std;
//typedef CGAL::Quotient<float> Number_type;
typedef CGAL::Cartesian<float> Kernel;
typedef CGAL::Arr_segment_traits_2<Kernel> Traits_2;
typedef Traits_2::Point_2 Point_2;
typedef Traits_2::X_monotone_curve_2 Segment_2;
typedef CGAL::Arrangement_2<Traits_2> Arrangement_2;

static glm::vec2 start_position;
static float x_vt = 10.0f;
glm::vec2 trans_coord(Point_2 pt) {
    glm::vec2 ret;
    ret.x = (float) pt.x();
    ret.y = (float) pt.y();
    ret = ret * x_vt;
    ret = ret + start_position;
    return ret;
}

void print_ccb (Arrangement_2::Ccb_halfedge_const_circulator circ)
{
    Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
    std::cout << "(" << curr->source()->point() << ")";
    do {
        Arrangement_2::Ccb_halfedge_const_circulator he = curr;
        std::cout << " [" << he->curve() << "] "
                  << "(" << he->target()->point() << ")";
    } while (++curr != circ);
    std::cout << std::endl;
}

void print_face (Arrangement_2::Face_const_handle f)
{
    // Print the outer boundary.
    if (f->is_unbounded())
        std::cout << "Unbounded face. " << std::endl;
    else {
        std::cout << "Outer boundary: ";
        print_ccb (f->outer_ccb());
    }
    // Print the boundary of each of the holes.
    Arrangement_2::Hole_const_iterator hi;
    int index = 1;
    for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
        std::cout << " Hole #" << index << ": ";
        print_ccb (*hi);
    }
    // Print the isolated vertices.
    Arrangement_2::Isolated_vertex_const_iterator iv;
    for (iv = f->isolated_vertices_begin(), index = 1;
         iv != f->isolated_vertices_end(); ++iv, ++index){
        std::cout << " Isolated vertex #" << index << ": "
                  << "(" << iv->point() << ")" << std::endl;
    }
}

int main(int argc, char ** argv) {
    QApplication a(argc, argv);
    QWidget * disp = new QWidget(NULL);
    disp->show();
    QPainter ptr(disp);
    disp->setAutoFillBackground(true);
    start_position.x = (float) disp->width();
    start_position.y = (float) disp->height();
    Arrangement_2 arr;
    Segment_2 cv[4];
    Point_2 p1(0.0f, 0.0f), p2(0.0f, 4.0f), p3(4.0f, 0.0f);
    cv[0] = Segment_2(p1, p2);
    cv[1] = Segment_2(p2, p3);
    cv[2] = Segment_2(p3, p1);
    cv[3] = Segment_2(Point_2(-1.0f, 2.0f), Point_2(4.0f, 2.0f));
    CGAL::insert(arr, &cv[0], &cv[4]);

    for (Arrangement_2::Face_const_iterator fit = arr.faces_begin();
            fit != arr.faces_end();
            fit++) {
        if (!fit->is_unbounded()) {
            // has bounded
            print_face(fit);
        }
    }

    return a.exec();
}
