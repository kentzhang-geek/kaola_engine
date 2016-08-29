#include "pickupdig.h"

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

    if(pickUpObj->get_obj_type() == gl3d::abstract_object::type_wall) {
        //初始化墙操作窗口
        initBasicInfo();
    } else if(pickUpObj->get_obj_type() == gl3d::abstract_object::type_scheme) {
        initBasicSchemeInfo();
    }



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
    wall->set_length(float(spinBox->value()));
    wall->calculate_mesh();
}

void PickupDig::slotSlider_DoubleSpinbox() {
    spinBox->setValue((double) (slider->value()) / 100);
}

void PickupDig::slotDoubleSpinbox_Slider2() {
    slider2->setValue((int) (spinBox2->value() * 100));

    //改变墙厚度
    wall->set_thickness(float(spinBox2->value()));
    wall->calculate_mesh();
}

void PickupDig::slotSlider_DoubleSpinbox2() {
    spinBox2->setValue((double) (slider2->value()) / 100);
}

void PickupDig::slotDoubleSpinbox_Slider3() {
    slider3->setValue((int) (spinBox3->value() * 100));

    //改变墙高度
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
    if (pickUpObj->get_obj_type() == gl3d::abstract_object::type_scheme) {
        // TODO ： 删除房间
        this->sketch->get_room(this->coord_on_screen);
    } else {
        this->main_scene->delete_obj(pickUpObjID);
        delete pickUpObj;
    }
    delete this;
    gl3d::gl3d_global_param::shared_instance()->current_work_state = gl3d::gl3d_global_param::normal;
}
