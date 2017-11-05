#include "utils/gl3d_global_param.h"

using namespace gl3d;
static gl3d_global_param * one_instance;

gl3d_global_param * gl3d_global_param::shared_instance() {
    if (NULL == one_instance) {
        one_instance = new gl3d_global_param();
    }

    return one_instance;
}

gl3d_global_param::gl3d_global_param(){
    memset(this, 0, sizeof(gl3d_global_param));
    this->wall_thick = 0.12f;
    this->old_sketch_test = NULL;
    this->password = new QString();
    this->username = new QString();
    this->room_height = 2.8f;
    this->maxCulledObjNum = 1000;
    this->maxInsPerDraw = 200;
    this->drawSpace = false;
    this->drawGeo = false;
}
