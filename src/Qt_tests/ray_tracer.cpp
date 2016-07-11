#include "kaola_engine/gl3d_render_process.hpp"
#include "ray_tracer.h"

class ray_tracer : public gl3d::render_process {
public:
    void render() {};
};
GL3D_ADD_RENDER_PROCESS(ray_tracer);
