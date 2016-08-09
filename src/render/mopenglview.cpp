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
    this->new_wall = NULL;
    is_drawwall_start_point = false;
    is_drawwall_connect = false;
    this->old_wall = NULL;
    this->connect_wall = NULL;
    this->start_connect_wall = NULL;

    //配置画墙时连接点图片
    connectDot = new QLabel(this);
    QPixmap pixmap(":/images/con_wall_dot");
    connectDot->setPixmap(pixmap);
    connectDot->setMask(pixmap.mask());
    connectDot->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    connectDot->setPalette(palette);
    connectDot->close();


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
    GL3D_GL()->glBindFramebuffer(GL_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
    GL3D_GL()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl3d_global_param::shared_instance()->framebuffer);
    glViewport(0, 0,
               this->main_scene->get_width(),
               this->main_scene->get_height());

    GL3D_GET_CURRENT_RENDER_PROCESS()->render();

    // 后渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->after_render();

    // release default fbo
    GL3D_GL()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    GL3D_GL()->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unlock render
    gl3d_lock::shared_instance()->render_lock.unlock();
}

void MOpenGLView::initializeGL() {
    QGLWidget::initializeGL();
    // here gat parent widget size and set to sel
    //    this->setGeometry(this->parentWidget()->geometry());

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
    this->wallsPoints = new QVector<point_wall_pair>();
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
        tmp_viewer->calculate_mat();
    }

    event->accept();      //接收该事件

    //获取画布上所有墙的顶点（用于吸附）
    this->getWallsPoint();
}

extern bool need_capture;

//opengl执行画墙
void MOpenGLView::openglDrawWall(const int x, const int y) {
    this->wall_temp_id = this->wall_temp_id + 1;
    gl3d::scene * vr = this->main_scene;
    // set wall
    glm::vec2 pick;
    //如果根据吸附点画的话就使用初始点
    if(!is_drawwall_start_point) {
        vr->coord_ground(glm::vec2(((float)x), ((float)y)), pick, 0.0);
    } else {
        vr->coord_ground(this->drawwall_start_point, pick, 0.0);
        is_drawwall_start_point = false;
    }

    //新建墙
    this->new_wall = new gl3d::gl3d_wall(pick, pick, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
    this->main_scene->add_obj(QPair<int , object *>(this->wall_temp_id, this->new_wall));
}

//获取屏幕上所有的墙
void MOpenGLView::getWallsPoint() {
    wallsPoints->clear();
    for (auto it = this->main_scene->get_objects()->begin();
         it != this->main_scene->get_objects()->end();
         it++) {
        gl3d::abstract_object * obj = *it;
        if (obj->get_obj_type() == obj->type_wall) {
            gl3d_wall * w = (gl3d_wall *)obj;
            glm::vec2 p1,p2;
            w->get_coord_on_screen(this->main_scene,
                                   p1, p2);
            wallsPoints->push_back(point_wall_pair(p1, w));
            wallsPoints->push_back(point_wall_pair(p2, w));
        }
    }
}


//鼠标按下事件
void MOpenGLView::mousePressEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //左键按下事件
    if(event->button() == Qt::LeftButton) {
        //获取画布上所有墙的顶点（用于吸附）
        this->getWallsPoint();

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


        //画房间节点开始
        if(now_state == gl3d::gl3d_global_param::drawhome) {
            this->drawhome_x1 = (float)event->x();
            this->drawhome_y1 = (float)event->y();
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhomeing;
        }
        //画房间节点结束
        if(now_state == gl3d::gl3d_global_param::drawhomeing) {
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhome;
            this->wall_temp_id = this->wall_temp_id + 4;
        }


        //画墙节点开始
        if(now_state == gl3d::gl3d_global_param::drawwall) {
            this->new_wall = NULL;
            this->openglDrawWall(event->x(), event->y());

            //开始画墙时新墙连接吸附墙
            if(this->start_connect_wall != NULL) {
                glm::vec2 tmpcoord;
                this->main_scene->coord_ground(this->drawwall_start_point, tmpcoord, 0.0f);
                if (
                        glm::length(
                            tmpcoord - this->start_connect_wall->get_start_point())
                        <=
                        glm::length(
                            tmpcoord - this->start_connect_wall->get_end_point())
                        )
                {
                    // pick up start
                    gl3d_wall::combine(
                                this->new_wall,
                                this->start_connect_wall,
                                gl3d_wall::combine_wall1_start_to_wall2_start
                                );
                }
                else {
                    // pick up end
                    gl3d_wall::combine(
                                this->new_wall,
                                this->start_connect_wall,
                                gl3d_wall::combine_wall1_start_to_wall2_end
                                );
                }


            }

            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwalling;
        }
        //画墙中节点结束
        if(now_state == gl3d::gl3d_global_param::drawwalling) {
            if(!is_drawwall_connect) {
                this->old_wall = this->new_wall;

                this->openglDrawWall(event->x(), event->y());

                //新墙连接老墙
                if(this->old_wall != NULL) {
                    gl3d_wall::combine(this->old_wall, this->new_wall, gl3d_wall::combine_wall1_end_to_wall2_start);
                }
            } else {
                //新墙连接吸附墙
                if(this->connect_wall != NULL) {
                    gl3d_wall::combine(this->new_wall, this->connect_wall, this->new_wall->get_end_point());
                }

                gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
            }
        }


        //右键按下事件
    } else if(event->button() == Qt::RightButton) {
        //点击右键-取消画墙,画房间状态
        if(now_state == gl3d::gl3d_global_param::drawwall || now_state == gl3d::gl3d_global_param::drawhome) {
            drawhomewin::on_draw_clear();
        }
        //点击右键-终止画墙连接
        if(now_state == gl3d::gl3d_global_param::drawwalling) {
            this->main_scene->delete_obj(this->wall_temp_id);
            delete this->new_wall;
            this->new_wall = NULL;
            this->old_wall = NULL;

            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        }
        //点击右键-终止画房间连接
        if(now_state == gl3d::gl3d_global_param::drawhomeing) {
            for (int i = 1; i != 5; i++) {
                this->main_scene->delete_obj(this->wall_temp_id + i);
            }
            delete this->new_walla;
            delete this->new_wallb;
            delete this->new_wallc;
            delete this->new_walld;
            this->new_walla = NULL;
            this->new_wallb = NULL;
            this->new_wallc = NULL;
            this->new_walld = NULL;
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhome;
        }
    } else if(event->button() == Qt::MidButton) {
    }
}

