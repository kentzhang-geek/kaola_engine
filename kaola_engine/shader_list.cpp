//
//  shader_list.cpp
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/11.
//  Copyright © 2016年 Kent. All rights reserved.
//

#include <stdio.h>
#include "shader_manager.hpp"

using namespace gl3d;
using namespace std;

GL3D_LOAD_SHADER(default, default.vdata, default.fdata);

GL3D_LOAD_SHADER(vector_light, vector_light.vdata, vector_light.fdata);

GL3D_LOAD_SHADER(vector_light_toon, vector_light_toon.vdata, vector_light_toon.fdata);


GL3D_LOAD_SHADER(multiple_text, multiple_text.vdata, multiple_text.fdata);

GL3D_LOAD_SHADER(multiple_text_vector, multiple_text_vector.vdata, multiple_text_vector.fdata);

GL3D_LOAD_SHADER(picking_mask, picking_mask.vdata, picking_mask.fdata);

GL3D_LOAD_SHADER(shadow_mask, shadow_mask.vdata, shadow_mask.fdata);

GL3D_LOAD_SHADER(multiple_text_vector_shadow, multiple_text_vector_shadow.vdata, multiple_text_vector_shadow.fdata);

GL3D_LOAD_SHADER(image, image.vdata, image.fdata);

GL3D_LOAD_SHADER(skybox, skybox.vdata, skybox.fdata);

GL3D_LOAD_SHADER(dm, dm.vdata, dm.fdata);

GL3D_LOAD_SHADER(dm2, dm2.vdata, dm2.fdata);
