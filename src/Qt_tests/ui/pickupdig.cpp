#include <include/editor/command.h>
#include "pickupdig.h"
#include "ui_window_or_door_selected.h"
#include "ui_furniture_selected.h"

PickupDig::PickupDig(QWidget *parent, int x, int y, int pickUpObjID, gl3d::scene *sc, klm::design::scheme *sch,
                     glm::vec2 cd_scr) {
    main_scene = sc;
    this->coord_on_screen = cd_scr;
    this->sketch = sch;
    this->pickUpObjID = pickUpObjID;
    this->setParent(parent);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248, 248, 255));
    this->setPalette(palette);

    //定义窗口坐标
    setGeometry(x + 300, y + 60, 0, 0);
    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    //根据拾取到的objid获取模型obj
    pickUpObj = (gl3d::object *) this->main_scene->get_obj(pickUpObjID);
    wall = (gl3d::gl3d_wall *) pickUpObj;

    switch (pickUpObj->get_obj_type()) {
        case abstract_object::type_wall : {
            initBasicInfo();
            initNormalLayout();
            break;
        }
        case abstract_object::type_scheme : {
            initBasicSchemeInfo();
            initNormalLayout();
            break;
        }
        case abstract_object::type_window : {
            Ui_window_or_door_selected * ui = new Ui_window_or_door_selected();
            ui->setupUi(this);
            delete ui;
            break;
        }
        case abstract_object::type_door : {
            Ui_window_or_door_selected * ui = new Ui_window_or_door_selected();
            ui->setupUi(this);
            delete ui;
            break;
        }
        case abstract_object::type_furniture : {
            Ui_furniture_selected * ui = new Ui_furniture_selected();
            ui->setupUi(this);
            ui->rotate_btn->setRange(0, 359);
            float tmp = math::calculate_angle_by_mat(this->pickUpObj->get_property()->rotate_mat);
            if (tmp >= 359.0f)
                tmp = 0.0f;
            ui->rotate_btn->setValue((int) tmp);
            ui->rot_angle->setText(QString::asprintf("%d", (int) tmp));
            ui->cpos_x->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.x));
            ui->cpos_y->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.z));
            delete ui;
            break;
        }
        default: {
            break;
        }
    }

    // pick
    switch (pickUpObj->get_obj_type()) {
        case gl3d::abstract_object::type_wall : {
            gl3d_wall *w = (gl3d_wall *) pickUpObj;
            w->set_is_picked(true);
            break;
        }
        case gl3d::abstract_object::type_scheme : {
            klm::design::scheme *skt = (klm::design::scheme *) pickUpObj;
            gl3d::room *r = skt->get_room(this->coord_on_screen);
            r->set_picked(true);
            break;
        }
        case abstract_object::type_furniture : {
            gl3d::object *o = (gl3d::object *) pickUpObj;
            o->set_pick_flag(true);
        }
        default:
            break;
    }
}

PickupDig::~PickupDig() {
    if (this->pickUpObjID < 0)
        return;

    // pick
    switch (pickUpObj->get_obj_type()) {
        case gl3d::abstract_object::type_wall : {
            gl3d_wall *w = (gl3d_wall *) pickUpObj;
            w->set_is_picked(false);
            break;
        }
        case gl3d::abstract_object::type_scheme : {
            klm::design::scheme *skt = (klm::design::scheme *) pickUpObj;
            gl3d::room *r = skt->get_room(this->coord_on_screen);
            r->set_picked(false);
            break;
        }
        case abstract_object::type_furniture : {
            gl3d::object *o = (gl3d::object *) pickUpObj;
            o->set_pick_flag(false);
        }
        default:
            break;
    }

}

void PickupDig::initNormalLayout() {
    QVBoxLayout *layout = new QVBoxLayout;              //定义一个垂直布局类实体，QHBoxLayout为水平布局类实体
    layout->addWidget(baseWidget);                      //加入baseWidget
    layout->setSizeConstraint(QLayout::SetFixedSize);   //设置窗体缩放模式，此处设置为固定大小
    layout->setSpacing(6);                              //窗口部件之间间隔大小
    layout->setMargin(5);
    setLayout(layout);                                  //加载到窗体上
}

