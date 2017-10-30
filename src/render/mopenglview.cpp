#include "kaola_engine/mopenglview.h"
#include "editor/sign_config.h"
#include <gl/glext.h>

using namespace std;

// 全局OpenGL Functions
QOpenGLFunctions_3_3_Core *gl3d_win_gl_functions = NULL;

void MOpenGLView::draw_image(QString img, float x, float y, int w, int h) {
    QPainter pter(this->main_scene->get_assistant_image());
    QPixmap pix;
    pix.load(img);
    pter.drawPixmap(x, y, w, h, pix);
}

void MOpenGLView::closeEvent(QCloseEvent *event) {
    // test save file
    pugi::xml_document doc;
    pugi::xml_node rootnode = doc.root();
    rootnode = rootnode.append_child("scheme");
    doc.save_file("test_sketch.xml");

    // test save style
    pugi::xml_document sdoc;
    pugi::xml_node rtnd = sdoc.root();
    rtnd = rtnd.append_child("style");
    sdoc.save_file("test_style.xml");

    this->hide();
    // release render process
    render_process_manager::get_shared_instance()->release_render();
    // release scene
    if (NULL != this->main_scene)
        delete this->main_scene;
    this->main_scene = NULL;
    if (this->isInitialized()) {
        // delete all shaders
        shader_manager *shader_mgr = ::shader_manager::sharedInstance();
        Q_FOREACH(gl3d::Program * ip, shader_mgr->shaders) {
                delete ip;
            }
        shader_mgr->shaders.clear();
    }

    QGLWidget::closeEvent(event);
    event->accept();
}

MOpenGLView::~MOpenGLView() {
//    QGLWidget::~QGLWidget();
    // delete scene this
    if (NULL != this->main_scene)
        delete this->main_scene;
    this->main_scene = NULL;

    // delete all programs
    if (this->isInitialized()) {
        shader_manager *shader_mgr = ::shader_manager::sharedInstance();
        Q_FOREACH(gl3d::Program * ip, shader_mgr->shaders) {
                delete ip;
            }
        shader_mgr->shaders.clear();
    }
}

void MOpenGLView::do_init() {
    //yananli code
    this->setMouseTracking(true);
    this->new_wall = NULL;
    is_drawwall_start_point = false;
    is_drawwall_connect = false;
    this->old_wall = NULL;
    this->connect_wall = NULL;
    this->start_connect_wall = NULL;
    this->pickUpObjID = -1;

    // set OPENGL context
    timer = new QTimer(this);
    timer->start(15);

    // init path KENT TODO : shader目录设置要调整
    this->res_path = GL3D_PATH_SHADER;

    // create sketch and scene
    this->create_scene();
    gl3d_global_param::shared_instance()->main_scene = this->main_scene;

    GL3D_SET_CURRENT_RENDER_PROCESS(normal, this->main_scene);

    this->main_scene->prepare_buffer();
    this->main_scene->gen_shadow_texture();

    // 一行代码用glkit设置好深度测试
    // 一行代码用glkit设置好模板缓冲区

    // set image for assistant lines
    this->main_scene->set_assistant_image(
            new QImage(this->width(), this->height(), QImage::Format_RGBA8888));
    this->main_scene->get_assistant_image()->fill(0);

    // advanced
    leftMousePressed = false;
    midMousePressed = false;
}

#define MAX_FILE_SIZE 10000

void MOpenGLView::create_scene() {
    this->main_scene = new gl3d::scene(this->height(), this->width());
    gl3d::gl3d_global_param::shared_instance()->canvas_height = this->height();
    gl3d::gl3d_global_param::shared_instance()->canvas_width = this->width();
    float xsss = this->width();
    float ysss = this->height();

    shader_manager *shader_mgr = ::shader_manager::sharedInstance();
    Program *prog;
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
        string tmp_vert((char *) tmp_v.data(), res_v.size());

        // read frag
        QFile res_f(QString(frag.c_str()));
        if (!res_f.open(QFile::ReadOnly)) {
            cout << res_f.errorString().toStdString() << endl;
            throw std::runtime_error("Load shader " + vert + res_f.errorString().toStdString());
        }
        auto tmp_f = res_f.readAll();
        string tmp_frag((char *) tmp_f.data(), res_f.size());

        // create program
        prog = new Program(tmp_vert, tmp_frag);
        shader_mgr->shaders.insert(load_iter.key(), prog);
    }

    return;
}

