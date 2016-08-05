//
//  gl3d_obj_authority.h
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/3/30.
//  Copyright © 2016年 Kent. All rights reserved.
//

#ifndef gl3d_obj_authority_h
#define gl3d_obj_authority_h

// control authority
#define GL3D_OBJ_ENABLE_ALL 0xffffffffffffffffUL
#define GL3D_OBJ_ENABLE_NONE 0x00UL
#define GL3D_OBJ_ENABLE_MOVE (0x01UL << 0)
#define GL3D_OBJ_ENABLE_ROTATE (0x01UL << 1)
#define GL3D_OBJ_ENABLE_CHANGEMTL (0x01UL << 2)
#define GL3D_OBJ_ENABLE_DEL (0x01UL << 3)
#define GL3D_OBJ_ENABLE_PICKING (0x01UL << 4)
#define GL3D_OBJ_ENABLE_CULLING (0x01UL << 5)

// render authority
#define GL3D_SCENE_DRAW_ALL 0xffffffffffffffffUL  // 注意设置参数时不要使用draw all宏
#define GL3D_SCENE_DRAW_NONE 0x00UL
#define GL3D_SCENE_DRAW_NORMAL (0x01UL << 0)
#define GL3D_SCENE_DRAW_SHADOW (0x01UL << 1)
#define GL3D_SCENE_DRAW_IMAGE (0x01UL << 2)
#define GL3D_SCENE_DRAW_GROUND (0x01UL << 3)
#define GL3D_SCENE_DRAW_SKYBOX (0x01UL << 4)
#define GL3D_SCENE_DRAW_RESULT (0x01UL << 5)
#define GL3D_SCENE_DRAW_SPECIAL (0x01UL << 6)
#define GL3D_SCENE_DRAW_WALL (0x01UL << 7)
#define GL3D_SCENE_DRAW_NET (0x01UL << 8)

#endif /* gl3d_obj_authority_h */
