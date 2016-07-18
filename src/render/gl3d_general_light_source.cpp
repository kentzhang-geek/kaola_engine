#include "kaola_engine/gl3d_general_light_source.h"

using namespace gl3d;

general_light_source::general_light_source() {
    this->is_attached_to_object = false;
    this->light_type = this->point_light;
    this->attached_object_id = -1;
}

void general_light_source::attach_to_object(int id) {
    this->is_attached_to_object = true;
    this->attached_object_id = id;
}

void general_light_source::disattach_object() {
    this->is_attached_to_object = false;
    this->attached_object_id = -1;
}