MOpenGLView::MOpenGLView() : QGLWidget() {
    throw std::invalid_argument("should not create openglview without parent widget");
}

bool need_capture;

void MOpenGLView::paintGL() {
    QGLWidget::paintGL();
    // lock render
    if (!gl3d_lock::shared_instance()->render_lock.tryLock()) {
        return;
    }
    this->view_change();
    if (this->main_scene->watcher->viewerChanged) {
        main_scene->spaceManager->cullObjects(main_scene->watcher,
                                              gl3d_global_param::shared_instance()->maxCulledObjNum);
        this->main_scene->watcher->viewerChanged = false;
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
//    gl3d_framebuffer fb(gl3d_global_param::shared_instance()->framebuffer,
//                        glm::vec2(
//                            this->main_scene->get_width(),
//                            this->main_scene->get_height()
//                            ));
//    fb.save_to_file("test2.jpg");

    // 后渲染
    GL3D_GET_CURRENT_RENDER_PROCESS()->after_render();

    // release default fbo
    GL3D_GL()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    GL3D_GL()->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unlock render
    gl3d_lock::shared_instance()->render_lock.unlock();
    this->fps += 1;
}

void MOpenGLView::initializeGL() {
    QGLWidget::initializeGL();
    // here gat parent widget size and set to sel
    //    this->setGeometry(this->parentWidget()->geometry());

    this->initializeOpenGLFunctions();
    gl3d_win_gl_functions = this;

    const GLubyte *OpenGLVersion = glGetString(GL_VERSION);
    string glv((char *) OpenGLVersion);
    cout << "OpenGL " << glv << endl;

    this->do_init();

    this->setFocusPolicy(Qt::StrongFocus);
    this->keyTimer = new QTimer();
    this->keyTimer->start(1000);
    connect(keyTimer, &QTimer::timeout, this, [&](){
        qDebug() << this->fps;
        this->fps = 0;
    });

    this->connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
//    this->connect(keyTimer, SIGNAL(timeout()), this, SLOT(view_change()));
    this->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void MOpenGLView::view_change() {
    if (this->key_press == Qt::Key_A) {
        this->main_scene->watcher->change_position(glm::vec3(-1.0, 0.0, 0.0) * 0.1);
    } else if (this->key_press == Qt::Key_S) {
        this->main_scene->watcher->change_position(glm::vec3(0.0, -1.0, 0.0) * 0.1);
    } else if (this->key_press == Qt::Key_D) {
        this->main_scene->watcher->change_position(glm::vec3(1.0, 0.0, 0.0) * 0.1);
    } else if (this->key_press == Qt::Key_W) {
        this->main_scene->watcher->change_position(glm::vec3(0.0, 1.0, 0.0) * 0.1);
    } else if (this->key_press == Qt::Key_Space) {
        this->main_scene->watcher->change_position(glm::vec3(0.0, 0.0, 1.0) * 0.1);
    }

    if ((this->main_scene != NULL) &&
            (this->main_scene->watcher->viewerChanged)) {
        this->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));
        this->key_press = -1;
        GL3D_GET_CURRENT_RENDER_PROCESS()->invoke();
    }
}

void MOpenGLView::keyPressEvent(QKeyEvent *event) {
    QGLWidget::keyPressEvent(event);
    this->key_press = event->key();
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
    this->points_for_walls = new QVector<points_wall>();
}

void MOpenGLView::resizeGL(int width, int height) {
    QGLWidget::resizeGL(width, height);
    // here gat parent widget size and set to sel
    //    this->setGeometry(this->parentWidget()->geometry());
    this->main_scene->set_height((float) height);
    this->main_scene->set_width((float) width);
    this->main_scene->watcher->set_width((float) width);
    this->main_scene->watcher->set_height((float) height);
    this->main_scene->watcher->calculate_mat();
    gl3d::gl3d_global_param::shared_instance()->canvas_height = (float) height;
    gl3d::gl3d_global_param::shared_instance()->canvas_width = (float) width;
    // resize assistant image , TODO : here may lead to memory leakage
    QImage *ori = this->main_scene->get_assistant_image();
    this->main_scene->set_assistant_image(
            new QImage(this->width(), this->height(), QImage::Format_RGBA8888));
    //    QPainter * pt = this->main_scene->get_assistant_drawer();
    //    this->main_scene->set_assistant_drawer(new QPainter(this->main_scene->get_assistant_image()));
    this->main_scene->get_assistant_image()->fill(0);
}


