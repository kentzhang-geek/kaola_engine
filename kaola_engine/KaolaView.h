//
//  KaolaView.h
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/2/23.
//  Copyright © 2016年 Kent. All rights reserved.
//

#import <GLKit/GLKit.h>
#include "gl3d.hpp"

@interface KaolaView : GLKView
@property gl3d::scene * one_scene;

-(void) render;
-(void) prepare_buffer;

@end

#ifndef __KAOLA_VIEW_H
#define __KAOLA_VIEW_H

// 工具宏
#define KAOLA_VIEW_ADD_OBJ(id, obj) ((KaolaView *)self.view).one_scene->add_obj(std::pair<int, gl3d::object *>(id, obj))

#endif