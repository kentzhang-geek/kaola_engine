//
//  gl3d_image.m
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/15.
//  Copyright © 2016年 Kent. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <Foundation/Foundation.h>
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
    bzero(this, sizeof(gl3d_image));
}

gl3d_image::~gl3d_image() {
    free(this->data);
}

gl3d_image::gl3d_image(char * f) {
    this->init();
    
    NSString * fileName = [NSString stringWithUTF8String:f];
    CGImageRef spriteImage = [UIImage imageWithContentsOfFile:fileName].CGImage;
    if (!spriteImage) {
        log_c("Failed to load image %s", f);
        throw std::runtime_error("Load image");
        return;
    }
    
    this->width = CGImageGetWidth(spriteImage);
    this->height = CGImageGetHeight(spriteImage);
    
    // 调整下大小
    this->width = resize_pixel(this->width);
    this->height = resize_pixel(this->height);

    this->data = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
    
    CGContextRef spriteContext =
    CGBitmapContextCreate(this->data, width, height, 8, width*4,
                          CGImageGetColorSpace(spriteImage),
                          kCGImageAlphaPremultipliedLast);
    
    // KENT TODO : 最好做下图片的resize，如果长宽不是2的N次方，则无法使用repeat贴图！
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);
    
    CGContextRelease(spriteContext);
        
    // Color space
    this->color_property = gl3d_image::color_space::CS_RGBA8888;
}