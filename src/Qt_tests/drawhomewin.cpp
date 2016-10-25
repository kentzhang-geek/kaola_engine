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
#include <include/resource_and_network/pack_tool.h>
#include <include/resource_and_network/network_tool.h>
#include "editor/gl3d_wall.h"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "resource_and_network/klm_resource_manager.h"
#include "editor/command.h"
#include "editor/style_package.h"
#include "resource_and_network/global_info.h"

using namespace std;

extern MOpenGLView *one_view;

static drawhomewin *dhw = NULL;

drawhomewin::drawhomewin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drawhomewin) {
    setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    dhw = this;

    QWebEngineView * wweb = this->findChild<QWebEngineView *>("web_goods");
    this->web = wweb;
    this->on_load_may_need_reload_web = true;
//    QWidget * qt = new QWidget(NULL);
//    qt->thread()->setPriority(QThread::HighestPriority);
//    qt->resize(410, 900);
//    this->web->setParent(qt);
//    qt->setWindowFlags(Qt::FramelessWindowHint);
//    qt->show();
//    qt->setFixedWidth(410);
//    this->web->setFixedWidth(400);
//    this->web->resize(410, 900);
    this->web->load(QUrl(KLM_WEB_GOODS_URL));
    this->web->show();
    this->channel = new QWebChannel(this);
    web->page()->setWebChannel(this->channel);
    this->channel->registerObject(QStringLiteral("draw_win"), this);
    connect(web, SIGNAL(loadFinished(bool)), this, SLOT(webload_finished(bool)));

    this->web->thread()->setPriority(QThread::HighestPriority);
}

void drawhomewin::webload_finished(bool isok) {
    if (isok) {
//        if (this->on_load_may_need_reload_web) {
//            this->web->reload();
//            this->on_load_may_need_reload_web = false;
//        }
//        else {
            qDebug("goods web ok");
            QString code = QString::fromLocal8Bit("qtinit()");
            web->page()->runJavaScript(code);
//        }
    }
    else {
        qDebug("goods web fucked");
    }
}

void drawhomewin::closeEvent(QCloseEvent *event) {
    event->accept();
    this->ui->OpenGLCanvas->close();
}

drawhomewin::~drawhomewin() {
    delete ui;
}

void drawhomewin::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);

    // for test
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

    // test with furniture
//    this->ui->OpenGLCanvas->sketch->add_furniture("000001", glm::vec3(2.0, 0.0, 2.0));
//    this->ui->OpenGLCanvas->sketch->add_furniture("000002", glm::vec3(-2.0, 0.0, -2.0));

    // load design if there is one
    this->load_sketch();
}

void drawhomewin::load_sketch() {
    // down load sketch TODO : test
    if (klm::info::shared_instance()->d_p_path.size() >= 1) {
        QString filename;
        switch (gl3d_global_param::shared_instance()->sketch_mode) {
            case gl3d_global_param::new_plan:{
                break;
            }
            case gl3d_global_param::change_design:{
                filename = "design.chd";
                break;
            }
            case gl3d_global_param::change_plan: {
                filename = "design.chp";
                break;
            }
            case gl3d_global_param::new_design: {
                filename = "design.chp";
                break;
            }
            default:break;
        }
        if (klm::network::call_web_download(
                QString(KLM_SERVER_URL) + klm::info::shared_instance()->d_p_path,
                                            "tmp\\dptmp.7z", "")) {
            klm::pack_tool packer;
            QEventLoop loop;
            connect(&packer, SIGNAL(finished()), &loop, SLOT(quit()));
            packer.unpack("tmp\\dptmp.7z", "");
            loop.exec();

            pugi::xml_document doc;
            doc.load_file("tmp\\sketch.xml");
            this->ui->OpenGLCanvas->sketch->load_from_xml(doc.root().child("sketch"));
        }

        switch (gl3d_global_param::shared_instance()->sketch_mode) {
            // clear sketch if needed
            case gl3d_global_param::change_plan: {
                this->ui->OpenGLCanvas->sketch->clear_sketch();
                break;
            }
            case gl3d_global_param::new_design: {
                this->ui->OpenGLCanvas->sketch->clear_sketch();
                break;
            }
            default:break;
        }
        // remove temp file
        QFile::remove("tmp/sketch.xml");
        QFile::remove("tmp/style.xml");
        QFile::remove("tmp/dptmp.7z");
    }
}

