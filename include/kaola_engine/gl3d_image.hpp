//
//  gl3d_image.h
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_image_h
#define gl3d_image_h

#include <string>
#include <QtCore>
#include <QtGui>
#include <QImage>
#include <string>
#include "gl3d_out_headers.h"

namespace gl3d {
    class gl3d_image {
    public:
        unsigned int size;
        unsigned int width;
        unsigned int height;
        std::string name;
        enum color_space {
            CS_RGBA8888 = 0,
        } color_property;
        GLubyte * data;
        gl3d_image(char * filename);
        ~gl3d_image();
    private:
        void load_jpg(char * filename);
        void load_tga(char * filename);
        void init();
    };
}

#endif /* gl3d_image_h */
