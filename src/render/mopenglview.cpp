#include "kaola_engine/mopenglview.h"
#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_render_process.hpp"
#include "utils/gl3d_global_param.h"

using namespace std;

// 全局OpenGL Functions
QOpenGLFunctions_4_1_Core * gl3d_win_gl_functions = NULL;

void MOpenGLView::do_init() {
    // set OPENGL context
    timer = new QTimer(this);
    timer->start(100);

    // init path KENT TODO : shader目录设置要调整
    this->res_path = "C:\\Users\\Administrator\\Desktop\\Qt_Projects\\qt_opengl_engine\\shaders";

    this->create_scene();
    GL3D_SET_CURRENT_RENDER_PROCESS(normal, this->main_scene);

    this->main_scene->prepare_buffer();
    this->main_scene->gen_shadow_texture();

    // 一行代码用glkit设置好深度测试
    // 一行代码用glkit设置好模板缓冲区
}

#define MAX_FILE_SIZE 10000
void MOpenGLView::create_scene() {
    this->main_scene = new gl3d::scene( this->height(), this->width() );
    gl3d::gl3d_global_param::shared_instance()->canvas_height = this->height();
    gl3d::gl3d_global_param::shared_instance()->canvas_width = this->width();
    //    this->main_scene->width = this->width();
    //    this->main_scene->height = this->height();

    shader_manager * shader_mgr = ::shader_manager::sharedInstance();
    Program * prog;
    auto load_iter = shader_mgr->loaders.begin();
    for (; load_iter != shader_mgr->loaders.end(); load_iter++) {
        string vert = this->res_path + "\\" + load_iter.value()->vertex;
        string frag = this->res_path + "\\" + load_iter.value()->frag;
        cout << "Load Vertex Shader " << vert << endl;
        cout << "Load Fragment Shader " << frag << endl;
#if 0
        for (i = 0; i < vert.length; i++) {
            vert_src[i] = ~vert_src[i];
        }
        for (i = 0; i < frag.length; i++) {
            frag_src[i] = ~frag_src[i];
        }
#endif
        // read vert
        QFile res_v(QString(vert.c_str()));
        if (!res_v.open(QFile::ReadOnly)) {
            cout << res_v.errorString().toStdString() << endl;
            throw std::runtime_error("Load shader " + vert + res_v.errorString().toStdString());
        }
        auto tmp_v = res_v.readAll();
        string tmp_vert((char *)tmp_v.data(), res_v.size());

        // read frag
        QFile res_f(QString(frag.c_str()));
        if (!res_f.open(QFile::ReadOnly)) {
            cout << res_f.errorString().toStdString() << endl;
            throw std::runtime_error("Load shader " + vert + res_f.errorString().toStdString());
        }
        auto tmp_f = res_f.readAll();
        string tmp_frag((char *)tmp_f.data(), res_f.size());

        // create program
        prog = new Program(tmp_vert, tmp_frag);
        shader_mgr->shaders.insert(load_iter.key(), prog);
    }

    return ;
}

MOpenGLView::MOpenGLView() : QGLWidget()
{
    throw std::invalid_argument("should not create openglview without parent widget");
}

void MOpenGLView::paintGL() {
    // 设置场景
    GL3D_GET_CURRENT_RENDER_PROCESS()->add_user_object("scene", this->main_scene);

    // 预渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->pre_render();

    // KENT HINT : 用这个接口重新绑定默认渲染目标等等
    // bind drawable KENT TODO : 应该给FBO加一个封装，由FBO句柄生成
    gl3d_global_param::shared_instance()->framebuffer = this->context()->contextHandle()->defaultFramebufferObject();
    glBindFramebuffer(GL_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
    // KENT TODO : 这里 为啥要乘以2？
    //    cout << "fbo is " << this->context()->contextHandle()->defaultFramebufferObject() << endl;
    glViewport(0, 0, this->width() * 2, this->height() * 2);

    GL3D_GET_CURRENT_RENDER_PROCESS()->render();

    // 后渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->after_render();
}

void MOpenGLView::initializeGL() {
    QGLWidget::initializeGL();

    this->initializeOpenGLFunctions();
    gl3d_win_gl_functions = this;

    const GLubyte* OpenGLVersion = glGetString(GL_VERSION);
    string glv((char *)OpenGLVersion);
    cout << "OpenGL " << glv << endl;

    this->do_init();

    this->setFocusPolicy(Qt::StrongFocus);
    this->keyTimer = new QTimer();
    this->keyTimer->start(100);

    this->connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    this->connect(keyTimer, SIGNAL(timeout()), this, SLOT(view_change()));
}

void MOpenGLView::view_change() {
    if (this->key_press == 'w') {
        this->main_scene->watcher->go_raise(2.0);
    }
    if (this->key_press == 'a') {
        this->main_scene->watcher->go_rotate(-2.0);
    }
    if (this->key_press == 's') {
        this->main_scene->watcher->go_raise(-2.0);
    }
    if (this->key_press == 'd') {
        this->main_scene->watcher->go_rotate(2.0);
    }
    if (this->key_press == 'j') {
        this->main_scene->watcher->change_position(glm::vec3(-1.0, 0.0, 0.0));
    }
    if (this->key_press == 'k') {
        this->main_scene->watcher->change_position(glm::vec3(0.0, -1.0, 0.0));
    }
    if (this->key_press == 'l') {
        this->main_scene->watcher->change_position(glm::vec3(1.0, 0.0, 0.0));
    }
    if (this->key_press == 'i') {
        this->main_scene->watcher->change_position(glm::vec3(0.0, 1.0, 0.0));
    }

    this->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));
    this->key_press = 0;
    GL3D_GET_CURRENT_RENDER_PROCESS()->invoke();
}

void MOpenGLView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
        this->key_press = 'w';
    if (event->key() == Qt::Key_A)
        this->key_press = 'a';
    if (event->key() == Qt::Key_S)
        this->key_press = 's';
    if (event->key() == Qt::Key_D)
        this->key_press = 'd';
    if (event->key() == Qt::Key_K)
        this->key_press = 'k';
    if (event->key() == Qt::Key_J)
        this->key_press = 'j';
    if (event->key() == Qt::Key_L)
        this->key_press = 'l';
    if (event->key() == Qt::Key_I)
        this->key_press = 'i';
}

MOpenGLView::MOpenGLView(QWidget *x) : QGLWidget(x) {
    auto f = this->format();
    f.setVersion(4, 1);    // opengl 2.0 for opengles 2.0 compatible
    f.setProfile(f.CoreProfile);
    this->setFormat(f);
}

void MOpenGLView::wheelEvent(QWheelEvent *event) {
    //滚动的角度，*8就是鼠标滚动的距离
    int numDegrees = event->delta() / 8;
    //滚动的步数，*15就是鼠标滚动的角度
    int numSteps = numDegrees / 15;

    cout << "Roller rolling Angle: " << numSteps << endl;

//    if (event->orientation() == Qt::Horizontal) {
//        scrollHorizontally(numSteps);       //水平滚动
//    } else {
//        scrollVertically(numSteps);       //垂直滚动
//    }
    event->accept();      //接收该事件
}


