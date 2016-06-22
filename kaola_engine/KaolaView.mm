//
//  KaolaView.m
//  kaola_engine_demo
//
//  Created by Kent Zhang on 16/2/23.
//  Copyright © 2016年 Kent. All rights reserved.
//

#import "KaolaView.h"
#include "gl3d_render_process.hpp"

#include "gl3d_teapot_drawer.hpp"
gl3d::gl_teapot_drawer * this_drawer;

@interface KaolaView ()
{
    CAEAGLLayer* _eaglLayer;
}
@property(nonatomic) GLfloat angle;
@property(nonatomic) GLfloat width;
@property(nonatomic) GLfloat height;
@property(nonatomic) GLuint framebuffer;
@property(nonatomic) GLuint colorRenderbuffer;
+(std::string) resourcePath:(const std::string&) fileName;
-(void) create_scene;
-(void) setupDepthBuffer;
-(BOOL) doInit;
-(void)setupLayer;
- (void)destoryBuffer:(GLuint *)buffer;
- (void)destoryBuffers;
@end

@implementation KaolaView
{
    gl3d::Program * pro;
    GLuint _depthRenderBuffer;
}
@synthesize one_scene, width, height, framebuffer, colorRenderbuffer;

- (void)setupLayer {
    _eaglLayer = (CAEAGLLayer*) self.layer;
    
    // Make CALayer visibale
    _eaglLayer.opaque = YES;
    
    // Set drawable properties
    _eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                     [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

-(void) create_scene {
    // create scene
    CGFloat scale = UIScreen.mainScreen.scale;
    int i = 0;

// 控制模拟器或者真机
#if 1
    // 模拟器横屏
    self.width = [[UIScreen mainScreen] bounds].size.width * scale;
    self.height = [[UIScreen mainScreen] bounds].size.height * scale;
#else
    // 真机横屏
    self.height = [[UIScreen mainScreen] bounds].size.width * scale;
    self.width = [[UIScreen mainScreen] bounds].size.height * scale;
#endif
    
    self.one_scene = new gl3d::scene
    ((GLfloat)height,
     width);

    // 初始化shader
    shader_manager * shader_mgr = ::shader_manager::sharedInstance();
    Program * prog;
    auto load_iter = shader_mgr->loaders.begin();
    for (; load_iter != shader_mgr->loaders.end(); load_iter++) {
        cout << "Load Vertex Shader " << SHADER_LOCATE((*load_iter).second->vertex) << endl;
        cout << "Load Fragment Shader " << SHADER_LOCATE((*load_iter).second->frag) << endl;
        NSMutableData * vert = [NSMutableData dataWithContentsOfFile: [NSString stringWithUTF8String:SHADER_LOCATE((*load_iter).second->vertex)]];
        NSMutableData * frag = [NSMutableData dataWithContentsOfFile: [NSString stringWithUTF8String:SHADER_LOCATE((*load_iter).second->frag)]];
        char * vert_src = (char *)[vert mutableBytes];
        char * frag_src = (char *)[frag mutableBytes];
#if 0
        for (i = 0; i < vert.length; i++) {
            vert_src[i] = ~vert_src[i];
        }
        for (i = 0; i < frag.length; i++) {
            frag_src[i] = ~frag_src[i];
        }
#endif
        NSMutableString * tmp_vert = [[NSMutableString alloc] initWithData:vert encoding:NSUTF8StringEncoding];
        NSMutableString * tmp_frag = [[NSMutableString alloc] initWithData:frag encoding:NSUTF8StringEncoding];
        prog = new Program(string(tmp_vert.UTF8String), string(tmp_frag.UTF8String));
        shader_mgr->shaders.insert(std::pair<string, Program *>((*load_iter).first, prog));
    }

    return ;
}

+(std::string) resourcePath:(const std::string&) fileName {
    NSString* fname = [NSString stringWithCString:fileName.c_str() encoding:NSUTF8StringEncoding];
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:fname];
    return std::string([path cStringUsingEncoding:NSUTF8StringEncoding]);
}

- (void)destoryBuffer:(GLuint *)buffer
{
    if (buffer && *buffer != 0) {
        glDeleteRenderbuffers(1, buffer);
        *buffer = 0;
    }
}

- (void)destoryBuffers
{
    [self destoryBuffer: &_depthRenderBuffer];
    [self destoryBuffer: &colorRenderbuffer];
    [self destoryBuffer: &framebuffer];
}

#if 0
-(void)layoutSubviews {
    [EAGLContext setCurrentContext:self.context];
    glUseProgram(GL3D_GET_SHADER("default")->getProgramID());
    
    [self destoryBuffers];
    
    [self setupDepthBuffer];

    [self render];
}
#endif

- (void)setupDepthBuffer
#if 1
{
    // fb
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // rb
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        log_c("Framebuffer status: %x", (int)status);
        return ;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif
#if 0
{
    // Setup frame buffer
    //
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Setup color render buffer
    //
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
    
    // Setup depth render buffer
    //
    int __width, __height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &__width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &__height);
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    // Attach color render buffer and depth render buffer to frameBuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, colorRenderbuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, _depthRenderBuffer);
    
    // Set color render buffer as current render buffer
    //
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"failed to make complete framebuffer object %p", status);
    }
}
#endif
#if 0
{
    // frame buffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // color buffer
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, self.width, self.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

    // depth buffer
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, self.width, self.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"failed to make complete framebuffer object %p", status);
    }
}
#endif

-(BOOL) doInit {
//    // clean up
//    [self destoryBuffers];
//    if (self.context && [EAGLContext currentContext] == self.context)
//    [EAGLContext setCurrentContext:nil];
//    self.context = nil;
//
//    // set up layer
//    [self setupLayer];
//
    // set OPENGL context
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:self.context];
    if(!self.context){
        [NSException raise:@"Failed to create ES Context" format:@""];
    }
    
    [self create_scene];
    GL3D_SET_CURRENT_RENDER_PROCESS(normal);
    
    one_scene->prepare_buffer();
    one_scene->gen_shadow_texture();

    // 一行代码用glkit设置好深度测试
    [self setDrawableDepthFormat:GLKViewDrawableDepthFormat16];
    // 一行代码用glkit设置好模板缓冲区
    [self setDrawableStencilFormat:GLKViewDrawableStencilFormat8];
    
    return TRUE;
}

-(id)initWithCoder:(NSCoder *)aDecoder {
    id view = [super initWithCoder:aDecoder];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [self doInit];
    });
    return view;
}

-(id)initWithFrame:(CGRect)frame {
    id view = [super initWithFrame:frame];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [self doInit];
    });
    return view;
}

-(id)initWithFrame:(CGRect)frame context:(EAGLContext *)ctx {
    id view = [super initWithFrame:frame context:ctx];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [self doInit];
    });
    return view;
}

-(void)render {
    // 设置场景
    GL3D_GET_CURRENT_RENDER_PROCESS()->add_user_object("scene", one_scene);
    
    // 预渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->pre_render();
    
    // KENT HINT : 用这个接口重新绑定默认渲染目标等等
    [self bindDrawable];
    GL3D_GET_CURRENT_RENDER_PROCESS()->render();

    // 后渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->after_render();
}

-(void) prepare_buffer {
    one_scene->prepare_buffer();
}

@end
