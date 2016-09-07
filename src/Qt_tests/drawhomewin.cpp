#include "drawhomewin.h"
#include "ui_drawhomewin.h"
#include <iostream>
#include <QTranslator>
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>
#include <QTextCodec>
#include "editor/gl3d_wall.h"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "resource_and_network/klm_resource_manager.h"
#include "editor/command.h"

using namespace std;

extern MOpenGLView *one_view;

static drawhomewin *dhw = NULL;

drawhomewin::drawhomewin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drawhomewin) {
    setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    dhw = this;
}

void drawhomewin::closeEvent(QCloseEvent *event) {
    event->accept();
    pugi::xml_document doc;
    pugi::xml_node nd = doc.root().append_child("sketch");
    this->ui->OpenGLCanvas->sketch->save_to_xml(nd);
    doc.save_file("sketch.xml");
    this->ui->OpenGLCanvas->close();
}

drawhomewin::~drawhomewin() {
    delete ui;
}

void drawhomewin::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);

    // for test
    GL3D_INIT_SANDBOX_PATH(GL3D_PATH_MODELS);
    gl3d::scene::scene_property config;
    config.background_color = glm::vec3(101.0f / 255.0, 157.0f / 255.0f, 244.0f / 255.0);
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
    klm::resource::manager::shared_instance()->preload_resources(this->ui->OpenGLCanvas->main_scene);

    // add a light
    general_light_source *light_1 = new general_light_source();
    glm::vec3 lightp(0.0, 1.8, 0.0);
    //    lightp += glm::vec3(2.0, -1.2, 4.0);
    lightp.y = 2.0f;
    light_1->set_location(lightp);
    light_1->set_direction(glm::vec3(0.0, -1.0, 0.0));
    light_1->set_light_type(light_1->directional_point_light);
    light_1->set_light_angle(30.0);

    this->ui->OpenGLCanvas->sketch->get_light_srcs()->insert(1, light_1);

    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);
}

class ray_thread : public QThread {
public:
    void run() {
        GL3D_GET_RENDER_PROCESS(ray_tracer)->render();
    }
};

void drawhomewin::on_switch3D_clicked() {
    static bool flag_edit = false;
    flag_edit = !flag_edit;
    if (flag_edit) {
        GL3D_SET_CURRENT_RENDER_PROCESS(editing, this->ui->OpenGLCanvas->main_scene);
    }
    else {
        GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);
    }
}

void drawhomewin::on_colse_b_clicked() {
    this->close();
}



//取消所有按钮选择状态
void drawhomewin::on_draw_clear() {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    dhw->ui->p_4->setIcon(QIcon(":/images/door"));
    dhw->ui->p_1->setIcon(QIcon(":/images/drawline"));
    dhw->ui->p_2->setIcon(QIcon(":/images/drawhome"));

    if(now_state == gl3d::gl3d_global_param::drawwall || now_state == gl3d::gl3d_global_param::drawhome) {
        dhw->dop->close();
        delete dhw->dop;
    }

    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}



//开门功能按钮
void drawhomewin::on_p_4_clicked() {
    this->on_draw_clear();
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::opendoor;
    this->ui->p_4->setIcon(QIcon(":/images/doorselectd"));
}
//画墙功能按钮
void drawhomewin::on_p_1_clicked() {
    this->on_draw_clear();
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
    this->ui->p_1->setIcon(QIcon(":/images/drawwallselectd"));
    dop = new DrawOption(this->ui->OpenGLCanvas);
    dop->show();
}
//画房间功能按钮
void drawhomewin::on_p_2_clicked() {
    this->on_draw_clear();
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhome;
    this->ui->p_2->setIcon(QIcon(":/images/drawhomeselectd"));
    dop = new DrawOption(this->ui->OpenGLCanvas);
    dop->show();
}

void drawhomewin::on_restore_clicked() {
    klm::command::command_stack::shared_instance()->redo();
}

void drawhomewin::on_undo_clicked() {
    klm::command::command_stack::shared_instance()->undo();
}
