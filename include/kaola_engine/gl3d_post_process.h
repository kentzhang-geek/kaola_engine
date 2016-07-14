#ifndef GL3D_POST_PROCESS_H
#define GL3D_POST_PROCESS_H

#include <stdio.h>
#include <string>
#include <QVector>
#include <QMap>
#include "glheaders.h"
#include "gl3d_out_headers.h"
#include "gl3d_viewer.h"
#include "gl3d_mesh.h"
#include "gl3d_material.hpp"
#include "utils/gl3d_scale.h"
#include "gl3d_object.h"
#include "gl3d_general_texture.hpp"
#include "gl3d_viewer.h"
#include "shader_manager.hpp"
#include "gl3d_scene.h"
#include "gl3d_framebuffer.hpp"
#include "utils/gl3d_utils.h"

using namespace std;


namespace gl3d {
class gl3d_post_process_set;

class gl3d_post_processer {
public:
    GL3D_UTILS_PROPERTY(parent, gl3d_post_process_set *);
    GL3D_UTILS_PROPERTY(textures, QMap<string, gl3d::gl3d_general_texture *> );
    GL3D_UTILS_PROPERTY(name, string);

    virtual gl3d::gl3d_framebuffer * process(
            gl3d::gl3d_framebuffer * src,
            gl3d::scene * scene) = 0;
    virtual void deel_with_src(gl3d::gl3d_framebuffer * src) = 0;
};

class gl3d_post_process_set {
public:
    static gl3d_post_process_set * shared_instance();
    gl3d::gl3d_framebuffer * process(QVector<string> commands,
                                     scene *scene,
                                     gl3d_framebuffer *src);
    void add_post_processer(string name, gl3d_post_processer * p);

    GL3D_UTILS_PROPERTY(processer_set,  QMap<string, gl3d_post_processer *> );
    GL3D_UTILS_PROPERTY(textures, QMap<string, gl3d::gl3d_general_texture *> );

private:
    gl3d_post_process_set();
};

template <class T>
class gl3d_post_processer_factory {
public:
    gl3d_post_processer_factory(string name){
        gl3d_post_process_set * m = gl3d_post_process_set::shared_instance();
        // create new post processer
        T * ins = new T();
        ins->set_textures(m->get_textures());
        ins->set_parent(m);
        ins->set_name(name);
        // insert post processer to set
        m->add_post_processer(name, ins);
    };
};
}

#define GL3D_GET_POST_PROCESSER(name) (gl3d::gl3d_post_process_set::shared_instance()->get_post_processer(string(#name)))
#define GL3D_ADD_POST_PROCESSER(name) const gl3d::gl3d_post_processer_factory<name> __tag_##post_processer_##name(string(#name));

#endif // GL3D_POST_PROCESS_H
