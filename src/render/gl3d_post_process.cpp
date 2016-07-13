#include "kaola_engine/gl3d_post_process.h"

using namespace std;
using namespace gl3d;

static gl3d_post_process_set * one_instance = NULL;

gl3d_post_process_set * gl3d_post_process_set::shared_instance() {
    if (NULL == one_instance) {
        one_instance = new gl3d_post_process_set();
    }

    return one_instance;
}

gl3d_post_process_set::gl3d_post_process_set() {
    this->textures.clear();
    this->processer_set.clear();
}

gl3d_framebuffer * gl3d_post_process_set::process(QVector<string> commands, scene * scene) {
    gl3d_framebuffer * dst = new gl3d_framebuffer(0,
                                                  scene->get_property()->screen_width,
                                                  scene->get_property()->screen_height);
    gl3d_framebuffer * src = new gl3d_framebuffer(0,
                                                  scene->get_property()->screen_width,
                                                  scene->get_property()->screen_height);
    auto it = commands.begin();
    for (; it != commands.end(); it++) {
        if (this->processer_set.contains(*it)) {
            gl3d_post_processer * prcer =
                    this->processer_set.value(*it);
            prcer->process(dst, src, scene);
            // swap dst to src
            gl3d_framebuffer * tmp = dst;
            dst = src;
            src = tmp;
        }
    }

    // now return framebuffer
    delete dst;
    return src;
}

void gl3d_post_process_set::add_post_processer(string name, gl3d_post_processer * p) {
    this->processer_set.insert(name, p);
}
