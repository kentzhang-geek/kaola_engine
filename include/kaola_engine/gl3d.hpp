//
//  gl3d.hpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/2/17.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_hpp
#define gl3d_hpp

/**
 *  @author Kent, 16-02-17 20:02:45
 *
 *  System Headers
 */
#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "kaola_engine/Program.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_render_process.hpp"

// OpenGL in windows should use GLEW
#include "kaola_engine/glheaders.h"

// kaolaengine components
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_object.h"
#include "kaola_engine/gl3d_viewer.h"
#include "kaola_engine/gl3d_scene.h"

// 工具宏
#define SHADER_LOCATE(x) [[[[NSBundle mainBundle] resourcePath]stringByAppendingString:[@"/Frameworks/kaola_engine.framework/" stringByAppendingString:[NSString stringWithUTF8String:(x)]]] UTF8String]

#define SHADER_SRC(x) [[NSString stringWithContentsOfFile: \
[NSString stringWithUTF8String : SHADER_LOCATE(x)] encoding: NSUTF8StringEncoding error:nil] UTF8String]

// 全局的沙盒位置
extern string gl3d_sandbox_path;
#define GL3D_INIT_SANDBOX_PATH(path) { \
gl3d_sandbox_path = std::string(path); }

#endif /* gl3d_hpp */
