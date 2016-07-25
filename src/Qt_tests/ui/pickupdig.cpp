#include "PickupDig.h"

PickupDig::PickupDig(QWidget* parent, int x, int y)
{
    this->setParent(parent);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,248,255));
    this->setPalette(palette);

    //定义窗口坐标
    setGeometry(x + 180, y + 30, 0, 0);
    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    initBasicInfo();                                    //初始化信息窗体

    QVBoxLayout* layout = new QVBoxLayout;              //定义一个垂直布局类实体，QHBoxLayout为水平布局类实体
    layout->addWidget(baseWidget);                      //加入baseWidget
    layout->setSizeConstraint(QLayout::SetFixedSize);   //设置窗体缩放模式，此处设置为固定大小
    layout->setSpacing(6);                              //窗口部件之间间隔大小
    setLayout(layout);                                  //加载到窗体上
}

void PickupDig::initBasicInfo()
{
    baseWidget = new QWidget;                           //实例化baseWidget
    QLabel* nameLabel = new QLabel(tr("Name"));         //定义窗体部件

    QFormLayout* formLayout = new QFormLayout;          //表单布局方法
    QSlider *slider = new QSlider(Qt::Horizontal);      //新建一个水平方向的滑动条QSlider控件
    slider->setMinimum(0);
    slider->setMaximum(100);
    slider->setValue(50);
    formLayout->addRow(nameLabel, slider);

    QVBoxLayout* vboxLayout = new QVBoxLayout;          //窗体顶级布局，布局本身也是一种窗口部件
    vboxLayout->addLayout(formLayout);                  //顶层窗体加入表单
    baseWidget->setLayout(vboxLayout);                  //加载到窗体上
}