//鼠标移动事件
void MOpenGLView::mouseMoveEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //画墙状态下吸附辅助点显示逻辑
    if(now_state == gl3d::gl3d_global_param::drawwall) {
        setCursor(Qt::CrossCursor);
        connectDot->close();
        is_drawwall_start_point = false;
        this->start_connect_wall = NULL;

        if(this->wallsPoints != NULL) {
            for(QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                it != this->wallsPoints->end(); it++) {
                glm::vec2 tmp((float)event->x(), (float)event->y());
                float dis = glm::length(tmp - (*it).first);
                if(dis < 18.0f) {
                    connectDot->setGeometry((*it).first.x - 12, (*it).first.y - 12, 24, 24);
                    connectDot->show();
                    this->drawwall_start_point = (*it).first;
                    is_drawwall_start_point = true;

                    this->start_connect_wall = (*it).second;
                }
            }
        }
    }

    //画墙中
    if(now_state == gl3d::gl3d_global_param::drawwalling) {
        setCursor(Qt::CrossCursor);
        connectDot->close();
        is_drawwall_connect = false;
        this->connect_wall = NULL;

        glm::vec2 pick;
        gl3d::scene * vr = this->main_scene;
        vr->coord_ground(glm::vec2(((float)event->x()), ((float)event->y())), pick, 0.0);
        this->new_wall->set_end_point(pick);
        this->new_wall->calculate_mesh();

        if(this->wallsPoints != NULL) {
            for(QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                it != this->wallsPoints->end(); it++) {
                glm::vec2 tmp((float)event->x(), (float)event->y());
                float dis = glm::length(tmp - (*it).first);
                if(dis < 18.0f) {
                    vr->coord_ground((*it).first, pick, 0.0);
                    this->new_wall->set_end_point(pick);
                    this->new_wall->calculate_mesh();

                    connectDot->setGeometry((*it).first.x - 12, (*it).first.y - 12, 24, 24);
                    connectDot->show();
                    is_drawwall_connect = true;

                    this->connect_wall = (*it).second;
                }
            }
        }

        if(this->old_wall != NULL) {
            this->old_wall->calculate_mesh();
        }
        if(this->start_connect_wall != NULL) {
            this->start_connect_wall->calculate_mesh();
        }
    }

    if(now_state == gl3d::gl3d_global_param::drawhome) {
        setCursor(Qt::CrossCursor);
    }

    //画房间中
    if(now_state == gl3d::gl3d_global_param::drawhomeing) {
        setCursor(Qt::CrossCursor);
        this->drawhome_x2 = (float)event->x();
        this->drawhome_y2 = (float)event->y();

        this->new_walla = NULL;
        this->new_wallb = NULL;
        this->new_wallc = NULL;
        this->new_walld = NULL;

        gl3d::scene * vr = this->main_scene;

        //set walla
        glm::vec2 picka;
        vr->coord_ground(glm::vec2((this->drawhome_x1),
                                   (this->drawhome_y1)),
                         picka, 0.0f);
        this->new_walla = new gl3d::gl3d_wall(picka, picka, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
        vr->add_obj(QPair<int , object *>(this->wall_temp_id + 1, this->new_walla));
        vr->coord_ground(glm::vec2((this->drawhome_x2),
                                   (this->drawhome_y1)),
                         picka, 0.0f);
        this->new_walla->set_end_point(picka);
        this->new_walla->calculate_mesh();

        //set wallb
        glm::vec2 pickb;
        vr->coord_ground(glm::vec2((this->drawhome_x1),
                                   (this->drawhome_y1)),
                         pickb, 0.0);
        this->new_wallb = new gl3d::gl3d_wall(pickb, pickb, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
        vr->add_obj(QPair<int , object *>(this->wall_temp_id + 2, this->new_wallb));
        vr->coord_ground(glm::vec2((this->drawhome_x1),
                                   (this->drawhome_y2)),
                         pickb, 0.0);
        this->new_wallb->set_end_point(pickb);
        this->new_wallb->calculate_mesh();

        //set wallc
        glm::vec2 pickc;
        vr->coord_ground(glm::vec2((this->drawhome_x2),
                                   (this->drawhome_y1)),
                         pickc, 0.0);
        this->new_wallc = new gl3d::gl3d_wall(pickc, pickc, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
        vr->add_obj(QPair<int , object *>(this->wall_temp_id + 3, this->new_wallc));
        vr->coord_ground(glm::vec2((this->drawhome_x2),
                                   (this->drawhome_y2)),
                         pickc, 0.0);
        this->new_wallc->set_end_point(pickc);
        this->new_wallc->calculate_mesh();

        //set walld
        glm::vec2 pickd;
        vr->coord_ground(glm::vec2((this->drawhome_x1),
                                   (this->drawhome_y2)),
                         pickd, 0.0);
        this->new_walld = new gl3d::gl3d_wall(pickd, pickd, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
        vr->add_obj(QPair<int , object *>(this->wall_temp_id + 4, this->new_walld));
        vr->coord_ground(glm::vec2((this->drawhome_x2),
                                   (this->drawhome_y2)),
                         pickd, 0.0);
        this->new_walld->set_end_point(pickd);

        //处理两堵墙连接处锯齿
        gl3d_wall::combine(new_walla, new_wallb, new_walla->get_start_point());
        gl3d_wall::combine(new_wallb, new_walld, new_wallb->get_end_point());
        gl3d_wall::combine(new_wallc, new_walld, new_wallc->get_end_point());
        gl3d_wall::combine(new_wallc, new_walla, new_wallc->get_start_point());

        this->new_walld->calculate_mesh();
    }


    if(event->buttons()&Qt::LeftButton) {
        //        cout << "left move: " << event->x() << ", " << event->y() << endl;
        auto tmp_viewer = this->main_scene->watcher;
        if (tmp_viewer->get_view_mode() == tmp_viewer->top_view) {
            //            cout << float(event->x() - this->tmp_point_x) / 100 << endl;
            tmp_viewer->change_position(glm::vec3(-float(event->x() - this->tmp_point_x) / 50, float(event->y() - this->tmp_point_y) / 50, 0.0));
            this->tmp_point_x = event->x();
            this->tmp_point_y = event->y();
            setCursor(Qt::SizeAllCursor);
            tmp_viewer->calculate_mat();

            //获取画布上所有墙的顶点（用于吸附）
            this->getWallsPoint();
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
    if(gl3d::gl3d_global_param::shared_instance()->current_work_state != gl3d::gl3d_global_param::drawwall
            && gl3d::gl3d_global_param::shared_instance()->current_work_state != gl3d::gl3d_global_param::drawhome) {
        setCursor(Qt::ArrowCursor);
    }
}
