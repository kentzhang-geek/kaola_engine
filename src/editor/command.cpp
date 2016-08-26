#include "editor/command.h"

#include "kaola_engine/gl3d.hpp"
#include "editor/gl3d_wall.h"

using namespace klm;
using namespace gl3d;
using namespace klm::design;
using namespace klm::command;

static command_stack *one_stack = NULL;

command_stack::command_stack(gl3d::scene *sc, klm::design::scheme *sch) {
    this->main_scene = sc;
    this->sketch = sch;
    return;
}

void command_stack::init(gl3d::scene *sc, klm::design::scheme *sch) {
    if (NULL == one_stack) {
        one_stack = new command_stack(sc, sch);
    }
    return;
}

command_stack *command_stack::shared_instance() {
    if (NULL == one_stack)
        GL3D_UTILS_THROW("command stack is NULL");

    return one_stack;
}

add_wall::add_wall(gl3d_wall *w) {
    this->setText("add_wall");
    this->start_pos = w->get_start_point();
    this->end_pos = w->get_end_point();
    this->thickness = w->get_thickness();
    this->height = w->get_hight();
    this->start_attach = *w->get_start_point_attach();
    this->end_attach = *w->get_end_point_attach();
    this->wall_id = w->get_id();
}

void add_wall::undo() {
    Q_FOREACH(gl3d_wall * w, *(command_stack::shared_instance()->get_sketch()->get_walls())) {
            math::line_2d w_l(this->start_pos, this->end_pos);
            if (w_l.point_on_line(w->get_start_point()) && w_l.point_on_line(w->get_end_point())) {
                command_stack::shared_instance()->get_sketch()->del_wal(w);
            }
        }
}

void add_wall::redo() {
    gl3d_wall * n_w = new gl3d_wall(this->start_pos, this->end_pos, this->thickness, this->height);
    gl3d_wall * tmp;
    n_w->set_start_point_attach(this->start_attach);
    n_w->set_end_point_attach(this->end_attach);
    command_stack::shared_instance()->get_sketch()->add_wall(n_w, tmp);
    command_stack::shared_instance()->get_main_scene()->delete_obj(n_w->get_id());
    n_w->set_id(this->wall_id);
    command_stack::shared_instance()->get_main_scene()->get_objects()->insert(this->wall_id, n_w);
    return;
}

bool add_wall::mergeWith(const QUndoCommand *other) {
    return false;
}

del_wall::del_wall(gl3d_wall *w) {
    this->setText("del_wall");
    this->start_pos = w->get_start_point();
    this->end_pos = w->get_end_point();
    this->thickness = w->get_thickness();
    this->height = w->get_hight();
    this->start_attach = *w->get_start_point_attach();
    this->end_attach = *w->get_end_point_attach();
    this->wall_id = w->get_id();
}

void del_wall::undo() {
    gl3d_wall * n_w = new gl3d_wall(this->start_pos, this->end_pos, this->thickness, this->height);
    gl3d_wall * tmp;
    n_w->set_start_point_attach(this->start_attach);
    n_w->set_end_point_attach(this->end_attach);
    command_stack::shared_instance()->get_sketch()->add_wall(n_w, tmp);
    // change id of new wall to origin wall
    command_stack::shared_instance()->get_main_scene()->delete_obj(n_w->get_id());
    n_w->set_id(this->wall_id);
    command_stack::shared_instance()->get_main_scene()->get_objects()->insert(this->wall_id, n_w);
    return;
}

void del_wall::redo() {
    Q_FOREACH(gl3d_wall * w, *(command_stack::shared_instance()->get_sketch()->get_walls())) {
            math::line_2d w_l(this->start_pos, this->end_pos);
            if (w_l.point_on_line(w->get_start_point()) && w_l.point_on_line(w->get_end_point())) {
                command_stack::shared_instance()->get_sketch()->del_wal(w);
            }
        }
}

set_wall_property::set_wall_property(gl3d_wall *w) {
    this->setText("set_wall_property");
    this->ori_start_pos = w->get_start_point();
    this->ori_end_pos = w->get_end_point();
    this->ori_thickness = w->get_thickness();
    this->ori_height = w->get_hight();
    this->ori_start_attach = *w->get_start_point_attach();
    this->ori_end_attach = *w->get_end_point_attach();
    this->wall_id = w->get_id();
}

