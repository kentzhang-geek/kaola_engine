#ifndef DESIGN_SCHEME_H
#define DESIGN_SCHEME_H

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include "kaola_engine/gl3d_abstract_object.h"

namespace klm {
    namespace design {
        class room {

        };
        class scheme : public gl3d::abstract_object {
        public:
            // get mesh for scheme
            bool is_data_changed() ;
            bool is_visible() ;
            glm::mat4 get_translation_mat() ;
            glm::mat4 get_rotation_mat() ;
            glm::mat4 get_scale_mat() ;
            void get_abstract_meshes(QVector<gl3d::mesh *> & ms) ;
            void clear_abstract_meshes(QVector<gl3d::mesh *> & ms) ;
            void get_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) ;
            void clear_abstract_mtls(QMap<unsigned int, gl3d_material *> & mt) ;
            void set_translation_mat(const glm::mat4 & trans) ;
        };
    }
}

#endif // DESIGN_SCHEME_H