//墙
void PickupDig::initBasicInfo() {
    baseWidget = new QWidget;                           //实例化baseWidget

    //控制按钮
    QPushButton *delButton = new QPushButton();
    delButton->setText("删除");
    connect(delButton, SIGNAL(clicked()), this, SLOT(on_delete_obj()));
    QPushButton *splitButton = new QPushButton();
    splitButton->setText("拆分");

    //    长度控件
    QLabel *nameLabel = new QLabel(tr("长度"));         //定义窗体部件
    slider = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider->setMinimum(0.12 * 100);
    slider->setMaximum(20.00 * 100);
    slider->setValue(wall->get_length() * 100);
    spinBox = new QDoubleSpinBox;
    spinBox->setRange(0.12, 20.00);
    spinBox->setValue(wall->get_length());
    spinBox->setSingleStep(0.01);
    QLabel *unitLabel = new QLabel(tr("M"));
    connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotDoubleSpinbox_Slider()));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSlider_DoubleSpinbox()));

    //    厚度控件
    QLabel *nameLabel2 = new QLabel(tr("厚度"));         //定义窗体部件
    slider2 = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider2->setMinimum(0.01 * 100);
    slider2->setMaximum(1.00 * 100);
    slider2->setValue(wall->get_thickness() * 100);
    spinBox2 = new QDoubleSpinBox;
    spinBox2->setRange(0.01, 1.00);
    spinBox2->setValue(wall->get_thickness());
    spinBox2->setSingleStep(0.01);
    QLabel *unitLabel2 = new QLabel(tr("M"));
    connect(spinBox2, SIGNAL(valueChanged(double)), this, SLOT(slotDoubleSpinbox_Slider2()));
    connect(slider2, SIGNAL(valueChanged(int)), this, SLOT(slotSlider_DoubleSpinbox2()));

    //    高度控件
    QLabel *nameLabel3 = new QLabel(tr("高度"));         //定义窗体部件
    slider3 = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider3->setMinimum(0.10 * 100);
    slider3->setMaximum(2.80 * 100);
    slider3->setValue(wall->get_hight() * 100);
    spinBox3 = new QDoubleSpinBox;
    spinBox3->setRange(0.10, 2.80);
    spinBox3->setValue(wall->get_hight());
    spinBox3->setSingleStep(0.01);
    QLabel *unitLabel3 = new QLabel(tr("M"));
    connect(spinBox3, SIGNAL(valueChanged(double)), this, SLOT(slotDoubleSpinbox_Slider3()));
    connect(slider3, SIGNAL(valueChanged(int)), this, SLOT(slotSlider_DoubleSpinbox3()));

    QHBoxLayout *hlayoutButtons = new QHBoxLayout;
    hlayoutButtons->setContentsMargins(0, 0, 0, 10);
    hlayoutButtons->addWidget(delButton);
    hlayoutButtons->addWidget(splitButton);

    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(nameLabel);
    hlayout1->addWidget(slider);
    hlayout1->addWidget(spinBox);
    hlayout1->addWidget(unitLabel);

    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->addWidget(nameLabel2);
    hlayout2->addWidget(slider2);
    hlayout2->addWidget(spinBox2);
    hlayout2->addWidget(unitLabel2);

    QHBoxLayout *hlayout3 = new QHBoxLayout;
    hlayout3->addWidget(nameLabel3);
    hlayout3->addWidget(slider3);
    hlayout3->addWidget(spinBox3);
    hlayout3->addWidget(unitLabel3);


    QVBoxLayout *vboxLayout = new QVBoxLayout;          //窗体顶级布局，布局本身也是一种窗口部件
    vboxLayout->addLayout(hlayoutButtons);
    vboxLayout->addLayout(hlayout1);
    vboxLayout->addLayout(hlayout2);
    vboxLayout->addLayout(hlayout3);

    baseWidget->setLayout(vboxLayout);                  //加载到窗体上
}

