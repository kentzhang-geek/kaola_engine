#include "mainwindow.h"
#include <iostream>
#include "ui_mainwindow.h"
#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "editor/klm_surface.h"

using namespace std;
using namespace klm;

extern MOpenGLView * one_view;

QTextEdit * qtout = NULL;
static gl3d_wall * wall = NULL;


gl3d::object * test_obj() {
    gl3d::obj_points pts[8];
    memset(pts, 0, sizeof(pts));

    GLfloat tmp[8][5] = {
    {-2.69258,-2.5,0, 0, 0},
    {2.69258,-2.5,0, 5.38516, 0},
    {2.69258,2.5,0, 5.38516, 5},
    {-2.69258,2.5,0, 0, 5},
    {-0.5,-0.5,0, 2.19258, 2},
    {0.5,-0.5,0, 3.19258, 2},
    {0.5,0.5,0, 3.19258, 3},
    {-0.5,0.5,0, 2.19258, 3}};

    for (int i = 0; i < 8; i++) {
        pts[i].vertex_x = tmp[i][0];
        pts[i].vertex_y = tmp[i][1];
        pts[i].vertex_z = tmp[i][2];
        pts[i].texture_x = tmp[i][3];
        pts[i].texture_y = tmp[i][4];
    }

    GLushort indexes[24] = {
        0,4,3,
        4,0,1,
        4,1,5,
        5,1,6,
        3,7,2,
        7,3,4,
        2,7,6,
        2,6,1
    };

    gl3d::object * obj = new gl3d::object(pts, 8, indexes, 24);

    obj->get_mtls()->clear();
    obj->get_mtls()->insert(0, new gl3d::gl3d_material("Color_A1.jpg"));
    obj->set_repeat(true);

    obj->get_property()->scale_unit = gl3d::scale::m;
    obj->get_property()->authority = GL3D_OBJ_ENABLE_ALL;
    obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;

    return obj;
}

gl3d::object * test_obj2() {
    gl3d::obj_points pts[8];
    memset(pts, 0, sizeof(pts));

    GLfloat tmp[4][5] = {
        {-0.5,-0.5,-5.87747e-39, 0, 0},
        {0.5,-0.5,-5.87747e-39, 1, 0},
        {0.5,0.5,-5.87747e-39, 1, 1},
        {-0.5,0.5,-5.87747e-39, 0, 1}}
  ;

    for (int i = 0; i < 4; i++) {
        pts[i].vertex_x = tmp[i][0];
        pts[i].vertex_y = tmp[i][1];
        pts[i].vertex_z = tmp[i][2];
        pts[i].texture_x = tmp[i][3];
        pts[i].texture_y = tmp[i][4];
    }

    GLushort indexes[6] = {
        1,3,0,
        3,1,2,
    };

    gl3d::object * obj = new gl3d::object(pts, 4, indexes, 6);

    obj->get_mtls()->clear();
    obj->get_mtls()->insert(0, new gl3d::gl3d_material("bottle.jpg"));
    obj->set_repeat(true);

    obj->get_property()->scale_unit = gl3d::scale::m;
    obj->get_property()->authority = GL3D_OBJ_ENABLE_ALL;
    obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;

    return obj;
}

gl3d::object * test_obj3() {
    QVector<glm::vec3> coords;
    coords.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    coords.push_back(glm::vec3(5.0f, 0.0f, 2.0f));
    coords.push_back(glm::vec3(5.0f, 5.0f, 2.0f));
    coords.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
    klm::Surface * surface = new klm::Surface(coords);

    QVector<glm::vec3> subCoords;
    subCoords.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    subCoords.push_back(glm::vec3( 0.5f, -0.5f, 0.0f));
    subCoords.push_back(glm::vec3( 0.5f,  0.5f, 0.0f));
    subCoords.push_back(glm::vec3(-0.5f,  0.5f, 0.0f));
    if(surface->addSubSurface(subCoords)){
        std::cout<<"SubSurface added"<<std::endl;
    }

    GLfloat * tmp_data = NULL;
    int pts_len;
    gl3d::obj_points * pts = NULL;
    surface->getRenderingVertices(tmp_data, pts_len);
    pts = (gl3d::obj_points *)
            malloc(sizeof(gl3d::obj_points) * pts_len);
    memset(pts, 0, sizeof(gl3d::obj_points) * pts_len);
    for (int i = 0; i < pts_len; i++) {
        pts[i].vertex_x = tmp_data[i * 5 + 0];
        pts[i].vertex_y = tmp_data[i * 5 + 1];
        pts[i].vertex_z = tmp_data[i * 5 + 2];
        pts[i].texture_x = tmp_data[i * 5 + 3];
        pts[i].texture_y = 1.0f - tmp_data[i * 5 + 4];
    }

    GLushort * idxes = NULL;
    int idx_len;
    surface->getRenderingIndicies(idxes, idx_len);

    gl3d::object * obj = new gl3d::object(pts, pts_len, idxes, idx_len);

    free(pts);
    free(idxes);

//    auto sub = surface->getSubSurface(0);
    obj->get_mtls()->clear();
    obj->get_mtls()->insert(0, new gl3d::gl3d_material("bottle.jpg"));
    obj->get_mtls()->insert(1, new gl3d::gl3d_material("_35.jpg"));
    obj->set_repeat(true);

    obj->get_property()->scale_unit = gl3d::scale::m;
    obj->get_property()->authority = GL3D_OBJ_ENABLE_ALL;
    obj->get_property()->draw_authority = GL3D_SCENE_DRAW_NORMAL;

    delete surface;
    return obj;
}

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
    cout << this->ui->openGLWidget->width() << "vs scene" << this->ui->openGLWidget->main_scene->get_width() << endl;
    cout << this->ui->openGLWidget->height() << "vs scene" << this->ui->openGLWidget->main_scene->get_height() << endl;
    this->ui->openGLWidget->main_scene->init(&config);
    this->ui->openGLWidget->main_scene->watcher->position(glm::vec3(0.0, 1.8, 0.0));
    this->ui->openGLWidget->main_scene->watcher->lookat(glm::vec3(2.0, -1.2, 4.0));
    this->ui->openGLWidget->main_scene->watcher->headto(glm::vec3(0.0, 1.0, 0.0));

    this->ui->openGLWidget->main_scene->set_width(
                this->ui->openGLWidget->size().width());
    this->ui->openGLWidget->main_scene->set_height(
                this->ui->openGLWidget->size().height());
    // 加载所有要加载的模型
    gl3d::model_manager::shared_instance()->init_objs(this->ui->openGLWidget->main_scene);
    // 释放模型加载器
    gl3d::model_manager::shared_instance()->release_loaders();

    this->ui->openGLWidget->main_scene->prepare_buffer();

    // add a light
    general_light_source * light_1 = new general_light_source();
    glm::vec3 lightp(24.0, 7.0, 12.0);