void set_wall_property::undo() {
    gl3d_wall * w = (gl3d_wall *) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    this->tar_start_pos = w->get_start_point();
    this->tar_end_pos = w->get_end_point();
    this->tar_thickness = w->get_thickness();
    this->tar_height = w->get_hight();
    this->tar_start_attach = *w->get_start_point_attach();
    this->tar_end_attach = *w->get_end_point_attach();

    w->set_start_point(this->ori_start_pos);
    w->set_end_point(this->ori_end_pos);
    w->set_thickness(this->ori_thickness);
    w->set_hight(this->ori_height);
    w->set_start_point_attach(this->ori_start_attach);
    w->set_end_point_attach(this->ori_end_attach);
    w->calculate_mesh();
    command_stack::shared_instance()->get_sketch()->recalculate_rooms();
}

void set_wall_property::redo() {
    gl3d_wall * w = (gl3d_wall *) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    w->set_start_point(this->tar_start_pos);
    w->set_end_point(this->tar_end_pos);
    w->set_thickness(this->tar_thickness);
    w->set_hight(this->tar_height);
    w->set_start_point_attach(this->tar_start_attach);
    w->set_end_point_attach(this->tar_end_attach);
    w->calculate_mesh();
    command_stack::shared_instance()->get_sketch()->recalculate_rooms();
}

int set_wall_property::id() const {
    return KLM_CMD_ID_SET_WALL;
}

bool set_wall_property::mergeWith(const QUndoCommand *other) {
    if (other->id() == 1) {
        set_wall_property * p = (set_wall_property *)other;
        gl3d_wall * w1 = (gl3d_wall *) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
        gl3d_wall * w2 = (gl3d_wall *) command_stack::shared_instance()->get_main_scene()->get_obj(p->wall_id);
        if (w1->get_id() == w2->get_id()) {
            this->tar_start_pos = p->tar_start_pos;
            this->tar_end_pos = p->tar_end_pos;
            this->tar_height = p->tar_height;
            this->tar_thickness = p->tar_thickness;
            this->tar_start_attach = p->tar_start_attach;
            this->tar_end_attach = p->tar_end_attach;
            return true;
        }
        else
            return false;
    }
    else {
        return false;
    }
}

add_hole::add_hole(hole * h, gl3d_wall *w, glm::vec3 center_point, float _width, float _min_height, float _max_height) {
    this->setText("add_hole");
    this->center = center_point;
    this->width = _width;
    this->min_height = _min_height;
    this->max_height = _max_height;
    this->wall_id = w->get_id();
    this->hole_id = h->get_hole_id();
}

void add_hole::undo() {
    gl3d_wall * w = (gl3d_wall * ) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    if (w->holes_on_this_wall.contains(this->hole_id)) {
        hole *h = w->holes_on_this_wall.value(this->hole_id);
        w->holes_on_this_wall.remove(this->hole_id);
        delete h;
        w->calculate_mesh();
    }
    else {
        // has failed
    }
}

void add_hole::redo() {
    gl3d_wall * w = (gl3d_wall * ) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    hole * ph = new hole(w, this->center, this->width, this->min_height, this->max_height);
    // change key of the hole
    if (ph->is_valid()) {
        w->holes_on_this_wall.remove(ph->get_hole_id());
        ph->set_hole_id(this->hole_id);
        w->holes_on_this_wall.insert(this->hole_id, ph);
        w->calculate_mesh();
    }
    else {
        // has failed
    }

    return;
}

del_hole::del_hole(hole *h, gl3d_wall *w) {
    this->setText("del_hole");
    this->pa = h->get_pta();
    this->pb = h->get_ptb();
    this->wall_id = w->get_id();
    this->hole_id = h->get_hole_id();
}

del_hole::del_hole(int hid, gl3d_wall *w) {
    this->setText("del_hole");
    hole * h = w->holes_on_this_wall.value(hid);
    this->hole_id = hid;
    this->pa = h->get_pta();
    this->pb = h->get_ptb();
    this->wall_id = w->get_id();
}

void del_hole::undo() {
    gl3d_wall * w = (gl3d_wall * ) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    hole * h = new hole(w, this->pa, this->pb);
    if (h->is_valid()) {
        // change key
        w->holes_on_this_wall.remove(this->hole_id);
        h->set_hole_id(this->hole_id);
        w->holes_on_this_wall.insert(this->hole_id, h);
        w->calculate_mesh();
    }
    else {
        // has failed
    }

}

void del_hole::redo() {
    gl3d_wall * w = (gl3d_wall * ) command_stack::shared_instance()->get_main_scene()->get_obj(this->wall_id);
    if (w->holes_on_this_wall.contains(this->hole_id)) {
        hole *h = w->holes_on_this_wall.value(this->hole_id);
        w->holes_on_this_wall.remove(this->hole_id);
        delete h;
        w->calculate_mesh();
    }
    else {
        // has failed
    }
}