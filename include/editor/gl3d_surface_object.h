#ifndef GL3D_SURFACE_OBJECT_H
#define GL3D_SURFACE_OBJECT_H

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "editor/klm_surface.h"
#include "utils/gl3d_path_config.h"

using namespace std;
using namespace klm;

namespace gl3d {
class surface_object : public gl3d::object {
public:
    surface_object(klm::Surface * sfc);
    void iter_surface(klm::Surface * sfc);
private :
    int tmp_idx;
};
}

#endif // GL3D_SURFACE_OBJECT_H