//    lightp += glm::vec3(2.0, -1.2, 4.0);
    lightp.y = 2.0f;
    light_1->set_location(lightp);
    light_1->set_direction(glm::vec3(0.0, -1.0, 0.0));
    light_1->set_light_type(light_1->directional_point_light);
    light_1->set_light_angle(30.0);

    this->ui->openGLWidget->main_scene->get_light_srcs()->insert(1, light_1);

    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->openGLWidget->main_scene);
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

class ray_thread : public QThread {
public:
    void run() {
        GL3D_GET_RENDER_PROCESS(ray_tracer)->render();
    }
};

void MainWindow::on_qqq_clicked()
{
    static bool flag_edit = false;
    flag_edit = !flag_edit;
    if (flag_edit) {
        GL3D_SET_CURRENT_RENDER_PROCESS(editing, this->ui->openGLWidget->main_scene);
    }
    else {
        GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->openGLWidget->main_scene);
    }
}

extern bool test_flag_global;
void MainWindow::on_pushButton_2_clicked()
{
    static int tmp_id = 23423;
    glm::vec3 pos = this->ui->openGLWidget->main_scene->watcher->get_current_position();
    glm::vec3 look = this->ui->openGLWidget->main_scene->watcher->get_look_direction();
    glm::vec2 st = glm::vec2(pos.x, pos.z);
//    st = st / gl3d::scale::shared_instance()->get_global_scale();
    glm::vec2 ed = st + glm::vec2(2.0, 2.0);
    if (NULL == wall) {
        wall = new gl3d_wall(
                    st, ed, 0.3, 1.8);
        this->ui->openGLWidget->main_scene->add_obj(
                    QPair<int , object *>(tmp_id++, wall));
    }
    else {
        wall->set_start_point(st + glm::vec2(0.0, 0.5));
        wall->set_end_point(st);
        wall->calculate_mesh();
    }

    test_flag_global = false;
}

void MainWindow::on_pushButton_3_clicked()
{
    // add a furniture
    glm::vec3 vc = this->ui->openGLWidget->main_scene->watcher->get_current_position();
    model_manager::shared_instance()->add_obj_to_scene(
                this->ui->openGLWidget->main_scene,
                (char *)"sidetable.3ds",
                glm::vec2(vc.x, vc.z));
}

void MainWindow::on_pushButton_4_clicked()
{
    gl3d::viewer * vr = this->ui->openGLWidget->main_scene->watcher;
    vr->set_top_view_size(vr->get_top_view_size() - 1.0f);
}

void MainWindow::on_pushButton_5_clicked()
{
    glm::vec3 locat = this->ui->openGLWidget->main_scene->watcher->get_current_position();
}

void MainWindow::on_pushButton_6_clicked()
{
    glm::vec3 locat = this->ui->openGLWidget->main_scene->watcher->get_current_position();
    glm::vec2 pick;
    gl3d::scene * vr = this->ui->openGLWidget->main_scene;

    // set wall
    vr->coord_ground(glm::vec2(0.75,0.5), pick, 0.0);
    wall->set_start_point(pick);
    vr->coord_ground(glm::vec2(0.5,0.75), pick, 0.0);
    wall->set_end_point(pick);
    wall->calculate_mesh();
}

void MainWindow::on_pushButton_7_clicked()
{
//    this->ui->openGLWidget->main_scene->add_obj(QPair<int , object *>(6666, test_obj()));
//    this->ui->openGLWidget->main_scene->add_obj(QPair<int , object *>(8888, test_obj2()));
    this->ui->openGLWidget->main_scene->add_obj(QPair<int , object *>(9999, test_obj3()));
}
