//
//  gl3d_texture.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_texture_hpp
#define gl3d_texture_hpp

#include <stdio.h>
#include <string>
#include "kaola_engine/gl3d_image.hpp"
#include "gl3d_general_texture.hpp"
#include <QVector>
#include <QMap>

namespace gl3d {
    class gl3d_texture : public gl3d_general_texture{
    public:
        std::string texture_name;
        gl3d_image * img;
        gl3d_texture(char * filename);
        gl3d_texture(gl3d_image * img);
        ~gl3d_texture();
        static void set_parami(bool repeat);
    private:
        void init();
    };
}

#endif /* gl3d_texture_hpp */
