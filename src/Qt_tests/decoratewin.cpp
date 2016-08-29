#include "decoratewin.h"
#include "ui_decoratewin.h"

decoratewin::decoratewin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::decoratewin)
{
    ui->setupUi(this);
}

decoratewin::~decoratewin()
{
    delete ui;
}
