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
    gl3d::scene::scene_property config;
    config.background_color = glm::vec4(101.0f / 255.0, 157.0f / 255.0f, 244.0f / 255.0, 1.0f);
//    config.background_color = glm::vec4(0.0f);
    // 绑定画布的参数
    cout << this->ui->glview->size().width() << endl;
    cout << this->ui->glview->size().height() << endl;
    cout << this->ui->glview->width() << "vs scene" << this->ui->glview->main_scene->get_width() << endl;
    cout << this->ui->glview->height() << "vs scene" << this->ui->glview->main_scene->get_height() << endl;
    this->ui->glview->main_scene->init(&config);
//    this->ui->glview->main_scene->watcher->position(glm::vec3(0,0,-150.0));
    this->ui->glview->main_scene->watcher->position(glm::vec3(0,0,0.0));
    this->ui->glview->main_scene->watcher->lookat(glm::vec3(0,0,1));
    this->ui->glview->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));

    this->ui->glview->main_scene->set_width(
            this->ui->glview->size().width());
    this->ui->glview->main_scene->set_height(
            this->ui->glview->size().height());

    // create space manager with 4096 cubes
    ui->glview->main_scene->spaceManager = new gl3d::SpaceManager();
    ui->glview->main_scene->spaceManager->initWithDepthAndSize(5, glm::vec3(160.0f), glm::vec3(-160.0f));
    ui->glview->main_scene->spaceManager->bufferData();
//    gl3d_global_param::shared_instance()->maxCulledObjNum = 1000;   // 1000 models in screen
//    gl3d_global_param::shared_instance()->maxInsPerDraw = 200;      // 200 models per draw

    // 加载所有要加载的模型
    gl3d::model_manager::shared_instance()->init_objs(ui->glview->main_scene);
    // first cull models
    ui->glview->main_scene->spaceManager->cullObjects(ui->glview->main_scene->watcher,
                                                      gl3d_global_param::shared_instance()->maxCulledObjNum);

    qrand();
    qrand();
    // add lights
    for (int i = 0; i < 100; i++) {
        general_light_source *light = new general_light_source();
        glm::vec3 pos;
        pos.x = (qrand() % 100) / 10.0f;
        pos.y = (qrand() % 100) / 10.0f;
        pos.z = (qrand() % 100) / 10.0f;
        light->set_location(pos);
        light->set_direction(glm::vec3(0.0, -1.0, 0.0));
        light->set_light_type(light->point_light);
        light->set_light_angle(30.0);
        this->ui->glview->main_scene->get_light_srcs()->insert(i, light);
    }

    GL3D_SET_CURRENT_RENDER_PROCESS(day, this->ui->glview->main_scene);
}

void DemoMainWindow::on_confirm_clicked()
{
    glm::vec3 newpos(
            ui->lin_x->text().toFloat(),
            ui->lin_y->text().toFloat(),
            ui->lin_z->text().toFloat()
    );
    ui->glview->main_scene->watcher->set_current_position(newpos);
    ui->glview->main_scene->watcher->calculate_mat();
}


void DemoMainWindow::on_btn_day_clicked()
{
    this->ui->glview->main_scene->get_property()->background_color= glm::vec4(101.0f / 255.0, 157.0f / 255.0f, 244.0f / 255.0, 1.0f);
    GL3D_SET_CURRENT_RENDER_PROCESS(day, this->ui->glview->main_scene);
}

void DemoMainWindow::on_btn_night_clicked()
{
    this->ui->glview->main_scene->get_property()->background_color = glm::vec4(0.0f);
    GL3D_SET_CURRENT_RENDER_PROCESS(night, this->ui->glview->main_scene);
}

void DemoMainWindow::on_cb_space_clicked() {
    if (ui->cb_space->checkState() == Qt::Checked) {
        gl3d_global_param::shared_instance()->drawSpace = true;
    } else if (ui->cb_space->checkState() == Qt::Unchecked) {
        gl3d_global_param::shared_instance()->drawSpace = false;
    }
}

void DemoMainWindow::on_cb_geo_clicked() {
    if (ui->cb_geo->checkState() == Qt::Unchecked) {
        gl3d_global_param::shared_instance()->drawGeo = false;
    } else if (ui->cb_geo->checkState() == Qt::Checked) {
        gl3d_global_param::shared_instance()->drawGeo = true;
    }

}
