#include <include/editor/command.h>
#include "kaola_engine/mopenglview.h"

using namespace std;

// 全局OpenGL Functions
QOpenGLFunctions_4_1_Core *gl3d_win_gl_functions = NULL;

void MOpenGLView::draw_image(QString img, float x, float y, int w, int h) {
    QPainter pter(this->main_scene->get_assistant_image());
    QPixmap pix;
    pix.load(img);
    pter.drawPixmap(x, y, w, h, pix);
}

void MOpenGLView::closeEvent(QCloseEvent *event) {
    this->hide();
    // clear undo and redo
    klm::command::command_stack::shared_instance()->clear();
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
        // release all materials
        global_material_lib::shared_instance()->release_all_mtl();
    }

    QGLWidget::closeEvent(event);
    event->accept();
}

MOpenGLView::~MOpenGLView() {
    QGLWidget::~QGLWidget();
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

    klm::command::command_stack::shared_instance()->clear();
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

    // set OPENGL context
    timer = new QTimer(this);
    timer->start(50);

    // init path KENT TODO : shader目录设置要调整
    this->res_path = GL3D_PATH_SHADER;

    // create sketch and scene
    this->create_scene();
    // test codes
//    if (gl3d_global_param::shared_instance()->old_sketch_test != NULL)
//        this->sketch = (klm::design::scheme *) gl3d_global_param::shared_instance()->old_sketch_test;
//    else
//        this->sketch = new klm::design::scheme(this->main_scene);
//    gl3d_global_param::shared_instance()->old_sketch_test = this->sketch;
    this->sketch = new klm::design::scheme(this->main_scene);
    this->sketch->set_attached_scene(this->main_scene);
    this->main_scene->set_attached_sketch(this->sketch);
    klm::command::command_stack::init(this->main_scene, this->sketch);
    this->main_scene->add_obj(this->sketch->get_id(), this->sketch);

    GL3D_SET_CURRENT_RENDER_PROCESS(normal, this->main_scene);

    this->main_scene->prepare_buffer();
    this->main_scene->gen_shadow_texture();

    // 一行代码用glkit设置好深度测试
    // 一行代码用glkit设置好模板缓冲区

    // set image for assistant lines
    this->main_scene->set_assistant_image(
            new QImage(this->width(), this->height(), QImage::Format_RGBA8888));
    this->main_scene->get_assistant_image()->fill(0);
    //    this->main_scene->set_assistant_drawer(new QPainter(this->main_scene->get_assistant_image()));
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

    const GLubyte *OpenGLVersion = glGetString(GL_VERSION);
    string glv((char *) OpenGLVersion);
    cout << "OpenGL " << glv << endl;

    this->do_init();

    this->setFocusPolicy(Qt::StrongFocus);
    this->keyTimer = new QTimer();
    this->keyTimer->start(50);

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

    if (this->main_scene != NULL) {
        this->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));
        this->key_press = 0;
        GL3D_GET_CURRENT_RENDER_PROCESS()->invoke();
    }
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
    auto tmp_viewer = this->main_scene->watcher;

    //滚动的角度，*8就是鼠标滚动的距离
    int numDegrees = event->delta() / 8;
    //滚动的步数，*15就是鼠标滚动的角度
    int numSteps = tmp_viewer->get_top_view_size() - (numDegrees / 15);

    if (tmp_viewer->get_view_mode() == tmp_viewer->top_view) {
        if (numSteps > 0) {
            tmp_viewer->set_top_view_size((float) numSteps);
            tmp_viewer->calculate_mat();
        }
    }

    event->accept();      //接收该事件

    //获取画布上所有墙的顶点（用于吸附）
    this->getWallsPoint();
}

extern bool need_capture;

