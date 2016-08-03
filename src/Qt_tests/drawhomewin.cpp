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
#include "editor/klm_surface.h"
#include "editor/gl3d_surface_object.h"

using namespace std;

extern MOpenGLView * one_view;

static drawhomewin * dhw = NULL;

drawhomewin::drawhomewin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drawhomewin)
{
    ui->setupUi(this);
    dhw = this;
}

drawhomewin::~drawhomewin()
{
    delete ui;
}

QMap<unsigned int, gl3d_material *> * test_mtl = NULL;

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

#if 1
    // test code , do not commit to repo
    test_mtl = new QMap<unsigned int, gl3d_material *>();
    test_mtl->insert(0, new gl3d_material("FieldSquareTile.jpg"));
    test_mtl->insert(1, new gl3d_material("DSCN6941.jpg"));
    test_mtl->insert(2, new gl3d_material("_3.jpg"));
    test_mtl->insert(3, new gl3d_material("_5.jpg"));
    test_mtl->insert(4, new gl3d_material("_19.jpg"));
    test_mtl->insert(5, new gl3d_material("_38.jpg"));
    test_mtl->insert(6, new gl3d_material("_Water_T.jpg"));
//    for (auto it = test_mtl->begin();
//         it != test_mtl->end();
//         it++) {
//        it.value()->colors.clear(); // no colors for test mtl
//    }
    QVector<glm::vec3 > points;
    klm::Surface * sfc;
    points.clear();
    points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    points.push_back(glm::vec3(5.0f, 0.0f, 0.0f));
    points.push_back(glm::vec3(5.0f, 5.0f, 0.0f));
    points.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
    sfc = new klm::Surface(points);

    points.clear();
    points.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    points.push_back(glm::vec3( 0.5f, -0.5f, 0.0f));
    points.push_back(glm::vec3( 0.5f,  0.5f, 0.0f));
    points.push_back(glm::vec3(-0.5f,  0.5f, 0.0f));
    sfc->addSubSurface(points);
    sfc->getSubSurface(0)->setHeightToParent(2.0f);

    surface_object * o = new surface_object(sfc);
    o->get_mtls()->insert(0, new gl3d_material("FieldSquareTile.jpg"));
    o->get_mtls()->insert(1, new gl3d_material("DSCN6941.jpg"));
    o->get_mtls()->insert(2, new gl3d_material("_3.jpg"));
    o->get_mtls()->insert(3, new gl3d_material("_5.jpg"));
    o->get_mtls()->insert(4, new gl3d_material("_19.jpg"));
    this->ui->OpenGLCanvas->main_scene->add_obj(QPair<int, gl3d::abstract_object *>(1233, o));
#endif
}

class ray_thread : public QThread {
public:
    void run() {
        GL3D_GET_RENDER_PROCESS(ray_tracer)->render();
    }
};

//取消所有按钮选择状态
void drawhomewin::on_draw_clear() {
    dhw->ui->drawwall_b->setCheckState(Qt::Unchecked);
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
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

void drawhomewin::on_switch3D_clicked()
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