//房间
void PickupDig::initBasicSchemeInfo() {
    baseWidget = new QWidget;                           //实例化baseWidget

    //控制按钮
    QPushButton *delButton = new QPushButton();
    delButton->setText("删除");
    connect(delButton, SIGNAL(clicked()), this, SLOT(on_delete_obj()));
    QPushButton *splitButton = new QPushButton();
    splitButton->setText("清空家具");

    QLabel *lbl_caption = new QLabel(tr("RoomName"));
    cbo = new QComboBox();
    cbo->addItem(QWidget::tr("Unname"));
    cbo->addItem(QWidget::tr("keting"));
    cbo->addItem(QWidget::tr("cangting"));
    cbo->addItem(QWidget::tr("zhuwo"));
    cbo->addItem(QWidget::tr("ciwo"));
    cbo->addItem(QWidget::tr("shufang"));
    cbo->addItem(QWidget::tr("chufang"));
    cbo->addItem(QWidget::tr("weishengjian"));
    cbo->addItem(QWidget::tr("yantai"));
    cbo->addItem(QWidget::tr("chuwujian"));

    // calculate current index
    if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_scheme) {
        // TODO : 房间命名
        gl3d::room * r = this->sketch->get_room(this->coord_on_screen);
        QVector<QString> vss;
        vss.push_back(QWidget::tr("Unname"));
        vss.push_back(QWidget::tr("keting"));
        vss.push_back(QWidget::tr("cangting"));
        vss.push_back(QWidget::tr("zhuwo"));
        vss.push_back(QWidget::tr("ciwo"));
        vss.push_back(QWidget::tr("shufang"));
        vss.push_back(QWidget::tr("chufang"));
        vss.push_back(QWidget::tr("weishengjian"));
        vss.push_back(QWidget::tr("yantai"));
        vss.push_back(QWidget::tr("chuwujian"));
        for (int i = 0; i < vss.size(); i++) {
            if (vss.at(i) == QString::fromStdString(r->name))
                cbo->setCurrentIndex(i);
        }
    }

    // connect cbo change evnets
    connect(cbo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            [=](const QString &text) {
                if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_scheme) {
                    // TODO : 房间命名
                    gl3d::room *r = this->sketch->get_room(this->coord_on_screen);
                    r->name = text.toStdString();
                }
            });

    QHBoxLayout *hlayoutButtons = new QHBoxLayout;
    hlayoutButtons->setContentsMargins(0, 0, 0, 10);
    hlayoutButtons->addWidget(delButton);
    hlayoutButtons->addWidget(splitButton);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(lbl_caption);
    hlayout->addWidget(cbo);


    QVBoxLayout *vboxLayout = new QVBoxLayout;          //窗体顶级布局，布局本身也是一种窗口部件
    vboxLayout->addLayout(hlayoutButtons);
    vboxLayout->addLayout(hlayout);
    baseWidget->setLayout(vboxLayout);                  //加载到窗体上
}








void PickupDig::slotDoubleSpinbox_Slider() {
    slider->setValue((int) (spinBox->value() * 100));

    //改变墙长度
    klm::command::command_stack::shared_instance()->push(new klm::command::set_wall_property(this->wall));
    wall->set_length(float(spinBox->value()));
    wall->calculate_mesh();
    this->sketch->recalculate_rooms();
}

void PickupDig::slotSlider_DoubleSpinbox() {
    spinBox->setValue((double) (slider->value()) / 100);
}

void PickupDig::slotDoubleSpinbox_Slider2() {
    slider2->setValue((int) (spinBox2->value() * 100));

    //改变墙厚度
    klm::command::command_stack::shared_instance()->push(new klm::command::set_wall_property(this->wall));
    wall->set_thickness(float(spinBox2->value()));
    wall->calculate_mesh();
}

void PickupDig::slotSlider_DoubleSpinbox2() {
    spinBox2->setValue((double) (slider2->value()) / 100);
}

void PickupDig::slotDoubleSpinbox_Slider3() {
    slider3->setValue((int) (spinBox3->value() * 100));

    //改变墙高度
    klm::command::command_stack::shared_instance()->push(new klm::command::set_wall_property(this->wall));
    wall->set_hight(float(spinBox3->value()));
    wall->calculate_mesh();
}

void PickupDig::slotSlider_DoubleSpinbox3() {
    spinBox3->setValue((double) (slider3->value()) / 100);
}


//删除拾取的obj对象
void PickupDig::on_delete_obj() {
    if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_wall) {
        //删除墙
        this->sketch->del_wal((gl3d_wall *) pickUpObj);
    }
    if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_scheme) {
        // TODO : 删除房间
        gl3d::room * r = this->sketch->get_room(this->coord_on_screen);
        this->sketch->delete_room(r);
    }
    this->pickUpObj = NULL;
    this->pickUpObjID = -1;
    delete this;
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}