//opengl执行画墙
void MOpenGLView::openglDrawWall(const int x, const int y) {
    this->wall_temp_id = this->wall_temp_id + 1;
    gl3d::scene *vr = this->main_scene;
    // set wall
    glm::vec2 pick;
    //如果根据吸附点画的话就使用初始点
    if (!is_drawwall_start_point) {
        vr->coord_ground(glm::vec2((float) x, (float) y), pick, 0.0);
        this->draw_start_dot = glm::vec2((float) x, (float) y);
    } else {
        vr->coord_ground(this->drawwall_start_point, pick, 0.0);
        this->draw_start_dot = this->drawwall_start_point;
        is_drawwall_start_point = false;
    }

    //新建墙
    this->new_wall = new gl3d::gl3d_wall(pick, pick, gl3d::gl3d_global_param::shared_instance()->wall_thick, 2.8);
    this->main_scene->add_obj(this->wall_temp_id, this->new_wall);
}

//获取屏幕上所有的墙-----获取屏幕上所有的墙的顶点
void MOpenGLView::getWallsPoint() {
    wallsPoints->clear();
    points_for_walls->clear();

    for (auto it = this->sketch->get_objects()->begin();
         it != this->sketch->get_objects()->end();
         it++) {
        gl3d::abstract_object *obj = *it;
        if (obj->get_obj_type() == obj->type_wall) {
            gl3d_wall *w = (gl3d_wall *) obj;
            glm::vec2 p1, p2;
            w->get_coord_on_screen(this->main_scene,
                                   p1, p2);
            wallsPoints->push_back(point_wall_pair(p1, w));
            wallsPoints->push_back(point_wall_pair(p2, w));

            points_for_walls->push_back(points_wall(p1, p2));
        }
    }
}

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
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //左键按下事件
    if (event->button() == Qt::LeftButton) {
        //获取画布上所有墙的顶点（用于吸附）
        this->getWallsPoint();

        //拾取并新建选项框
        if (now_state == gl3d::gl3d_global_param::normal) {
            need_capture = true;
            pickUpObjID = this->main_scene->get_object_id_by_coordination(event->x(), event->y());
            if (pickUpObjID > 0) {
                cout << "click objid: " << pickUpObjID << endl;
                puDig = new PickupDig(this->parentWidget(), event->x(), event->y(), pickUpObjID, this->main_scene,
                                      this->sketch, glm::vec2(event->x(), event->y()));
                puDig->show();
                gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::pickup;
            }
        }

        //取消拾取
        if (now_state == gl3d::gl3d_global_param::pickup) {
            puDig->close();
            delete puDig;
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
        }

        this->tmp_point_x = event->x();
        this->tmp_point_y = event->y();


        //画房间节点开始
        if (now_state == gl3d::gl3d_global_param::drawhome) {
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhomeing;
        }
        //画房间节点结束
        if (now_state == gl3d::gl3d_global_param::drawhomeing) {
            gl3d_wall *tmp;
            this->sketch->add_wall(this->new_walla, tmp);
            this->sketch->add_wall(this->new_wallb, tmp);
            this->sketch->add_wall(this->new_wallc, tmp);
            this->sketch->add_wall(this->new_walld, tmp);
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhome;
            this->wall_temp_id = this->wall_temp_id + 4;
        }


        //画墙节点开始
        if (now_state == gl3d::gl3d_global_param::drawwall) {
            this->new_wall = NULL;
            this->openglDrawWall(event->x(), event->y());

            //开始画墙时新墙连接吸附墙
            if (this->start_connect_wall != NULL) {
                glm::vec2 tmpcoord;
                this->main_scene->coord_ground(this->drawwall_start_point, tmpcoord, 0.0f);
                if (
                        glm::length(
                                tmpcoord - this->start_connect_wall->get_start_point())
                        <=
                        glm::length(
                                tmpcoord - this->start_connect_wall->get_end_point())
                        ) {
                    // pick up start
                    gl3d_wall::combine(
                            this->new_wall,
                            this->start_connect_wall,
                            gl3d_wall::combine_wall1_start_to_wall2_start
                    );
                } else {
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
        if (now_state == gl3d::gl3d_global_param::drawwalling) {
            if (!is_drawwall_connect) {
                this->old_wall = this->new_wall;
                //使用老墙的结束点坐标作为新墙的起始点
                glm::vec2 old_wall_end_point = this->main_scene->project_point_to_screen(
                        this->old_wall->get_end_point());
                this->openglDrawWall(old_wall_end_point.x, old_wall_end_point.y);

                //新墙连接老墙
                if (this->old_wall != NULL) {
                    gl3d_wall::combine(this->old_wall, this->new_wall, gl3d_wall::combine_wall1_end_to_wall2_start);
                    gl3d_wall *tmppp = NULL;
                    this->sketch->add_wall(this->old_wall, tmppp);
                    if (tmppp != NULL) {
                        this->old_wall = tmppp;
                        this->old_wall->calculate_mesh();
                    }
                }
            } else {
                //新墙连接吸附墙
                if (this->connect_wall != NULL) {
                    gl3d_wall::combine(this->new_wall, this->connect_wall, this->new_wall->get_end_point());
                }
                gl3d_wall *tmppp = NULL;
                this->sketch->add_wall(this->new_wall, tmppp);
                if (tmppp != NULL) {
                    this->new_wall = tmppp;
                    this->new_wall->calculate_mesh();
                }
                tmppp = NULL;
                this->sketch->add_wall(this->connect_wall, tmppp);
                if (tmppp != NULL) {
                    this->connect_wall = tmppp;
                    this->connect_wall->calculate_mesh();
                }

                gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
            }
        }
        //右键按下事件
    } else if (event->button() == Qt::RightButton) {
        //点击右键-取消画墙,画房间状态
        if (now_state == gl3d::gl3d_global_param::drawwall || now_state == gl3d::gl3d_global_param::drawhome ||
            now_state == gl3d::gl3d_global_param::opendoor) {
            drawhomewin::on_draw_clear();
            setCursor(Qt::ArrowCursor);
            this->main_scene->get_assistant_image()->fill(0);
        }
        //点击右键-终止画墙连接
        if (now_state == gl3d::gl3d_global_param::drawwalling) {
            this->main_scene->get_assistant_image()->fill(0);
            this->main_scene->delete_obj(this->wall_temp_id);
            delete this->new_wall;
            this->new_wall = NULL;
            this->old_wall = NULL;

            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        }
        //点击右键-终止画房间连接
        if (now_state == gl3d::gl3d_global_param::drawhomeing) {
            this->main_scene->get_assistant_image()->fill(0);
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
    } else if (event->button() == Qt::MidButton) {
    }
}

//鼠标移动事件
void MOpenGLView::mouseMoveEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //移动墙顶点--------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::normal || now_state == gl3d::gl3d_global_param::movewall) {
        glm::vec2 tmp_pt((float) event->x(), (float) event->y());
        setCursor(Qt::ArrowCursor);
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;

        //墙顶点吸附
        if (this->wallsPoints->length() != 0) {
            for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                 it != this->wallsPoints->end(); it++) {
                float dis = glm::length(tmp_pt - (*it).first);
                if (dis < 15.0f) {
                    float start_dis = glm::length(
                            (*it).first - this->main_scene->project_point_to_screen(
                                    (*it).second->get_start_point()
                            )
                    );
                    float end_dis = glm::length(
                            (*it).first - this->main_scene->project_point_to_screen(
                                    (*it).second->get_end_point()
                            )
                    );

                    if (start_dis == 0) {
                        move_change_is_start_or_end = true;
                    } else if (end_dis == 0) {
                        move_change_is_start_or_end = false;
                    }

                    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::movewall;
                    move_change_wall = (*it).second;
                    setCursor(Qt::SizeAllCursor);
                }
            }
        }
    }

    //画墙----------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::drawwall) {
        setCursor(Qt::CrossCursor);
        this->main_scene->get_assistant_image()->fill(0);
        is_drawwall_start_point = false;
        this->start_connect_wall = NULL;
        glm::vec2 tmp_pt((float) event->x(), (float) event->y());
        glm::vec2 conn_dot = tmp_pt;

        //墙顶点吸附
        if (this->wallsPoints->length() != 0) {
            for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                 it != this->wallsPoints->end(); it++) {
                float dis = glm::length(tmp_pt - (*it).first);
                if (dis < 18.0f) {
                    is_drawwall_start_point = true;
                    conn_dot = (*it).first;
                    this->start_connect_wall = (*it).second;
                }
            }
        }

        //墙体吸附
        if (!is_drawwall_start_point) {
            glm::vec2 point = this->wallLineAdsorption(tmp_pt, 18.0f);
            if (glm::length(point - tmp_pt) > 0.01) {
                conn_dot = point;
                is_drawwall_start_point = true;
            }
        }

        //墙顶点直角吸附
        if (!is_drawwall_start_point) {
            glm::vec2 point = this->wallPeakRightAngleAdsorption(tmp_pt);
            if (glm::length(point - tmp_pt) > 0.01) {
                conn_dot = point;
                is_drawwall_start_point = true;
            }
        }

        //显示辅助点逻辑
        if (is_drawwall_start_point) {
            this->drawwall_start_point = conn_dot;
            this->draw_image(":/images/con_wall_dot",
                             conn_dot.x - 12, conn_dot.y - 12, 24, 24);
        }
    }


    //画墙中----------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::drawwalling) {
        setCursor(Qt::CrossCursor);
        this->main_scene->get_assistant_image()->fill(0);
        is_drawwall_connect = false;
        this->connect_wall = NULL;

        glm::vec2 tmp_pt((float) event->x(), (float) event->y());
        glm::vec2 pick;
        gl3d::scene *vr = this->main_scene;

        bool is_show_conn_dot = false;
        glm::vec2 conn_dot = tmp_pt;

        //墙顶点吸附
        if (this->wallsPoints->length() != 0) {
            for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                 it != this->wallsPoints->end(); it++) {
                float dis = glm::length(tmp_pt - (*it).first);
                if (dis < 18.0f) {
                    tmp_pt = (*it).first;
                    is_show_conn_dot = true;
                    conn_dot = (*it).first;
                    is_drawwall_connect = true;
                    this->connect_wall = (*it).second;
                }
            }
        }

        //墙体吸附
        if (!is_drawwall_connect) {
            glm::vec2 point = this->wallLineAdsorption(tmp_pt, 18.0f);
            if (glm::length(point - tmp_pt) > 0.001) {
                is_show_conn_dot = true;
                is_drawwall_connect = true;
                conn_dot = point;
                tmp_pt = point;
            }
        }

        //墙顶点直角吸附
        if (!is_drawwall_connect) {
            glm::vec2 point = this->wallPeakRightAngleAdsorption(tmp_pt);
            if (glm::length(point - tmp_pt) > 0.001) {
                conn_dot = point;
                tmp_pt = point;
                is_show_conn_dot = true;
            }
        }

        //墙体垂直和水平吸附
        {
            float ang_conn_dis_x = glm::length(this->draw_start_dot.x - (float) event->x());
            float ang_conn_dis_y = glm::length(this->draw_start_dot.y - (float) event->y());
            if (ang_conn_dis_x < 16) {
                tmp_pt.x = this->draw_start_dot.x;
            }
            if (ang_conn_dis_y < 16) {
                tmp_pt.y = this->draw_start_dot.y;
            }
        }

        //是否显示辅助点
        if (is_show_conn_dot) {
            this->draw_image(":/images/con_wall_dot",
                             conn_dot.x - 12, conn_dot.y - 12, 24, 24);
        }

        //持续画墙
        vr->coord_ground(tmp_pt, pick, 0.0);
        this->new_wall->set_end_point(pick);
        this->new_wall->calculate_mesh();

        //重新渲染墙，让连接边及时呈现效果
        if (this->old_wall != NULL) {
            this->old_wall->calculate_mesh();
        }
        if (this->start_connect_wall != NULL) {
            this->start_connect_wall->calculate_mesh();
        }
    }

    //画房间----------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::drawhome) {
        setCursor(Qt::CrossCursor);

        this->drawhome_x1 = (float) event->x();
        this->drawhome_y1 = (float) event->y();

        this->main_scene->get_assistant_image()->fill(0);

        bool helpLineProcessed = false;
        bool is_show_conn_dot = false;

        glm::vec2 tmp_pt(this->drawhome_x1, this->drawhome_y1);
        glm::vec2 conn_dot = tmp_pt;

        //墙顶点吸附
        glm::vec2 point = this->wallPeakAdsorption(tmp_pt, 18.0f);
        if (glm::length(point - tmp_pt) > 0.001) {
            this->drawhome_x1 = point.x;
            this->drawhome_y1 = point.y;
            conn_dot = point;
            helpLineProcessed = true;
            is_show_conn_dot = true;
        }

        //墙体吸附
        if (!helpLineProcessed) {
            glm::vec2 point = this->wallLineAdsorption(tmp_pt, 18.0f);
            if (glm::length(point - tmp_pt) > 0.001) {
                helpLineProcessed = true;
                is_show_conn_dot = true;
                conn_dot = point;
                this->drawhome_x1 = point.x;
                this->drawhome_y1 = point.y;
            }
        }

        //墙顶点直角吸附
        if (!helpLineProcessed) {
            glm::vec2 point = this->wallPeakRightAngleAdsorption(tmp_pt);
            if (glm::length(point - tmp_pt) > 0.001) {
                helpLineProcessed = true;
                is_show_conn_dot = true;
                this->drawhome_x1 = point.x;
                this->drawhome_y1 = point.y;
                conn_dot = point;
            }
        }

        //是否显示补助点
        if (is_show_conn_dot) {
            this->draw_image(":/images/con_wall_dot",
                             point.x - 12, point.y - 12, 24, 24);
        }
    }

    //画房间中----------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::drawhomeing) {
        setCursor(Qt::CrossCursor);
        this->drawhome_x2 = (float) event->x();
        this->drawhome_y2 = (float) event->y();

        this->new_walla = NULL;
        this->new_wallb = NULL;
        this->new_wallc = NULL;
        this->new_walld = NULL;

        this->main_scene->get_assistant_image()->fill(0);

        glm::vec2 tmp_pt(this->drawhome_x2, this->drawhome_y2);
        glm::vec2 conn_dot = tmp_pt;

        bool helpLineProcessed = false;
        bool is_show_conn_dot = false;

        gl3d::scene *vr = this->main_scene;

        //墙顶点吸附
        glm::vec2 point = this->wallPeakAdsorption(tmp_pt, 18.0f);
        if (glm::length(point - tmp_pt) > 0.001) {
            this->drawhome_x2 = point.x;
            this->drawhome_y2 = point.y;
            conn_dot = point;
            is_show_conn_dot = true;
            helpLineProcessed = true;
        }

        //墙体吸附
        if (!helpLineProcessed) {
            glm::vec2 point = this->wallLineAdsorption(tmp_pt, 18.0f);
            if (glm::length(point - tmp_pt) > 0.001) {
                conn_dot = point;
                is_show_conn_dot = true;
                helpLineProcessed = true;
                this->drawhome_x2 = point.x;
                this->drawhome_y2 = point.y;
            }
        }

        //墙顶点直角吸附
        if (!helpLineProcessed) {
            glm::vec2 point = this->wallPeakRightAngleAdsorption(tmp_pt);
            if (glm::length(point - tmp_pt) > 0.001) {
                conn_dot = point;
                helpLineProcessed = true;
                is_show_conn_dot = true;
                this->drawhome_x2 = point.x;
                this->drawhome_y2 = point.y;
            }
        }

        {
            //set walla
            glm::vec2 picka;
            vr->coord_ground(glm::vec2((this->drawhome_x1),
                                       (this->drawhome_y1)),
                             picka, 0.0f);
            this->new_walla = new gl3d::gl3d_wall(picka, picka, gl3d::gl3d_global_param::shared_instance()->wall_thick,
                                                  2.8);
            vr->add_obj(this->wall_temp_id + 1, this->new_walla);
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
            this->new_wallb = new gl3d::gl3d_wall(pickb, pickb, gl3d::gl3d_global_param::shared_instance()->wall_thick,
                                                  2.8);
            vr->add_obj(this->wall_temp_id + 2, this->new_wallb);
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
            this->new_wallc = new gl3d::gl3d_wall(pickc, pickc, gl3d::gl3d_global_param::shared_instance()->wall_thick,
                                                  2.8);
            vr->add_obj(this->wall_temp_id + 3, this->new_wallc);
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
            this->new_walld = new gl3d::gl3d_wall(pickd, pickd, gl3d::gl3d_global_param::shared_instance()->wall_thick,
                                                  2.8);
            vr->add_obj(this->wall_temp_id + 4, this->new_walld);
            vr->coord_ground(glm::vec2((this->drawhome_x2),
                                       (this->drawhome_y2)),
                             pickd, 0.0);
            this->new_walld->set_end_point(pickd);
        }

        //处理两堵墙连接处锯齿
        gl3d_wall::combine(new_walla, new_wallb, new_walla->get_start_point());
        gl3d_wall::combine(new_wallb, new_walld, new_wallb->get_end_point());
        gl3d_wall::combine(new_wallc, new_walld, new_wallc->get_end_point());
        gl3d_wall::combine(new_wallc, new_walla, new_wallc->get_start_point());

        this->new_walld->calculate_mesh();

        //是否显示补助点
        if (is_show_conn_dot) {
            this->draw_image(":/images/con_wall_dot",
                             conn_dot.x - 12, conn_dot.y - 12, 24, 24);
        }
    }


    //选择开门位置----------------------------------------------------------------
    if (now_state == gl3d::gl3d_global_param::opendoor) {
        this->main_scene->get_assistant_image()->fill(0);
        glm::vec2 tmp_pt((float) event->x(), (float) event->y());
        //墙体吸附
        glm::vec2 wallLinePoint = this->wallLineAdsorption(tmp_pt, 25.0f);
        glm::vec2 pa = this->main_scene->project_point_to_screen(glm::vec3(0.0f));
        glm::vec2 pb = this->main_scene->project_point_to_screen(glm::vec3(1.0f, 0.0f, 0.0f));
        int _1m = (int) glm::length(pb - pa);
        this->draw_image(":/images/drawquyu",
                         wallLinePoint.x - (_1m / 2), wallLinePoint.y - _1m, _1m, _1m);

        //        doorsWindowsImages->push_back();



        //        static int tmp = 1234;
        //        static gl3d::object * o = NULL;
        //        glm::vec2 tmpcd;
        //        this->main_scene->coord_ground(wallLinePoint, tmpcd);
        //        if (o == NULL) {
        //            klm::resource::manager::shared_instance()->perform_async_res_load(new klm::resource::default_model_loader(
        //                                                                                  this->main_scene,
        //                                                                  GL3D_SCENE_DRAW_SHADOW | GL3D_SCENE_DRAW_GROUND,
        //                                                                  GL3D_OBJ_ENABLE_CHANGEMTL | GL3D_OBJ_ENABLE_PICKING),
        //                                         "000003");
        //            o = (gl3d::object *)0xff;
        //        } else {
        //            o->get_property()->position = glm::vec3(tmpcd.x, 0.0f, tmpcd.y);
        //        }
    }


    if (event->buttons() & Qt::LeftButton) {
        //获取画布上所有墙的顶点（用于吸附）
        this->getWallsPoint();

        //移动画布视角------------------------------------
        if (now_state == gl3d::gl3d_global_param::normal) {
            auto tmp_viewer = this->main_scene->watcher;
            if (tmp_viewer->get_view_mode() == tmp_viewer->top_view) {
                tmp_viewer->change_position(glm::vec3(-float(event->x() - this->tmp_point_x) / 50,
                                                      float(event->y() - this->tmp_point_y) / 50, 0.0));
                this->tmp_point_x = event->x();
                this->tmp_point_y = event->y();
                setCursor(Qt::SizeAllCursor);
                tmp_viewer->calculate_mat();
            }
        }

        //改变墙顶点位置----------------------------------
        if (now_state == gl3d::gl3d_global_param::movewall) {
            gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::movewalling;
            setCursor(Qt::SizeAllCursor);
        }
        if (now_state == gl3d::gl3d_global_param::movewalling) {
            this->connect_wall = NULL;
            this->main_scene->get_assistant_image()->fill(0);
            glm::vec2 tmp_pt((float) event->x(), (float) event->y());
            glm::vec2 tmp;
            bool show_conn_dot = false;
            glm::vec2 conn_dot = tmp_pt;

            if (!move_change_is_start_or_end) {
                tmp = this->main_scene->project_point_to_screen(
                        this->move_change_wall->get_start_point());
            } else {
                tmp = this->main_scene->project_point_to_screen(
                        this->move_change_wall->get_end_point());
            }

            //墙体垂直和水平吸附
            {
                float dis_x = glm::length(tmp.x - (float) event->x());
                float dis_y = glm::length(tmp.y - (float) event->y());
                if (dis_x < 16) {
                    tmp_pt.x = tmp.x;
                }
                if (dis_y < 16) {
                    tmp_pt.y = tmp.y;
                }
            }

            //墙顶点吸附
            if (this->wallsPoints->length() != 0) {
                for (QVector<point_wall_pair>::iterator it = this->wallsPoints->begin();
                     it != this->wallsPoints->end(); it++) {
                    float dis = glm::length(tmp_pt - (*it).first);
                    if (dis < 18.0f && (*it).second != this->move_change_wall) {
                        show_conn_dot = true;
                        tmp_pt = (*it).first;
                        conn_dot = (*it).first;
                        this->connect_wall = (*it).second;
                    }
                }
            }

            glm::vec2 pick;
            gl3d::scene *vr = this->main_scene;
            vr->coord_ground(tmp_pt, pick, 0.0);

            // reg undo and redo command
            klm::command::command_stack::shared_instance()->push(
                    new klm::command::set_wall_property(this->move_change_wall));
            if (move_change_is_start_or_end) {
                //开始点
                this->move_change_wall->set_start_point(pick);
            } else {
                //结束点
                this->move_change_wall->set_end_point(pick);
            }

            this->move_change_wall->calculate_mesh();

            //显示辅助点逻辑
            if (show_conn_dot) {
                this->draw_image(":/images/con_wall_dot",
                                 conn_dot.x - 12, conn_dot.y - 12, 24, 24);
            }
        }
    } else if (event->buttons() & Qt::LeftButton) {
        //        cout << "right move: " << event->x() << ", " << event->y() << endl;
    } else if (event->buttons() & Qt::LeftButton) {
        //        cout << "centre move: " << event->x() << ", " << event->y() << endl;
    }
}

//鼠标松开事件
void MOpenGLView::mouseReleaseEvent(QMouseEvent *event) {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    //移动墙连接吸附墙
    if (now_state == gl3d::gl3d_global_param::movewalling) {
        if (this->connect_wall != NULL) {
            this->main_scene->get_assistant_image()->fill(0);
            if (move_change_is_start_or_end) {
                gl3d_wall::combine(
                        this->move_change_wall,
                        this->connect_wall,
                        this->move_change_wall->get_start_point()
                );
            } else {
                gl3d_wall::combine(
                        this->move_change_wall,
                        this->connect_wall,
                        this->move_change_wall->get_end_point()
                );
            }
        }
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
    }

    if (now_state != gl3d::gl3d_global_param::drawwall
        && now_state != gl3d::gl3d_global_param::drawhome) {
        setCursor(Qt::ArrowCursor);
    }
}
