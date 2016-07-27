#include "utils/gl3d_lock.h"

using namespace gl3d;

static gl3d_lock * this_lock = NULL;

gl3d_lock::gl3d_lock() {
    this_lock = this;
}

gl3d_lock * gl3d_lock::shared_instance() {
    if (NULL == this_lock) {
        this_lock = new gl3d_lock();
    }

    return this_lock;
}
