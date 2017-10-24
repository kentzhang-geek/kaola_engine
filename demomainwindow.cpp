#include <QtCore/QDir>
#include <include/utils/gl3d_global_param.h>
#include <include/kaola_engine/gl3d.hpp>
#include <include/kaola_engine/model_manager.hpp>
#include "demomainwindow.h"
#include "ui_demomainwindow.h"

DemoMainWindow::DemoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DemoMainWindow) {
    ui->setupUi(this);
    qDebug() << QDir::currentPath();
    gl3d::render_process_manager::get_shared_instance()->set_current_process("normal");
}

DemoMainWindow::~DemoMainWindow()
{
    delete ui;
}

void DemoMainWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    // for test
    gl3d::scene::scene_property config;
    config.background_color = glm::vec3(101.0f / 255.0, 157.0f / 255.0f, 244.0f / 255.0);
    // 绑定画布的参数
    cout << this->ui->glview->size().width() << endl;
    cout << this->ui->glview->size().height() << endl;
    cout << this->ui->glview->width() << "vs scene" << this->ui->glview->main_scene->get_width() << endl;
    cout << this->ui->glview->height() << "vs scene" << this->ui->glview->main_scene->get_height() << endl;
    this->ui->glview->main_scene->init(&config);
    this->ui->glview->main_scene->watcher->position(glm::vec3(0,0,-3));
    this->ui->glview->main_scene->watcher->lookat(glm::vec3(0,0,1));
    this->ui->glview->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));

    this->ui->glview->main_scene->set_width(
            this->ui->glview->size().width());
    this->ui->glview->main_scene->set_height(
            this->ui->glview->size().height());
    // 加载所有要加载的模型
    gl3d::model_manager::shared_instance()->init_objs(ui->glview->main_scene);

    // add a light
    general_light_source *light_1 = new general_light_source();
    glm::vec3 lightp(0.0, 1.8, 0.0);
    lightp.y = 2.0f;
    light_1->set_location(lightp);
    light_1->set_direction(glm::vec3(0.0, -1.0, 0.0));
    light_1->set_light_type(light_1->directional_point_light);
    light_1->set_light_angle(30.0);

    this->ui->glview->main_scene->get_light_srcs()->insert(1, light_1);

    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->glview->main_scene);

    // test
    ui->glview->main_scene->spaceManager = new gl3d::SpaceManager();
    ui->glview->main_scene->spaceManager->initWithDepthAndSize(4, glm::vec3(100.0f), glm::vec3(-100.0f));
}
