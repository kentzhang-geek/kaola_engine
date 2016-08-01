#include "kaola_engine/mopenglview.h"
#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_render_process.hpp"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_lock.h"

using namespace std;

// 全局OpenGL Functions
QOpenGLFunctions_4_1_Core * gl3d_win_gl_functions = NULL;

void MOpenGLView::do_init() {
    //yananli code
    this->setMouseTracking(true);

    // set OPENGL context
    timer = new QTimer(this);
    timer->start(100);

    // init path KENT TODO : shader目录设置要调整
    this->res_path = GL3D_PATH_SHADER;

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
    float xsss = this->width();
    float ysss = this->height();

    shader_manager * shader_mgr = ::shader_manager::sharedInstance();
    Program * prog;
    auto load_iter = shader_mgr->loaders.begin();
    for (; load_iter != shader_mgr->loaders.end(); load_iter++) {
        string vert = this->res_path + GL3D_PATH_SEPRATOR + load_iter.value()->vertex;
        string frag = this->res_path + GL3D_PATH_SEPRATOR + load_iter.value()->frag;
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

bool need_capture;
void MOpenGLView::paintGL() {
    QGLWidget::paintGL();

    // lock render
    if (!gl3d_lock::shared_instance()->render_lock.tryLock()) {
        return;
    }

    // 设置场景
    GL3D_GET_CURRENT_RENDER_PROCESS()->add_user_object("scene", this->main_scene);

    // 预渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->pre_render();

    // KENT HINT : 用这个接口重新绑定默认渲染目标等等
    // bind drawable KENT TODO : 应该给FBO加一个封装，由FBO句柄生成
    gl3d_global_param::shared_instance()->framebuffer = this->context()->contextHandle()->defaultFramebufferObject();
    glBindFramebuffer(GL_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
    glViewport(0, 0,
               this->main_scene->get_width(),
               this->main_scene->get_height());

    GL3D_GET_CURRENT_RENDER_PROCESS()->render();

    // 后渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->after_render();

    // release default fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unlock render
    gl3d_lock::shared_instance()->render_lock.unlock();

}

void MOpenGLView::initializeGL() {
    QGLWidget::initializeGL();
    // here gat parent widget size and set to sel

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
    QGLWidget::keyPressEvent(event);
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
    int wid = this->width();
    int hit = this->height();

    this->wall_temp_id = 23333;
}

void MOpenGLView::resizeGL(int width, int height) {
    QGLWidget::resizeGL(width, height);
    // here gat parent widget size and set to sel
//    this->setGeometry(this->parentWidget()->geometry());
    this->main_scene->set_height((float)height);
    this->main_scene->set_width((float)width);
    this->main_scene->watcher->set_width((float)width);
    this->main_scene->watcher->set_height((float)height);
    this->main_scene->watcher->calculate_mat();
    gl3d::gl3d_global_param::shared_instance()->canvas_height = (float) height;
    gl3d::gl3d_global_param::shared_instance()->canvas_width = (float) width;
}


//yananli codes ----------------------------------------------------------------------

//滚轮滑动事件
void MOpenGLView::wheelEvent(QWheelEvent *event) {
    //滚动的角度，*8就是鼠标滚动的距离
    int numDegrees = event->delta() / 8;

    //滚动的步数，*15就是鼠标滚动的角度
    int numSteps = numDegrees / 15;

    auto tmp_viewer = this->main_scene->watcher;
    if (tmp_viewer->get_view_mode() == tmp_viewer->top_view) {
        tmp_viewer->set_top_view_size(tmp_viewer->get_top_view_size() - (float)numSteps);
    }

    event->accept();      //接收该事件
}

extern bool need_capture;

//opengl执行画墙
void MOpenGLView::openglDrawWall(const int x, const int y) {
    this->new_wall = NULL;
    this->wall_temp_id = this->wall_temp_id + 1;
    gl3d::scene * vr = this->main_scene;
    // set wall
    glm::vec2 pick;
    vr->coord_ground(glm::vec2(((float)x) / this->width(),
                               ((float)y) / this->height()),
                     pick, 0.0);
    this->new_wall = new gl3d::gl3d_wall(pick, pick, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
    this->main_scene->add_obj(QPair<int , object *>(this->wall_temp_id, this->new_wall));
}


//鼠标按下事件
void MOpenGLView::mousePressEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //    QString str = "("+QString::number(event->x())+","+QString::number(event->y())+")";
    //    cout << "down: " << event->x() << ", " << event->y() << endl;

    //左键按下事件
    if(event->button() == Qt::LeftButton) {
        //拾取并新建选项框
        if(now_state == gl3d::gl3d_global_param::normal) {
            need_capture = true;
            pickUpObjID = this->main_scene->get_object_id_by_coordination(event->x(), event->y());
            if(pickUpObjID > 0) {
                cout << "click objid: " << pickUpObjID << endl;
                puDig = new PickupDig(this->parentWidget(), event->x(), event->y(), pickUpObjID, this->main_scene);
                puDig->show();
                gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::pickup;
            }
        }

        //取消拾取
        if(now_state == gl3d::gl3d_global_param::pickup) {
            puDig->close();
            delete puDig;
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
        }

        this->tmp_point_x = event->x();
        this->tmp_point_y = event->y();

        //画墙结束
        if(now_state == gl3d::gl3d_global_param::drawwallend) {
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        }

        //画墙中节点结束
        if(now_state == gl3d::gl3d_global_param::drawwalling) {
            this->openglDrawWall(event->x(), event->y());
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwalling;
        }

        //画墙节点开始
        if(now_state == gl3d::gl3d_global_param::drawwall) {
            this->openglDrawWall(event->x(), event->y());
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwalling;
//            cout << "left down: " << event->x() << ", " << event->y() << endl;
        }


        //右键按下事件
    } else if(event->button() == Qt::RightButton) {
//        cout << "right down: " << event->x() << ", " << event->y() << endl;
        //点击右键-取消画墙状态
        if(now_state == gl3d::gl3d_global_param::drawwall) {
            drawhomewin::on_draw_clear();
        }
        //点击右键-终止画墙连接
        if(now_state == gl3d::gl3d_global_param::drawwalling) {
            this->main_scene->delete_obj(this->wall_temp_id);
            delete this->new_wall;
            this->new_wall = NULL;
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        }
    } else if(event->button() == Qt::MidButton) {
//        cout << "centre down: " << event->x() << ", " << event->y() << endl;
    }
}

//鼠标移动事件
void MOpenGLView::mouseMoveEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //画墙中
    if(now_state == gl3d::gl3d_global_param::drawwalling) {
        setCursor(Qt::CrossCursor);
        glm::vec2 pick;
        gl3d::scene * vr = this->main_scene;
        vr->coord_ground(glm::vec2(((float)event->x()) / this->width(),
                                   ((float)event->y()) / this->height()),
                         pick, 0.0);
        this->new_wall->set_end_point(pick);
        this->new_wall->calculate_mesh();
//        cout << "move: " << event->x() << ", " << event->y() << endl;
    }

    if(event->buttons()&Qt::LeftButton) {
        cout << "left move: " << event->x() << ", " << event->y() << endl;
        auto tmp_viewer = this->main_scene->watcher;
        if (tmp_viewer->get_view_mode() == tmp_viewer->top_view) {
//            cout << float(event->x() - this->tmp_point_x) / 100 << endl;
            tmp_viewer->change_position(glm::vec3(-float(event->x() - this->tmp_point_x) / 50, float(event->y() - this->tmp_point_y) / 50, 0.0));
            this->tmp_point_x = event->x();
            this->tmp_point_y = event->y();
            setCursor(Qt::SizeAllCursor);
        }
    } else if(event->buttons()&Qt::LeftButton) {
        //        cout << "right move: " << event->x() << ", " << event->y() << endl;
    } else if(event->buttons()&Qt::LeftButton) {
        //        cout << "centre move: " << event->x() << ", " << event->y() << endl;
    }
}

//鼠标松开事件
void MOpenGLView::mouseReleaseEvent(QMouseEvent *event) {
    //    cout << "loosen: " << event->x() << ", " << event->y() << endl;
    if(gl3d::gl3d_global_param::shared_instance()->current_work_state != gl3d::gl3d_global_param::drawwall) {
        setCursor(Qt::ArrowCursor);
    }
}
