#include "kaola_engine/gl3d_post_process.h"

class copy_only : public gl3d_post_processer {
public:
    gl3d_general_texture * process(gl3d_general_texture *src, scene *scene) {
        return src;
    }
    void deel_with_src(gl3d_general_texture * src) {};
};
GL3D_ADD_POST_PROCESSER(copy_only);
