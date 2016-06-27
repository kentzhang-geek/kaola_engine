//
//  gl3d_image.m
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include "gl3d_image.hpp"
#include <string.h>
#include "log.h"

using namespace std;
using namespace gl3d;

static inline unsigned long long resize_pixel(unsigned long long x) {
    unsigned long long ret = 0;
    for (int i = 0; i < (sizeof(x) * 8); i++) {
        if ((1UL << i) & (x)) {
            ret = (1UL << i);
        }
    }
    return ret;
}

void gl3d_image::init() {
    memset(this, 0, sizeof(gl3d_image));
}

gl3d_image::~gl3d_image() {
    free(this->data);
}

#include <QTextEdit>

extern QTextEdit * qtout;

gl3d_image::gl3d_image(char * f) {
    this->init();
    
    QString filename(f);
    QImage img(filename);

    QString log("Load Image : ");
    log.append(filename);
    qtout->setText(qtout->toPlainText() + log + "\n");

    if (img.isNull()) {
        log_c("Failed to load image %s", f);
        throw std::runtime_error("Load image");
        return;
    }
    
    this->width = img.width();
    this->height = img.height();
    
    // 调整下大小
    this->width = resize_pixel(this->width);
    this->height = resize_pixel(this->height);

    img = img.scaled(QSize(this->width, this->height), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    img = img.convertToFormat(QImage::Format_RGBA8888);
    if (QImage::Format_RGBA8888 != img.format()) {
        log_c("Image %s format is not rgba8888", f);
        throw std::runtime_error("Load Image format");
        return ;
    }

    this->data = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
            
    // Color space
    this->color_property = gl3d_image::color_space::CS_RGBA8888;

    // copy data
    memcpy(this->data, img.bits(), width*height*4);
}
