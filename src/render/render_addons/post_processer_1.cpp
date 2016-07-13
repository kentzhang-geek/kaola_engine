#include "kaola_engine/gl3d_post_process.h"

class testpp : public gl3d_post_processer {
public:
    void process(gl3d_framebuffer *dest, gl3d_framebuffer *src, scene *scene) {
//        cout << "has complete " << dest->is_complete() << src->is_complete() << " and scene " << scene << endl;
        return;
    }
};
GL3D_ADD_POST_PROCESSER(testpp);
