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
#include "utils/gl3d_path_config.h"
#include "resource_and_network/klm_resource_manager.h"
#include "editor/gl3d_surface_object.h"
#include <pugixml.hpp>

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

    this->ui->OpenGLCanvas->main_scene->get_light_srcs()->insert(1, light_1);

    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);


    QVector<glm::vec3> pts;
    pts.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    pts.push_back(glm::vec3(5.0f, 0.0f, 2.0f));
    pts.push_back(glm::vec3(5.0f, 5.0f, 2.0f));
    pts.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
    klm::Surface *sfc = new klm::Surface(pts);

    pts.clear();
    pts.push_back(glm::vec3(-2.0, -2.0, 0.0f));
    pts.push_back(glm::vec3(-1.0, -2.0, 0.0f));
    pts.push_back(glm::vec3(-1.0, 0.0, 0.0f));
    pts.push_back(glm::vec3(-2.0, 0.0, 0.0f));
    klm::Surface *sub = sfc->addSubSurface(pts);
    if (sub != nullptr) {
        sub->setTranslate(glm::vec3(0.0, 0.0, 1.0));

        pts.clear();
        pts.push_back(glm::vec3(-0.4, -0.5, 0.0));
        pts.push_back(glm::vec3(0.0, -0.7, 0.0));
        pts.push_back(glm::vec3(0.4, -0.5, 0.0));
        pts.push_back(glm::vec3(0.4, 0.5, 0.0));
        pts.push_back(glm::vec3(0.0, 0.7, 0.0));
        pts.push_back(glm::vec3(-0.4, 0.5, 0.0));

        klm::Surface *ss = sub->addSubSurface(pts);
        if (ss) {
            ss->setRotation(glm::rotate(glm::mat4(1.0), 12.0f, glm::vec3(0.0, 1.0, 0.0)));
            ss->setScale(glm::vec3(0.8, 0.8, 1.0));
            ss->setTranslate(glm::vec3(0.0, 0.0, -1.0));

            pts.clear();
            pts.push_back(glm::vec3(-0.2, -0.1, 0.0));
            pts.push_back(glm::vec3(-0.1, -0.1, 0.0));
            pts.push_back(glm::vec3(-0.1, 0.0, 0.0));
            pts.push_back(glm::vec3(-0.2, 0.0, 0.0));


            klm::Surface *sss = ss->addSubSurface(pts);
            if (sss != nullptr) {
                sss->setTranslate(glm::vec3(0.0, 0.15, -2.5f));
            } else {
                cout << "failed to add last sub-surface" << endl;
            }
        }

    }

    pts.clear();
    pts.push_back(glm::vec3(1.0, 1.0, 0.0));
    pts.push_back(glm::vec3(2.0, 1.0, 0.0));
    pts.push_back(glm::vec3(2.0, 2.0, 0.0));
    pts.push_back(glm::vec3(1.0, 2.0, 0.0));

    klm::Surface *seSub = sfc->addSubSurface(pts);
    if (seSub != nullptr) {
        seSub->setScale(glm::vec3(0.8, 0.8, 1.0));
    }

    pugi::xml_document doc;
    sfc->save(doc);
    doc.save_file("/Users/gang_liu/Desktop/surface.xml");
    delete sfc;

    pugi::xml_document load_doc;
    load_doc.load_file("/Users/gang_liu/Desktop/surface.xml");
    pugi::xml_node surface_node = load_doc.select_node("//surface").node();
    if (surface_node) {
        std::cout << "surface_node.name() = " << surface_node.name() << std::endl;
    }

    Surface *mySurface = new Surface;

    if (mySurface->load(surface_node)) {
        mySurface->setTranslate(glm::vec3(0.0, 0.0, 1.0));
//        sfc->setTranslate(glm::vec3(0.0, 0.5, 1.0));

        gl3d::surface_object *oo = new gl3d::surface_object(mySurface);
        oo->get_mtls()->insert(0, new gl3d_material("___101.jpg"));
        oo->get_mtls()->insert(1, new gl3d_material("_LimeGre.jpg"));
        oo->get_mtls()->insert(2, new gl3d_material("bottle.jpg"));
        oo->get_mtls()->insert(3, new gl3d_material("58.jpg"));
        oo->get_mtls()->insert(4, new gl3d_material("123.jpg"));
        oo->get_mtls()->insert(5, new gl3d_material("___1.jpg"));
        oo->get_mtls()->insert(6, new gl3d_material("WoodVeneer.jpg"));
        oo->get_mtls()->insert(7, new gl3d_material("xin_4005.jpg"));
        oo->get_mtls()->insert(8, new gl3d_material("_2.jpg"));
        oo->get_mtls()->insert(9, new gl3d_material("_1.jpg"));
        oo->get_mtls()->insert(10, new gl3d_material("_3.jpg"));
        oo->get_mtls()->insert(11, new gl3d_material("_5.jpg"));
        oo->get_mtls()->insert(12, new gl3d_material("_17.jpg"));
        oo->get_mtls()->insert(13, new gl3d_material("_7.jpg"));
        oo->get_mtls()->insert(14, new gl3d_material("_8.jpg"));
        this->ui->OpenGLCanvas->main_scene->add_obj(QPair<int, abstract_object *>(12333, oo));
    }

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
    dhw->ui->checkBox->setCheckState(Qt::Unchecked);

    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}

