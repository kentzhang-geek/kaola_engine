#include "drawhomewin.h"
#include "ui_drawhomewin.h"
#include <iostream>

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "utils/gl3d_global_param.h"

using namespace std;

extern MOpenGLView * one_view;

static drawhomewin * dhw = NULL;

drawhomewin::drawhomewin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drawhomewin)
{
    ui->setupUi(this);
    dhw = this;
    ui->OpenGLCanvas->setParent(this);
}

drawhomewin::~drawhomewin()
{
    delete ui;
}

void drawhomewin::showEvent(QShowEvent * ev) {
    QWidget::showEvent(ev);

    // for test
    GL3D_INIT_SANDBOX_PATH("D:\\User\\Desktop\\KLM\\kaolao_cat_demo\\kaola_engine_demo");
    gl3d::scene::scene_property config;
    config.background_color = glm::vec3(101.0f/255.0, 157.0f/255.0f, 244.0f/255.0);
    // 绑定画布的参数
    cout << this->ui->OpenGLCanvas->size().width() << endl;
    cout << this->ui->OpenGLCanvas->size().height() << endl;
    cout << this->ui->OpenGLCanvas->width() << "vs scene" << this->ui->OpenGLCanvas->main_scene->get_width() << endl;
    cout << this->ui->OpenGLCanvas->height() << "vs scene" << this->ui->OpenGLCanvas->main_scene->get_height() << endl;
    this->ui->OpenGLCanvas->main_scene->init(&config);
    this->ui->OpenGLCanvas->main_scene->watcher->position(glm::vec3(0.0, 1.8, 0.0));
    this->ui->OpenGLCanvas->main_scene->watcher->lookat(glm::vec3(2.0, -1.2, 4.0));
    this->ui->OpenGLCanvas->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));

    this->ui->OpenGLCanvas->main_scene->set_width(
                this->ui->OpenGLCanvas->size().width());
    this->ui->OpenGLCanvas->main_scene->set_height(
                this->ui->OpenGLCanvas->size().height());
    // 加载所有要加载的模型
    gl3d::model_manager::shared_instance()->init_objs(this->ui->OpenGLCanvas->main_scene);
    // 释放模型加载器
    gl3d::model_manager::shared_instance()->release_loaders();

    this->ui->OpenGLCanvas->main_scene->prepare_buffer();

    // add a light
    general_light_source * light_1 = new general_light_source();
    glm::vec3 lightp(0.0, 1.8, 0.0);
    //    lightp += glm::vec3(2.0, -1.2, 4.0);
    lightp.y = 2.0f;
    light_1->set_location(lightp);
    light_1->set_direction(glm::vec3(0.0, -1.0, 0.0));
    light_1->set_light_type(light_1->directional_point_light);
    light_1->set_light_angle(30.0);

    this->ui->OpenGLCanvas->main_scene->get_light_srcs()->insert(1, light_1);

    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);
}

class ray_thread : public QThread {
public:
    void run() {
        GL3D_GET_RENDER_PROCESS(ray_tracer)->render();
    }
};



//切换视角按钮-temp-click
void drawhomewin::on_switch_2_clicked()
{
    static bool flag_edit = false;
    flag_edit = !flag_edit;
    if (flag_edit) {
        GL3D_SET_CURRENT_RENDER_PROCESS(editing, this->ui->OpenGLCanvas->main_scene);
    }
    else {
        GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);
    }
}

//取消所有按钮选择状态
void drawhomewin::on_draw_clear() {
    dhw->ui->drawwall_b->setCheckState(Qt::Unchecked);
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}

//画墙按钮-temp-click
extern bool test_flag_global;
void drawhomewin::on_tempdraw_clicked()
{
    static int tmp_id = 23423;
    static gl3d_wall * wall = NULL;
    glm::vec3 pos = this->ui->OpenGLCanvas->main_scene->watcher->get_current_position();
    glm::vec3 look = this->ui->OpenGLCanvas->main_scene->watcher->get_look_direction();
    glm::vec2 st = glm::vec2(pos.x, pos.z);
    glm::vec2 ed = st + glm::vec2(look.x, look.z) * 2;
    if (NULL == wall) {
        wall = new gl3d_wall(
                    st, ed, 0.5, 1.2);
        this->ui->OpenGLCanvas->main_scene->add_obj(
                    QPair<int , object *>(tmp_id++, wall));
    } else {
        wall->set_end_point(ed);
        wall->calculate_mesh();
    }

    test_flag_global = false;
}

//画墙功能按钮-change
void drawhomewin::on_drawwall_b_stateChanged(int arg1)
{
    if(!arg1) {
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
        dop->close();
        delete dop;
    } else {
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        dop = new DrawOption(this->ui->OpenGLCanvas);
        dop->show();
    }
}
