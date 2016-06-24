//
//  kaola_engine.h
//  kaola_engine
//
//  Created by Kent Zhang on 16/2/17.
//  Copyright © 2016年 Kent. All rights reserved.
//

// glms
#include "glm/glm.hpp"

// engine
#include "mopenglview.h"
#include "Program.hpp"
#include "gl3d.hpp"
#include "Shader.hpp"

#define PATHOFFILE(x)  [[[[NSBundle mainBundle] resourcePath]stringByAppendingString:@"/"] stringByAppendingString:x]
#define C_PATHOFFILE(x)  [[[[[NSBundle mainBundle] resourcePath]stringByAppendingString:@"/"] stringByAppendingString:x] cStringUsingEncoding:NSUTF8StringEncoding]

// In this header, you should import all the public headers of your framework using statements like #import <kaola_engine/PublicHeader.h>


