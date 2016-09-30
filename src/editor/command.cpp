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
    this->clear();
    return;
}

void command_stack::init(gl3d::scene *sc, klm::design::scheme *sch) {
    if (NULL != one_stack)
        delete one_stack;
    one_stack = new command_stack(sc, sch);
    return;
}

command_stack *command_stack::shared_instance() {
    if (NULL == one_stack)
        GL3D_UTILS_THROW("command stack is NULL");

    return one_stack;
}

add_or_del_wall::add_or_del_wall(gl3d_wall *w) {
    this->start_pos = w->get_start_point();
    this->end_pos = w->get_end_point();
    this->thickness = w->get_thickness();
    this->height = w->get_hight();
    this->start_attach = *w->get_start_point_attach();
    this->end_attach = *w->get_end_point_attach();
    this->wall_id = w->get_id();
    this->wall = w;
    this->start_fixed = w->get_start_point_fixed();
    this->end_fixed = w->get_end_point_fixed();
    this->wall_exist = true;
}

add_or_del_wall::~add_or_del_wall() {
    if (!this->wall_exist) {
        delete this->wall;
    }
}

void add_or_del_wall::combine_wall() {
    if (this->start_fixed) {
        if (this->start_attach.attach_point = gl3d_wall_attach::start_point) {
            gl3d_wall::combine(this->wall, this->start_attach.attach, gl3d_wall::combine_wall1_start_to_wall2_start);
        }
        else {
            gl3d_wall::combine(this->wall, this->start_attach.attach, gl3d_wall::combine_wall1_start_to_wall2_end);
        }
    }
    if (this->end_fixed) {
        if (this->end_attach.attach_point = gl3d_wall_attach::start_point) {
            gl3d_wall::combine(this->wall, this->end_attach.attach, gl3d_wall::combine_wall1_end_to_wall2_start);
        }
        else {
            gl3d_wall::combine(this->wall, this->end_attach.attach, gl3d_wall::combine_wall1_end_to_wall2_end);
        }
    }
}

bool add_or_del_wall::add_wall() {
    command_stack::shared_instance()->get_main_scene()->delete_obj(this->wall_id);
    this->wall->set_id(this->wall_id);
//    this->combine_wall();
    this->wall->calculate_mesh();
    this->wall->buffer_data();
    command_stack::shared_instance()->get_sketch()->get_objects()->insert(this->wall_id, this->wall);
    command_stack::shared_instance()->get_sketch()->get_walls()->insert(this->wall);
    this->wall_exist = true;
    command_stack::shared_instance()->get_sketch()->recalculate_rooms();
    return true;
}

bool add_or_del_wall::del_wall() {
    scheme * skt = command_stack::shared_instance()->get_sketch();
    if (skt->get_objects()->contains(this->wall_id)) {
        gl3d_wall * w = (gl3d_wall *)skt->get_obj(this->wall_id);
        skt->get_walls()->remove(w);
        command_stack::shared_instance()->get_main_scene()->delete_obj(this->wall_id);
        if (this->wall->get_start_point_fixed()) {
            gl3d_wall * w = this->wall->get_start_point_attach()->attach;
            this->wall->seperate(*this->wall->get_start_point_attach());
            w->calculate_mesh();
        }
        if (this->wall->get_end_point_fixed()) {
            gl3d_wall * w = this->wall->get_end_point_attach()->attach;
            this->wall->seperate(*this->wall->get_end_point_attach());
            w->calculate_mesh();
        }
    }
//    Q_FOREACH(gl3d_wall * w, *(command_stack::shared_instance()->get_sketch()->get_walls())) {
//            math::line_2d w_l(this->start_pos, this->end_pos);
//            if (w_l.point_on_line(w->get_start_point()) && w_l.point_on_line(w->get_end_point())) {
//                command_stack::shared_instance()->get_sketch()->del_wal(w);
//            }
//        }
    this->wall_exist = false;
    command_stack::shared_instance()->get_sketch()->recalculate_rooms();
    return false;
}

add_wall::add_wall(gl3d_wall *w) : add_or_del_wall(w) {
    this->setText("add_wall");
}

void add_wall::undo() {
    add_or_del_wall::del_wall();
    return;
}

void add_wall::redo() {
    add_or_del_wall::add_wall();
    return;
}

del_wall::del_wall(gl3d_wall *w) : add_or_del_wall(w) {
    this->setText("del_wall");
}

void del_wall::undo() {
    add_or_del_wall::add_wall();
    return;
}