// delete window or door
void PickupDig::on_del_window_or_wall_clicked() {
    if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_window) {
        //del window
        this->sketch->del_window((gl3d_window *) pickUpObj);
    }
    if (pickUpObj->get_obj_type() == abstract_object::type_door) {
        // del door
        this->sketch->del_door((gl3d_door *) pickUpObj);
    }
    this->pickUpObj = NULL;
    this->pickUpObjID = -1;
    delete this;
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}

void PickupDig::on_rotate_btn_valueChanged(int value) {
    this->pickUpObj->get_property()->rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians((float )value),
                                                              glm::vec3(0.0f, 1.0f, 0.0f));
    QLineEdit * lne = this->findChild<QLineEdit *>("rot_angle");
    lne->setText(QString::asprintf("%d", value));
    qDebug("rotate %d degree", value);
}

void PickupDig::on_delete_btn_clicked() {
    this->sketch->del_furniture(this->pickUpObj);
    this->pickUpObj = NULL;
    this->pickUpObjID = -1;
    delete this;
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
    qDebug("delete furniture btn");
}

void PickupDig::on_rot_angle_editingFinished() {
    QLineEdit * lne = this->findChildren<QLineEdit *>("rot_angle").first();
    QString num = lne->text();
    bool isnum;
    int rot_degree = num.toInt(&isnum);
    rot_degree = rot_degree % 360;
    if (isnum) {
        this->pickUpObj->get_property()->rotate_mat = glm::rotate(glm::mat4(1.0f), (float)rot_degree, glm::vec3(0.0f, 1.0f, 0.0f));
        QDial * dia = this->findChild<QDial *>("rotate_btn");
        dia->setValue(rot_degree);
    }
    else {
        float deg = math::calculate_angle_by_mat(this->pickUpObj->get_property()->rotate_mat);
        if (deg >= 359.0f)
            deg = 0.0f;
        lne->setText(QString::asprintf("%d", (int) deg));
    }
}

void PickupDig::on_cpos_x_editingFinished() {
    QLineEdit * cpos_x = this->findChild<QLineEdit *>("cpos_x");
    bool isnum;
    float posx;
    posx = cpos_x->text().toFloat(&isnum);
    if (isnum) {
        this->pickUpObj->get_property()->position.x = posx;
    }
    else {
        cpos_x->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.x));
    }
}

void PickupDig::on_cpos_y_editingFinished() {
    QLineEdit * cpos_y = this->findChild<QLineEdit *>("cpos_y");
    bool isnum;
    float posy;
    posy = cpos_y->text().toFloat(&isnum);
    if (isnum) {
        this->pickUpObj->get_property()->position.z = posy;
    }
    else {
        cpos_y->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.z));
    }
}

void PickupDig::on_trans_x_editingFinished() {
    QLineEdit *value_ln = this->findChild<QLineEdit *>("trans_x");
    QLineEdit *show_ln = this->findChild<QLineEdit *>("cpos_x");
    bool isnum;
    float val;
    val = value_ln->text().toFloat(&isnum);
    if (isnum) {
        this->pickUpObj->get_property()->position.x += val;
        show_ln->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.x));
    }
    value_ln->clear();
}

void PickupDig::on_trans_y_editingFinished() {
    QLineEdit *value_ln = this->findChild<QLineEdit *>("trans_y");
    QLineEdit *show_ln = this->findChild<QLineEdit *>("cpos_y");
    bool isnum;
    float val;
    val = value_ln->text().toFloat(&isnum);
    if (isnum) {
        this->pickUpObj->get_property()->position.z += val;
        show_ln->setText(QString::asprintf("%f", this->pickUpObj->get_property()->position.z));
    }
    value_ln->clear();
}

void PickupDig::on_test_window_or_door_clicked() {
    if (this->pickUpObj->get_obj_type() == abstract_object::type_door) {
        gl3d_door * d = (gl3d_door *) this->pickUpObj;
        d->change_model("000002");
    }
    else if (this->pickUpObj->get_obj_type() == abstract_object::type_window) {
        gl3d_window * w = (gl3d_window *) this->pickUpObj;
        w->change_model("000002");
    }
}