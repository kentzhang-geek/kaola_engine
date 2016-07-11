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
    // set default scale value
    this->length_unit_to_scale_factor.insert(this->mm, 0.005f);
    this->length_unit_to_scale_factor.insert(this->cm, 0.05f);
    this->length_unit_to_scale_factor.insert(this->m, 5.0f);
    this->length_unit_to_scale_factor.insert(this->skybox, 10.0f);
    // set default platform value
    this->platform_to_standard_widget_width.insert(this->windows, 1024.0f);

    // set current platform
    this->current_platform = this->windows;
}

scale * scale::shared_instance() {
    if (NULL == one_instance) {
        one_instance = new scale();
    }

    return one_instance;
}

float scale::get_scale_factor(length_unit s, float widget_width) {
    float ret;
    ret = this->length_unit_to_scale_factor.value(s);
    ret = ret / (widget_width/this->platform_to_standard_widget_width.value(this->current_platform));
    return ret;
}