//yananli codes ----------------------------------------------------------------------

//滚轮滑动事件
void MOpenGLView::wheelEvent(QWheelEvent *event) {
    event->accept();      //接收该事件
    auto tmp_viewer = this->main_scene->watcher;

    //拾取obj
    int pobjid = this->main_scene->get_object_id_by_coordination(event->x(), event->y());
    qDebug() << pobjid;
    if (pickUpObjID > 0) {
        this->main_scene->get_obj(pickUpObjID)->set_pick_flag(false);
        pickUpObjID = -1;
    }
    if (pobjid > 0) {
        pickUpObjID = pobjid;
        this->main_scene->get_obj(pickUpObjID)->set_pick_flag(true);
    }
    // start arc ball rotate
    main_scene->watcher->startArcballRotate(event->pos());
    if (pickUpObjID > 0) {
        main_scene->watcher->rotateCenterPoint = main_scene->get_obj(pickUpObjID)->getCenterPointInWorldCoord();
    }

    float distance = 1.0;
    distance *= event->delta() / 128.0;
    main_scene->watcher->pullPushArcBall(distance);
    main_scene->watcher->endArcballRotate();
    main_scene->watcher->calculate_mat();
}

extern bool need_capture;

//墙顶点吸附坐标计算
glm::vec2 MOpenGLView::wallPeakAdsorption(glm::vec2 pt, float dis_com) {
    if (this->wallsPoints->length() != 0) {
        for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
             it != this->wallsPoints->end(); it++) {
            float dis = glm::length(pt - (*it).first);
            if (dis < dis_com) {
                return glm::vec2((*it).first.x, (*it).first.y);
            }
        }
        return pt;
    } else {
        return pt;
    }
}

//墙体吸附坐标计算
glm::vec2 MOpenGLView::wallLineAdsorption(glm::vec2 pt, float dis_com) {
    if (this->points_for_walls->length() != 0) {
        for (QVector<points_wall>::iterator it = this->points_for_walls->begin();
             it != this->points_for_walls->end(); it++) {
            gl3d::math::line_2d wall_line((*it).first, (*it).second);
            float dis = gl3d::math::point_distance_to_line(pt, wall_line);
            if (dis < dis_com) {
                glm::vec2 line_connect_point;
                bool sucess = gl3d::math::point_project_to_line(wall_line, pt, line_connect_point);
                if (sucess) {
                    if ((glm::length(line_connect_point - wall_line.a)
                         + glm::length(line_connect_point - wall_line.b))
                        <= (glm::length(wall_line.a - wall_line.b + 0.000f))) {
                        return line_connect_point;
                    }
                }
            }
        }
        return pt;
    } else {
        return pt;
    }
}

//墙顶点直角吸附坐标计算
glm::vec2 MOpenGLView::wallPeakRightAngleAdsorption(glm::vec2 pt) {
    if (this->wallsPoints->length() != 0) {
        typedef QPair<glm::vec2, glm::vec2> pair;
        QVector<pair> points;

        for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
             it != this->wallsPoints->end(); it++) {
            float angle_dis_x = glm::length(pt.x - (*it).first.x);
            float angle_dis_y = glm::length(pt.y - (*it).first.y);

            if (angle_dis_x < 15.0f) {
                points.push_back(pair(glm::vec2((*it).first.x, pt.y),
                                      glm::vec2((*it).first.x, (*it).first.y)));
            }

            if (angle_dis_y < 15.0f) {
                points.push_back(pair(glm::vec2(pt.x, (*it).first.y),
                                      glm::vec2((*it).first.x, (*it).first.y)));
            }
        }

        if (points.length() != 0) {
            //找出距离pt最近的点并返回
            float mixDis = 9999.0f;
            glm::vec2 mixDot;
            glm::vec2 mixPt;

            for (QVector<pair>::iterator it = points.begin();
                 it != points.end(); it++) {
                float dis = glm::length(pt - (*it).second);
                if (dis < mixDis) {
                    mixDis = dis;
                    mixPt = (*it).first;
                    mixDot = (*it).second;
                }
            }

            QPen pen;  // creates a default pen
            pen.setStyle(Qt::DashLine);
            pen.setWidth(2);
            pen.setBrush(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            QPainter pter(this->main_scene->get_assistant_image());
            pter.setPen(pen);
            pter.drawLine(mixDot.x, mixDot.y,
                          mixPt.x, mixPt.y);
            return mixPt;
        } else {
            return pt;
        }
    } else {
        return pt;
    }
}

