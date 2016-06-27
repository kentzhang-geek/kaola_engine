#include "mainwindow.h"
#include <iostream>
#include "ui_mainwindow.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_obj_authority.h"

using namespace std;

extern MOpenGLView * one_view;

QTextEdit * qtout = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->setParent(this);
    connect(this, SIGNAL(sig1()), this, SLOT(on_sig1()));

    qtout = this->ui->tout;
}

void MainWindow::showEvent(QShowEvent * ev) {
    QMainWindow::showEvent(ev);

    // for test
    GL3D_INIT_SANDBOX_PATH("D:\\User\\Desktop\\KLM\\kaolao_cat_demo\\kaola_engine_demo");
    gl3d::scene::scene_property config;
    config.background_color = glm::vec3(101.0f/255.0, 157.0f/255.0f, 244.0f/255.0);
    // 绑定画布的参数
    cout << this->ui->openGLWidget->size().width() << endl;
    cout << this->ui->openGLWidget->size().height() << endl;
    cout << this->ui->openGLWidget->width() << "vs scene" << this->ui->openGLWidget->main_scene->width << endl;
    cout << this->ui->openGLWidget->height() << "vs scene" << this->ui->openGLWidget->main_scene->height << endl;
    this->ui->openGLWidget->main_scene->init(&config);
    this->ui->openGLWidget->main_scene->watcher->position(glm::vec3(24.0, 12.0, 7.0));
    this->ui->openGLWidget->main_scene->watcher->lookat(glm::vec3(2.0, 4.0, -1.2));
    this->ui->openGLWidget->main_scene->watcher->headto(glm::vec3(0.0, 0.0, 1.0));

    // 加载所有要加载的模型
    gl3d::model_manager::shared_instance()->init_objs(this->ui->openGLWidget->main_scene);
    // 释放模型加载器
    gl3d::model_manager::shared_instance()->release_loaders();

    this->ui->openGLWidget->main_scene->prepare_buffer();

    GL3D_SET_CURRENT_RENDER_PROCESS(moving);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    exit(0);
}

void MainWindow::on_sig1() {
    ui->tout->setText(ui->tout->toPlainText() + "qqq");
    ui->lcdn->setHexMode();
    ui->lcdn->display(ui->lcdn->intValue() + 1);
}

void MainWindow::on_qqq_clicked()
{
    this->ui->openGLWidget->main_scene->watcher->go_rotate(2.0);
    GL3D_GET_CURRENT_RENDER_PROCESS()->invoke();
}
