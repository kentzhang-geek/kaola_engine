//
//  gl3d_material.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/16.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_material_hpp
#define gl3d_material_hpp

#include <map>
#include <string>
#include <vector>

#include "gl3d_texture.hpp"

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
        
        map<material_type, gl3d_texture *> textures;
        map<material_type, ::glm::vec3> colors;
        
        gl3d_material(aiMaterial * mtls);
        gl3d_material(string file_name);
        ~gl3d_material();
        void use_this(GLuint pro);
        bool is_empty;
    private:
        void init();
    };
}


#endif /* gl3d_material_hpp */
