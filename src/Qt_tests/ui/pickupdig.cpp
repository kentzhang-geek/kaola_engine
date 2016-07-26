#include "PickupDig.h"

PickupDig::PickupDig(QWidget* parent, int x, int y)
{
    this->setParent(parent);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,248,255));
    this->setPalette(palette);

    //定义窗口坐标
    setGeometry(x + 160, y + 60, 0, 0);
    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    initBasicInfo();                                    //初始化信息窗体

    QVBoxLayout* layout = new QVBoxLayout;              //定义一个垂直布局类实体，QHBoxLayout为水平布局类实体
    layout->addWidget(baseWidget);                      //加入baseWidget
    layout->setSizeConstraint(QLayout::SetFixedSize);   //设置窗体缩放模式，此处设置为固定大小
    layout->setSpacing(6);                              //窗口部件之间间隔大小
    layout->setMargin(5);
    setLayout(layout);                                  //加载到窗体上
}

//墙
void PickupDig::initBasicInfo()
{
    baseWidget = new QWidget;                           //实例化baseWidget

   //控制按钮
    QPushButton *delButton = new QPushButton();
    delButton->setText("删除");
    QPushButton *splitButton = new QPushButton();
    splitButton->setText("拆分");

    //    长度控件
    QLabel *nameLabel = new QLabel(tr("长度"));         //定义窗体部件
    QSlider *slider = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider->setMinimum(1);
    slider->setMaximum(20);
    slider->setValue(10);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setRange(1, 20);
    QLabel *unitLabel = new QLabel(tr("m"));

    //    厚度控件
    QLabel *nameLabel2 = new QLabel(tr("厚度"));         //定义窗体部件
    QSlider *slider2 = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider2->setMinimum(1);
    slider2->setMaximum(20);
    slider2->setValue(10);
    QSpinBox *spinBox2 = new QSpinBox;
    spinBox2->setRange(1, 20);
    QLabel *unitLabel2 = new QLabel(tr("m"));

    //    高度控件
    QLabel *nameLabel3 = new QLabel(tr("高度"));         //定义窗体部件
    QSlider *slider3 = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider3->setMinimum(1);
    slider3->setMaximum(20);
    slider3->setValue(10);
    QSpinBox *spinBox3 = new QSpinBox;
    spinBox3->setRange(1, 20);
    QLabel *unitLabel3 = new QLabel(tr("m"));


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


    QVBoxLayout* vboxLayout = new QVBoxLayout;          //窗体顶级布局，布局本身也是一种窗口部件
    vboxLayout->addLayout(hlayoutButtons);
    vboxLayout->addLayout(hlayout1);
    vboxLayout->addLayout(hlayout2);
    vboxLayout->addLayout(hlayout3);

    baseWidget->setLayout(vboxLayout);                  //加载到窗体上
}
