#include <QApplication>
#include <iostream>
#include "src/utils/qui/myhelper.h"
#include <QString>
#include <QMessageBox>
#include "utils/gl3d_utils.h"
#include "utils/gl3d_path_config.h"
#include "demomainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTranslator *translator = new QTranslator;
//    translator->load(":/translator/zh.qm");
//    if (translator->isEmpty()) {
//        QMessageBox msgBox(QMessageBox::Critical, "Error",
//                           "zh.qm", 0, NULL);
//        return msgBox.exec();
//    }
//    a.installTranslator(translator);

    myHelper::SetUTF8Code();
    myHelper::SetStyle("black");//黑色风格
//    myHelper::SetStyle("blue");//蓝色风格
//    myHelper::SetStyle("gray");//灰色风格
//    myHelper::SetStyle("navy");//天蓝色风格
    myHelper::SetChinese();

    DemoMainWindow w;
    w.show();

    try {
        return a.exec();
    }
    catch (QString err) {
        QMessageBox msgBox(QMessageBox::Critical, "Error",
                              err, 0, NULL);
        return msgBox.exec();
    }
}
