//
//  gl3d_material.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_material_hpp
#define gl3d_material_hpp

#include <QMap>
#include <string>
#include <QVector>

#include "gl3d_texture.hpp"
#include "gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_image.hpp"
#include "gl3d_out_headers.h"

using namespace std;
using namespace gl3d;

namespace gl3d {
    class gl3d_material {
    public:
        enum material_type {
            ambient = 0,
            diffuse,
            specular
        };
        
        QMap<material_type, gl3d_general_texture *> textures;
        QMap<material_type, ::glm::vec3> colors;
        QMap<material_type, gl3d_image *> type_to_image;
        
        gl3d_material(aiMaterial * mtls, string base_path);
        gl3d_material(string file_name);
        gl3d_material();
        gl3d_material(gl3d_general_texture * gtext);
        ~gl3d_material();
        void use_this(GLuint pro);
        bool is_empty;
        void buffer_data();
        bool data_buffered;

        // build pure color material
        static gl3d_material * from_color(glm::vec3 rgb, int alpha = 255);

    private:
        void init();
    };
}

#endif /* gl3d_material_hpp */
