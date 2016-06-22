//
//  kaola_engine.h
//  kaola_engine
//
//  Created by Kent Zhang on 16/2/17.
//  Copyright © 2016年 Kent. All rights reserved.
//

#import <UIKit/UIKit.h>

// glms
#import "glm/glm.hpp"

// engine
#import "KaolaView.h"
#include "gl3d.hpp"
#include "gl3d_teapot_drawer.hpp"
#include "obj_loader.hpp"
#include "Program.hpp"
#include "Shader.hpp"

#define PATHOFFILE(x)  [[[[NSBundle mainBundle] resourcePath]stringByAppendingString:@"/"] stringByAppendingString:x]
#define C_PATHOFFILE(x)  [[[[[NSBundle mainBundle] resourcePath]stringByAppendingString:@"/"] stringByAppendingString:x] cStringUsingEncoding:NSUTF8StringEncoding]

//! Project version number for kaola_engine.
FOUNDATION_EXPORT double kaola_engineVersionNumber;

//! Project version string for kaola_engine.
FOUNDATION_EXPORT const unsigned char kaola_engineVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <kaola_engine/PublicHeader.h>


