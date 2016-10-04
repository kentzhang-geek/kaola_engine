#ifndef STYLE_PACKAGE_H
#define STYLE_PACKAGE_H


#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "kaola_engine/Program.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_render_process.hpp"

// OpenGL in windows should use GLEW
#include "kaola_engine/glheaders.h"

// kaolaengine components
#include "utils/gl3d_utils.h"
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_general_light_source.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_object.h"
#include "kaola_engine/gl3d_scene.h"
#include "utils/gl3d_math.h"
#include "utils/gl3d_global_param.h"

namespace klm {
    namespace design {
        // define common interface
        class abstract_style_package {
            virtual QString get_style_name() = 0;
            virtual bool read_from_scheme(scheme * sketch) = 0;
            virtual int apply_style(scheme * sketch) = 0;
        };

        class common_style_package : public utils::noncopyable, public abstract_style_package {
        public:
            common_style_package();
            virtual ~common_style_package();

            QString get_style_name();
            bool read_from_scheme(scheme * sketch);
            int apply_style(scheme * sketch);

            // save and load
            bool save_to_xml(pugi::xml_node & node);
            bool load_from_xml(pugi::xml_node node);

            // name and resource map
            QString style_name;
            QMap<std::string, std::string> room_name_to_wall_res;
            QMap<std::string, std::string> room_name_to_ground_res;
        };
    }
}


#endif // STYLE_PACKAGE_H