class ray_thread : public QThread {
public:
    void run() {
        GL3D_GET_RENDER_PROCESS(ray_tracer)->render();
    }
};

//清空切换视角按钮背景颜色
void drawhomewin::clear_bg_color() {
    this->ui->pushButton->setStyleSheet(
                "border-radius: 0;"
                "background-color: rgb(240, 241, 243);"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(66, 66, 66);"
                );
    this->ui->switch3D->setStyleSheet(
                "border-radius: 0;"
                "background-color: rgb(240, 241, 243);"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(66, 66, 66);"
                "border-left-color: rgb(240, 241, 243);"
                "border-right-color: rgb(240, 241, 243);"
                );
    this->ui->pushButton_2->setStyleSheet(
                "border-radius: 0;"
                "background-color: rgb(240, 241, 243);"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(66, 66, 66);"
                );
}

//修改户型视角
void drawhomewin::on_pushButton_clicked() {
    this->clear_bg_color();
    this->ui->pushButton->setStyleSheet(
                "background-color: rgb(55, 59, 66);"
                "border-radius: 0;"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(255, 255, 255);"
                );
    GL3D_SET_CURRENT_RENDER_PROCESS(editing, this->ui->OpenGLCanvas->main_scene);
}
//装修视角
void drawhomewin::on_switch3D_clicked() {
    this->clear_bg_color();
    this->ui->switch3D->setStyleSheet(
                "background-color: rgb(55, 59, 66);"
                "border-radius: 0;"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(255, 255, 255);"
                "border-left-color: rgb(240, 241, 243);"
                "border-right-color: rgb(240, 241, 243);"
                );
    GL3D_SET_CURRENT_RENDER_PROCESS(has_post, this->ui->OpenGLCanvas->main_scene);
}
//3D浏览视角
void drawhomewin::on_pushButton_2_clicked() {
    this->clear_bg_color();
    this->ui->pushButton_2->setStyleSheet(
                "background-color: rgb(55, 59, 66);"
                "border-radius: 0;"
                "border:1px solid rgb(66, 66, 66);"
                "color: rgb(255, 255, 255);"
                );
}

//完成按钮
void drawhomewin::on_colse_b_clicked() {
    this->close();
}

