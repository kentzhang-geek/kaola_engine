#ifndef DRAWOPTION_H
#define DRAWOPTION_H

#include <iostream>

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "utils/gl3d_global_param.h"

using namespace std;

class DrawOption:public QDialog
{
    Q_OBJECT
public:
    DrawOption(QWidget* parent);
    void initBasicInfo();
private slots:
    void proxyChange(const QString &text);
private:
    QWidget* baseWidget;
};

#endif // DRAWOPTION_H