//画墙功能按钮-change
void drawhomewin::on_drawwall_b_stateChanged(int arg1) {
    if (!arg1) {
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
        dop->close();
        delete dop;
    } else {
        dhw->ui->checkBox->setCheckState(Qt::Unchecked);
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawwall;
        dop = new DrawOption(this->ui->OpenGLCanvas);
        dop->show();
    }
}

//画房间功能按钮-change
void drawhomewin::on_checkBox_stateChanged(int arg1) {
    if (!arg1) {
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
        dop->close();
        delete dop;
    } else {
        dhw->ui->drawwall_b->setCheckState(Qt::Unchecked);
        gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::drawhome;
        dop = new DrawOption(this->ui->OpenGLCanvas);
        dop->show();
    }
}


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

bool test_ext_check = false;
void drawhomewin::on_pushButton_clicked() {
    // test code
    test_ext_check = !test_ext_check;
#if 0
    gl3d::scene *sce = this->ui->OpenGLCanvas->main_scene;
    gl3d::abstract_object *abs_obj = NULL;
    glm::vec2 codin(sce->get_width() / 2.0f, sce->get_height() / 2.0f);
    int obj_id = sce->get_object_id_by_coordination(codin.x, codin.y);
    if (obj_id > 0) {
        abs_obj = sce->get_obj(obj_id);
        if (abs_obj->get_obj_type() == abs_obj->type_wall) {
            gl3d_wall *w = (gl3d_wall *) abs_obj;
            glm::vec3 pt;
            glm::vec3 nor;
            glm::vec2 tmpcod;
            sce->coord_ground(codin, tmpcod);
            if (w->get_coord_on_wall(sce, codin, pt, nor)) {
                object *obj = new object((char *) (gl3d_sandbox_path + GL3D_PATH_SEPRATOR + "armchair.3ds").c_str());
                obj->set_control_authority(GL3D_OBJ_ENABLE_DEL | GL3D_OBJ_ENABLE_PICKING);
                obj->get_property()->scale_unit = gl3d::scale::mm;
                obj->set_render_authority(GL3D_SCENE_DRAW_NORMAL | GL3D_SCENE_DRAW_IMAGE | GL3D_SCENE_DRAW_SHADOW);
                obj->pre_scale();
                obj->merge_meshes();
                obj->recalculate_normals();
                obj->convert_left_hand_to_right_hand();
                obj->get_property()->position = pt;
                obj->get_property()->rotate_mat = gl3d::math::get_rotation_from_a_to_b(glm::vec3(0.0, 1.0, 0.0), nor);
                obj->buffer_data();
                if (NULL != this->ui->OpenGLCanvas->main_scene->get_obj(23148)) {
                    gl3d::object * tmpobj = (gl3d::object *)this->ui->OpenGLCanvas->main_scene->get_obj(23148);
                    this->ui->OpenGLCanvas->main_scene->delete_obj(23148);
                    delete tmpobj;
                }
                this->ui->OpenGLCanvas->main_scene->add_obj(QPair<int, gl3d::object *>(23148, obj));
            }
        }
    }
#endif

#if 0
    gl3d::scene *sce = this->ui->OpenGLCanvas->main_scene;
    gl3d::abstract_object *abs_obj = NULL;
    glm::vec2 coda(sce->get_width() / 2.0f, sce->get_height() / 2.0f);
    glm::vec2 codb(sce->get_width() / 4.0f * 3.0f, sce->get_height() / 4.0f * 3.0f);
    int obj_id = sce->get_object_id_by_coordination(coda.x, coda.y);
    if (obj_id > 0) {
        abs_obj = sce->get_obj(obj_id);
        if (abs_obj->get_obj_type() == abs_obj->type_wall) {
            gl3d_wall *w = (gl3d_wall *) abs_obj;
            if (obj_id != sce->get_object_id_by_coordination(codb.x, codb.y)) {
                return; //check obj
            }
            glm::vec3 holea, holeb;
            holea = math::convert_vec2_to_vec3(w->get_start_point());
            holeb = math::convert_vec2_to_vec3(w->get_end_point());
            glm::vec3 dir = holeb - holea;
            dir = glm::normalize(dir);
            float xlen = w->get_length();
            holeb = holea + dir * 0.75f * xlen;
            holea = holea + dir * 0.25f * xlen;
            holea.y = 0.75f;
            holeb.y = 2.25f;
            hole * h = new hole(w, holea, holeb);
            w->calculate_mesh();
        }
    }
#endif
}
