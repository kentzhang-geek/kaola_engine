#include "drawoption.h"

DrawOption::DrawOption(QWidget* parent) {
    this->setParent(parent);

    //定义窗口坐标
    setGeometry(5, 5, 0, 0);
    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248,248,255));
    this->setPalette(palette);

    initBasicInfo();                                    //初始化信息窗体

    QVBoxLayout* layout = new QVBoxLayout;              //定义一个垂直布局类实体，QHBoxLayout为水平布局类实体
    layout->addWidget(baseWidget);                      //加入baseWidget
    layout->setSizeConstraint(QLayout::SetFixedSize);   //设置窗体缩放模式，此处设置为固定大小
    layout->setSpacing(6);                              //窗口部件之间间隔大小
    layout->setMargin(2);
    setLayout(layout);                                  //加载到窗体上
}

void DrawOption::initBasicInfo()
{
    baseWidget = new QWidget;                           //实例化baseWidget

    QLabel *nameLabel = new QLabel(tr("墙厚"));
    QComboBox *wallThick = new QComboBox();
    wallThick->addItem(QWidget::tr("0.10"));
    wallThick->addItem(QWidget::tr("0.12"));
    wallThick->addItem(QWidget::tr("0.18"));
    wallThick->addItem(QWidget::tr("0.20"));
    wallThick->addItem(QWidget::tr("0.24"));
    QString tmp_qstring = QString("%1").arg(gl3d::gl3d_global_param::shared_instance()->wall_thick);
    wallThick->setCurrentText(tmp_qstring);
    QLabel *unitLabel = new QLabel(tr("M"));
    connect(wallThick, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(proxyChange(const QString &)));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(nameLabel);
    hlayout->addWidget(wallThick);
    hlayout->addWidget(unitLabel);

    baseWidget->setLayout(hlayout);                  //加载到窗体上
}

void DrawOption::proxyChange(const QString &text)
{
    float tmp_float = text.toFloat();
    gl3d::gl3d_global_param::shared_instance()->wall_thick = tmp_float;
}