void del_wall::redo() {
    add_or_del_wall::del_wall();
    return;
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
    this->auto_call = true;
    this->init_time = QDateTime::currentMSecsSinceEpoch();
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
    if (this->auto_call) {
        this->auto_call = false;
        return;
    }
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
            if (abs(p->init_time - this->init_time) > this->ONE_SECOND) {
                return false;
            }
            this->init_time = p->init_time;
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
    hole * ph = new hole(w, this->center, this->width, this->min_height, this->max_height, w->get_availble_hole_id());
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
    hole * h = new hole(w, this->pa, this->pb, w->get_availble_hole_id());
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

obj_add_or_del::obj_add_or_del(gl3d::object *o) {
    this->obj_id = o->get_id();
    this->obj_file_name = o->get_obj_file_name();
    this->res_id = o->get_res_id();
    this->position = o->get_property()->position;
    this->rotate_mat = o->get_property()->rotate_mat;
    this->s_unit = o->get_property()->scale_unit;
    this->render_code = o->get_render_authority();
    this->control_code = o->get_control_authority();
}

void obj_add_or_del::del_obj() {
    if (command_stack::shared_instance()->get_sketch()->get_objects()->contains(this->obj_id)) {
        gl3d::object * o = (gl3d::object *) command_stack::shared_instance()->get_main_scene()->get_obj(this->obj_id);
        command_stack::shared_instance()->get_main_scene()->delete_obj(this->obj_id);
        delete o;
    }
}

void obj_add_or_del::add_obj() {
    // may be here should async?
//    klm::resource::manager::shared_instance()->perform_async_res_load(this, this->res_id);
    std::string tmp = this->obj_file_name.toStdString();
    this->do_work((void *) &tmp);
}

void obj_add_or_del::do_work(void *object) {
    std::string * name = (std::string *)object;
    gl3d::object *obj = new gl3d::object((char *)name->c_str());
    obj->set_res_id(this->get_obj_res_id());

    // set property
    obj->set_control_authority(this->control_code);
    obj->set_render_authority(this->render_code);
    obj->get_property()->scale_unit = this->s_unit;
    obj->get_property()->position = this->position;
    obj->get_property()->rotate_mat = this->rotate_mat;
    obj->pre_scale();
    obj->merge_meshes();
    obj->recalculate_normals();
    obj->convert_left_hand_to_right_hand();

    gl3d_lock::shared_instance()->render_lock.lock();
    command_stack::shared_instance()->get_sketch()->add_obj(this->obj_id, obj);
    gl3d_lock::shared_instance()->render_lock.unlock();

    return;
}

add_obj::add_obj(gl3d::object *o) : obj_add_or_del(o) {
    this->setText("add_obj");
    this->on_create = true;
}

void add_obj::undo() {
    obj_add_or_del::del_obj();
}

void add_obj::redo() {
    if (this->on_create) {
        this->on_create = false;
    }
    else
        obj_add_or_del::add_obj();
}

del_obj::del_obj(gl3d::object *o) : obj_add_or_del(o) {
    this->setText("del_obj");
    this->on_create = true;
}

void del_obj::undo() {
    obj_add_or_del::add_obj();
}

void del_obj::redo() {
    if (this->on_create)
        this->on_create = false;
    else
        obj_add_or_del::del_obj();
}

set_obj_property::set_obj_property(gl3d::object *o) : obj_add_or_del(o), target(NULL){
    this->setText("set_obj_property");
    this->obj = o;
}

int set_obj_property::id() const {
    return KLM_CMD_ID_SET_FURNITURE;
}

void set_obj_property::undo() {
    if (NULL != this->target)
        delete this->target;
    this->target = new set_obj_property(this->obj);
}

void set_obj_property::redo() {
    this->obj->get_property()->position = this->target->position;
    this->obj->get_property()->rotate_mat = this->target->rotate_mat;
    this->obj->get_property()->scale_unit = this->target->s_unit;
    this->obj->set_render_authority(this->target->render_code);
    this->obj->set_control_authority(this->target->control_code);
}

bool set_obj_property::mergeWith(const QUndoCommand *other) {
    if (other->id() == KLM_CMD_ID_SET_FURNITURE) {
        set_obj_property * sp = (set_obj_property *) other;
        if (this->obj_id == sp->obj_id) {
            // same id then merge
            if (NULL != this->target)
                delete target;
            this->target = new set_obj_property(sp->obj);
            return true;
        }
    }
    return false;
}

set_obj_property::~set_obj_property() {
    if (NULL != this->target)
        delete this->target;
}

add_or_del_door::add_or_del_door(gl3d_door *d) {
    // copy important data
    this->res_id = d->door_model->get_res_id();
    this->start_pt = d->start_pt;
    this->end_pt = d->end_pt;
    this->center_pt = d->center_pt;
    this->height = d->height;
    this->width = d->width;
    this->thickness = d->thickness;
    this->attached_wall_id = d->attached_wall_id;
    this->attached_hole_id = d->attached_hole_id;
    this->pre_translate_mat = d->pre_translate_mat;
    this->pre_scale_mat = d->pre_scale_mat;
    this->trans_mat = d->trans_mat;
    this->rotate_mat = d->rotate_mat;
    this->obj_id = d->get_id();
}

add_or_del_door::~add_or_del_door() {
    // TODO : seems not need
}

void add_or_del_door::add_door() {
    scheme * skt = command_stack::shared_instance()->get_sketch();
    gl3d_wall * w = (gl3d_wall *)skt->get_obj(this->attached_wall_id);

    gl3d_door * d = new gl3d_door(this->res_id);
    d->set_id(skt->find_available_id());
    if ((w == NULL) || (w->get_obj_type() != abstract_object::type_wall)) {
        return;
    }

    if (!d->install_to_wall(w, this->center_pt, this->width, this->height)) {
        delete d;
        return;
    }

    this->obj_id = d->get_id();
    this->attached_hole_id = d->attached_hole_id;
    skt->get_doors()->insert(d);
    skt->get_objects()->insert(d->get_id(), d);
}

void add_or_del_door::del_door() {
    scheme * skt = command_stack::shared_instance()->get_sketch();
    if (skt->get_obj(this->obj_id) != NULL) {
        gl3d_door * d = (gl3d_door *)skt->get_obj(this->obj_id);
        if (d->get_obj_type() == abstract_object::type_door) {
            gl3d_wall * w = (gl3d_wall *)skt->get_obj(d->attached_wall_id);
            w->holes_on_this_wall.remove(d->attached_hole_id);
            skt->get_doors()->remove(d);
            skt->get_objects()->remove(d->get_id());
            delete d;
        }
    }
}

add_door::add_door(gl3d_door *d) : add_or_del_door(d) {
    this->on_create = true;
    this->setText("add_door");
}

void add_door::redo() {
    if (this->on_create)
        this->on_create = false;
    else
        add_or_del_door::add_door();
}

void add_door::undo() {
    add_or_del_door::del_door();
}

del_door::del_door(gl3d_door *d):add_or_del_door(d) {
    this->on_create = true;
    this->setText("del_door");
}

void del_door::redo() {
    if (this->on_create)
        this->on_create = false;
    else
        add_or_del_door::del_door();
}

void del_door::undo() {
    add_or_del_door::add_door();
}

add_or_del_window::add_or_del_window(gl3d_window *w) {
    // copy important data
    this->res_id = w->window_model->get_res_id();
    this->start_pt = w->start_pt;
    this->end_pt = w->end_pt;
    this->center_pt = w->center_pt;
    this->height_max = w->height_max;
    this->height_min = w->height_min;
    this->width = w->width;
    this->thickness = w->thickness;
    this->attached_wall_id = w->attached_wall_id;
    this->attached_hole_id = w->attached_hole_id;
    this->pre_translate_mat = w->pre_translate_mat;
    this->pre_scale_mat = w->pre_scale_mat;
    this->trans_mat = w->trans_mat;
    this->rotate_mat = w->rotate_mat;
    this->obj_id = w->get_id();
}

add_or_del_window::~add_or_del_window() {
    // TODO : seems not need
}

void add_or_del_window::add_window() {
    scheme * skt = command_stack::shared_instance()->get_sketch();
    gl3d_wall * wall = (gl3d_wall *)skt->get_obj(this->attached_wall_id);

    gl3d_window * win = new gl3d_window(this->res_id);
    win->set_id(skt->find_available_id());
    if ((wall == NULL) || (wall->get_obj_type() != abstract_object::type_wall)) {
        return;
    }

    if (!win->install_to_wall(wall, this->center_pt, this->width, this->height_max, this->height_min)) {
        delete win;
        return;
    }

    this->obj_id = win->get_id();
    this->attached_hole_id = win->attached_hole_id;
    skt->get_windows()->insert(win);
    skt->get_objects()->insert(win->get_id(), win);
}

void add_or_del_window::del_window() {
    scheme * skt = command_stack::shared_instance()->get_sketch();
    if (skt->get_obj(this->obj_id) != NULL) {
        gl3d_window * win = (gl3d_window *)skt->get_obj(this->obj_id);
        if (win->get_obj_type() == abstract_object::type_window) {
            gl3d_wall * wall = (gl3d_wall *)skt->get_obj(win->attached_wall_id);
            wall->holes_on_this_wall.remove(win->attached_hole_id);
            skt->get_windows()->remove(win);
            skt->get_objects()->remove(win->get_id());
            delete win;
        }
    }
}

add_window::add_window(gl3d_window *win):add_or_del_window(win) {
    this->on_create = true;
    this->setText("add_window");
}

void add_window::redo() {
    if (this->on_create)
        this->on_create = false;
    else
        add_or_del_window::add_window();
}

void add_window::undo() {
    add_or_del_window::del_window();
}

del_window::del_window(gl3d_window *win):add_or_del_window(win) {
    this->on_create = true;
    this->setText("del_window");
}

void del_window::redo() {
    if (this->on_create)
        this->on_create = false;
    else
        add_or_del_window::del_window();
}

void del_window::undo() {
    add_or_del_window::add_window();
}