//鼠标按下事件
void MOpenGLView::mousePressEvent(QMouseEvent *event) {
    this->main_scene->get_assistant_image()->fill(0);
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //左键按下事件
    if (event->button() == Qt::LeftButton || event->button() == Qt::MidButton) {
        //拾取并新建选项框
        if (now_state == gl3d::gl3d_global_param::normal) {
            need_capture = true;
            int pobjid = this->main_scene->get_object_id_by_coordination(event->x(), event->y());
            qDebug() << pobjid;
            if (pickUpObjID > 0) {
                this->main_scene->get_obj(pickUpObjID)->set_pick_flag(false);
                pickUpObjID = -1;
            }
            if (pobjid > 0) {
                pickUpObjID = pobjid;
                this->main_scene->get_obj(pickUpObjID)->set_pick_flag(true);
            }
        }
        // start arc ball rotate
        main_scene->watcher->startArcballRotate(event->pos());
        if (pickUpObjID > 0) {
            main_scene->watcher->rotateCenterPoint = main_scene->get_obj(pickUpObjID)->getCenterPointInWorldCoord();
        }
    }

    if (event->button() == Qt::LeftButton) {
        leftMousePressed = true;
    } else if (event->button() == Qt::MidButton) {
        midMousePressed = true;
    }
}

//鼠标移动事件
void MOpenGLView::mouseMoveEvent(QMouseEvent *event) {
    this->main_scene->get_assistant_image()->fill(0);
    this->draw_assistant_img();

    if (leftMousePressed) {
        main_scene->watcher->updateArcballRotate(event->pos());
    } else if (midMousePressed) {
        main_scene->watcher->updateArcballFlatMove(event->pos());
    }
}

//鼠标松开事件
void MOpenGLView::mouseReleaseEvent(QMouseEvent *event) {
    this->main_scene->get_assistant_image()->fill(0);
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;
    if (leftMousePressed || midMousePressed) {
        main_scene->watcher->endArcballRotate();
        leftMousePressed = false;
        midMousePressed = false;
    }
}

void MOpenGLView::draw_assistant_img() {
    QPainter ptr(this->main_scene->get_assistant_image());
    auto now_work = gl3d_global_param::shared_instance()->current_work_state;
    if (now_work == gl3d_global_param::draw_area) {
        // draw area
        auto apts = (QVector<glm::vec3> *) this->user_data.value("area_points");
        auto nowapt = (glm::vec3 *) this->user_data.value("current_point");
        QPolygon poly;
        bool add_nowpt = true;
        Q_FOREACH(glm::vec3 pit, *apts) {
                glm::vec2 ptin = this->main_scene->project_point_to_screen(pit);
                ptin = math::point_clamp_in_range(ptin, glm::vec2(0.0f), glm::vec2(this->width(), this->height()));
                poly.append(QPoint(ptin.x, ptin.y));
                if (math::point_near_point(*nowapt, pit)) {
                    add_nowpt = false;
                }
            }
        if (add_nowpt) {
            glm::vec2 ptin = this->main_scene->project_point_to_screen(*nowapt);
            ptin = math::point_clamp_in_range(ptin, glm::vec2(0.0f), glm::vec2(this->width(), this->height()));
            poly.append(QPoint(ptin.x, ptin.y));
        }
        QBrush br(QColor(250, 0, 0, 64), Qt::SolidPattern);
        ptr.setBrush(br);
        if (poly.size() >= 3)
            ptr.drawConvexPolygon(poly);
    }
}
