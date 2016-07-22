#include "utils/gl3d_scale.h"

using namespace gl3d;
static scale * one_instance;

scale::scale() {
    this->init();
}

void scale::init() {
    // clear map
    this->length_unit_to_scale_factor.clear();
    this->platform_to_standard_widget_width.clear();

    // set current global scale
    this->global_scale = 5.0f;

    // set default pre-scale value
    this->length_unit_to_scale_factor.insert(this->mm, 0.001f);
    this->length_unit_to_scale_factor.insert(this->cm, 0.01f);
    this->length_unit_to_scale_factor.insert(this->m, 1.0f);
    this->length_unit_to_scale_factor.insert(this->skybox, 2.0f);
    this->length_unit_to_scale_factor.insert(this->wall, 1.0f);
    this->length_unit_to_scale_factor.insert(this->special, 1.0f);
    // set default platform value
    this->platform_to_standard_widget_width.insert(this->windows, 1024.0f);

    // set current platform
    this->current_platform = this->windows;
}

template <typename T>
T scale::pre_scale_vertex(T &in_value, length_unit unit) {
    return in_value * this->length_unit_to_scale_factor.value(unit);
}

scale * scale::shared_instance() {
    if (NULL == one_instance) {
        one_instance = new scale();
    }

    return one_instance;
}

float scale::get_scale_factor(float widget_width) {
    float ret;
    ret = this->global_scale;
    ret = ret / (widget_width/this->platform_to_standard_widget_width.value(this->current_platform));
    return ret;
}