//取消所有按钮选择状态
void drawhomewin::on_draw_clear() {
    auto now_state = gl3d::gl3d_global_param::shared_instance()->current_work_state;

    dhw->ui->p_4->setIcon(QIcon(":/images/door"));
    dhw->ui->p_1->setIcon(QIcon(":/images/drawline"));
    dhw->ui->p_2->setIcon(QIcon(":/images/drawhome"));
    dhw->ui->p_7->setIcon(QIcon(":/images/window"));

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

//恢复
void drawhomewin::on_restore_clicked() {
    klm::command::command_stack::shared_instance()->redo();
}

//撤销
void drawhomewin::on_undo_clicked() {
    klm::command::command_stack::shared_instance()->undo();
}

void drawhomewin::on_p_7_clicked() {
    this->on_draw_clear();
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d_global_param::openwindow;
    this->ui->p_7->setIcon(QIcon(":/images/window_selected"));
}

void drawhomewin::on_p_11_clicked()
{
    // set rule tool
    gl3d_global_param::shared_instance()->current_work_state = gl3d_global_param::work_state::ruling;
}

void drawhomewin::on_p_12_clicked() {
    // set area tool
    gl3d_global_param::shared_instance()->current_work_state = gl3d_global_param::work_state::draw_area;
    auto new_pts = new QVector<glm::vec3>;
    this->ui->OpenGLCanvas->user_data.insert("area_points", new_pts);
    this->ui->OpenGLCanvas->user_data.insert("current_point", new glm::vec3(0.0f));
}

static bool flag_save = false;

void drawhomewin::on_save_b_clicked() {
    if (flag_save)
        return;

    QDir tmp("tmp");
    if (!tmp.exists()) {
        QDir::current().mkdir("tmp");
    }

    pugi::xml_document doc;
    pugi::xml_node rootnode = doc.root();
    rootnode = rootnode.append_child("sketch");
    this->ui->OpenGLCanvas->sketch->save_to_xml(rootnode);
    doc.save_file("tmp/sketch.xml");

    pugi::xml_document doc2;
    pugi::xml_node root2 = doc2.root();
    root2 = root2.append_child("style");
    klm::design::common_style_package st;
    st.read_from_scheme(this->ui->OpenGLCanvas->sketch);
    st.save_to_xml(root2);
    doc2.save_file("tmp/style.xml");

    klm::pack_tool * newpack = new klm::pack_tool;
    QVector<QString> fs;
    fs.append("tmp/sketch.xml");
    fs.append("tmp/style.xml");
    newpack->pack(fs, "tmp/design.chd");
    connect(newpack, SIGNAL(finished()), this, SLOT(save_ok()));

    flag_save = true;
}

void drawhomewin::save_ok() {
    flag_save = false;

    gl3d::viewer copy = *this->ui->OpenGLCanvas->main_scene->watcher;
    this->ui->OpenGLCanvas->main_scene->watcher->set_top_view();
    this->ui->OpenGLCanvas->main_scene->watcher->calculate_mat();
    QImage * img = this->ui->OpenGLCanvas->main_scene->draw_screenshot();
    img->save("icon.jpg");
    delete img;
    *this->ui->OpenGLCanvas->main_scene->watcher = copy;

    // choose save action
    switch (gl3d_global_param::shared_instance()->sketch_mode) {
        case gl3d_global_param::new_plan: {
            this->new_plan();
            break;
        }
        case gl3d_global_param::new_design: {
            this->new_design();
            break;
        }
        case gl3d_global_param::change_design: {
            this->change_design();
            break;
        }
        case gl3d_global_param::change_plan: {
            this->change_plan();
            break;
        }
        default: {
            qDebug("sketch mode save failed");
            break;
        }
    }

    QFile::remove("tmp/sketch.xml");
    QFile::remove("tmp/style.xml");
    QFile::remove("tmp/design.chd");
    QFile::remove("icon.jpg");

    return;
}

void drawhomewin::new_plan() {
    QString pdn;

    QMessageBox info1;
    info1.setWindowTitle(tr("uploading"));
    info1.setText(tr("uploading"));
    info1.exec();

    QJsonDocument doc;
    QString plan_pdn;
    QString design_pdn;

    doc = klm::network::call_web_new(klm::info::shared_instance()->housename,
                                     klm::info::shared_instance()->house_url, KLM_SERVER_URL_NEW_PLAN);
    int tval = doc.object().find("results").value().toArray()[0].toObject().value("resource_id").toInt();
    klm::info::shared_instance()->plan_id = QString::asprintf("%d", tval);
    plan_pdn = doc.object().find("results").value().toArray()[0].toObject().value("path").toString();
    doc = klm::network::call_web_new(klm::info::shared_instance()->housename,
                                     klm::info::shared_instance()->house_url, KLM_SERVER_URL_NEW_DESIGN);
    tval = doc.object().find("results").value().toArray()[0].toObject().value("resource_id").toInt();
    klm::info::shared_instance()->design_id = QString::asprintf("%d", tval);
    design_pdn = doc.object().find("results").value().toArray()[0].toObject().value("path").toString();

    bool upload_flag = true;
    QString id;
    id = klm::info::shared_instance()->design_id;
    if (id.size() >= 1) {
        doc = klm::network::call_web_file_upload(id, "tmp/design.chd", "design.chd", KLM_SERVER_URL_UPLOAD_DESIGN);
        if ((!doc.object().contains("status")) || (doc.object().value("status").toString() != "OK")) {
            upload_flag = false;
        }
    }
    id = klm::info::shared_instance()->plan_id;
    if (id.size() >= 1) {
        doc = klm::network::call_web_file_upload(id, "tmp/design.chd", "design.chp", KLM_SERVER_URL_UPLOAD_PLAN);
        if ((!doc.object().contains("status")) || (doc.object().value("status").toString() != "OK")) {
            upload_flag = false;
        }
    }

    if (!upload_flag) {
        QMessageBox info;
        info.setWindowTitle(tr("save failed"));
        info.setText(tr("save failed"));
        info.exec();
    }
    else {
        QMessageBox info;
        info.setWindowTitle(tr("save ok"));
        info.setText(tr("save complete"));
        info.exec();
    }

    // TODO : upload cover now
    doc = klm::network::call_web_file_upload_cover(klm::info::shared_instance()->design_id,
                                                   design_pdn, "icon.jpg", "upload.jpg", KLM_SERVER_URL_UPLOAD_DESIGN_COVER);
    doc = klm::network::call_web_file_upload_cover(klm::info::shared_instance()->plan_id,
                                                   plan_pdn, "icon.jpg", "upload.jpg", KLM_SERVER_URL_UPLOAD_PLAN_COVER);
}

void drawhomewin::new_design() {
    QMessageBox info1;
    info1.setWindowTitle(tr("uploading"));
    info1.setText(tr("uploading"));
    info1.exec();

    QJsonDocument doc;
    QString plan_pdn;
    QString design_pdn;

    // new design
    doc = klm::network::call_web_new(klm::info::shared_instance()->housename,
                                     klm::info::shared_instance()->house_url, KLM_SERVER_URL_NEW_DESIGN);
    int tval = doc.object().find("results").value().toArray()[0].toObject().value("resource_id").toInt();
    klm::info::shared_instance()->design_id = QString::asprintf("%d", tval);
    design_pdn = doc.object().find("results").value().toArray()[0].toObject().value("path").toString();

    // upload plan and design
    bool upload_flag = true;
    QString id;
    id = klm::info::shared_instance()->design_id;
    if (id.size() >= 1) {
        doc = klm::network::call_web_file_upload(id, "tmp/design.chd", "design.chd", KLM_SERVER_URL_UPLOAD_DESIGN);
        if ((!doc.object().contains("status")) || (doc.object().value("status").toString() != "OK")) {
            upload_flag = false;
        }
    }

    if (!upload_flag) {
        QMessageBox info;
        info.setWindowTitle(tr("save failed"));
        info.setText(tr("save failed"));
        info.exec();
    }
    else {
        QMessageBox info;
        info.setWindowTitle(tr("save ok"));
        info.setText(tr("save complete"));
        info.exec();
    }

    // upload design cover
    doc = klm::network::call_web_file_upload_cover(klm::info::shared_instance()->design_id,
                                                   design_pdn, "icon.jpg", "upload.jpg", KLM_SERVER_URL_UPLOAD_DESIGN_COVER);
}

void drawhomewin::change_design() {
    QString pdn;

    QMessageBox info1;
    info1.setWindowTitle(tr("uploading"));
    info1.setText(tr("uploading"));
    info1.exec();

    QJsonDocument doc;
    QString plan_pdn;
    QString design_pdn;

    bool upload_flag = true;
    QString id;
    id = klm::info::shared_instance()->design_id;
    if (id.size() >= 1) {
        doc = klm::network::call_web_file_upload(id, "tmp/design.chd", "design.chd", KLM_SERVER_URL_UPLOAD_DESIGN);
        if ((!doc.object().contains("status")) || (doc.object().value("status").toString() != "OK")) {
            upload_flag = false;
        }
    }

    if (!upload_flag) {
        QMessageBox info;
        info.setWindowTitle(tr("save failed"));
        info.setText(tr("save failed"));
        info.exec();
    }
    else {
        QMessageBox info;
        info.setWindowTitle(tr("save ok"));
        info.setText(tr("save complete"));
        info.exec();
    }

    // TODO : upload cover now
    doc = klm::network::call_web_file_upload_cover(klm::info::shared_instance()->design_id,
                                                   design_pdn, "icon.jpg", "upload.jpg",
                                                   KLM_SERVER_URL_UPLOAD_DESIGN_COVER);
}

void drawhomewin::change_plan() {
    QString pdn;

    QMessageBox info1;
    info1.setWindowTitle(tr("uploading"));
    info1.setText(tr("uploading"));
    info1.exec();

    QJsonDocument doc;
    QString plan_pdn;
    QString design_pdn;

    bool upload_flag = true;
    QString id;
    id = klm::info::shared_instance()->plan_id;
    if (id.size() >= 1) {
        doc = klm::network::call_web_file_upload(id, "tmp/design.chd", "design.chp", KLM_SERVER_URL_UPLOAD_PLAN);
        if ((!doc.object().contains("status")) || (doc.object().value("status").toString() != "OK")) {
            upload_flag = false;
        }
    }

    if (!upload_flag) {
        QMessageBox info;
        info.setWindowTitle(tr("save failed"));
        info.setText(tr("save failed"));
        info.exec();
    }
    else {
        QMessageBox info;
        info.setWindowTitle(tr("save ok"));
        info.setText(tr("save complete"));
        info.exec();
    }

    // upload plan cover
    doc = klm::network::call_web_file_upload_cover(klm::info::shared_instance()->plan_id,
                                                   plan_pdn, "icon.jpg", "upload.jpg", KLM_SERVER_URL_UPLOAD_PLAN_COVER);
}
