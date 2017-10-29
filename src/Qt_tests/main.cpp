#include <QApplication>
#include <iostream>
#include "src/utils/qui/myhelper.h"
#include <QString>
#include <QMessageBox>
#include "utils/gl3d_utils.h"
#include "utils/gl3d_path_config.h"
#include "demomainwindow.h"
#include "utils/gl3d_math.h"
#include "utils/gl3d_global_param.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 1) {
        printf("usage : gltest.exe [maxCulledObjects] [maxInstanceNumPerDraw]\n");
        QString num = argv[1];
        gl3d::gl3d_global_param::shared_instance()->maxCulledObjNum = num.toInt();
        num = argv[2];
        gl3d::gl3d_global_param::shared_instance()->maxInsPerDraw = num.toInt();
    }
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

    // test:
    for (auto pit : gl3d::math::cubeVertexsFromBoundry(glm::vec3(-0.5f), glm::vec3(0.5f))) {
        glm::vec3 spheric = gl3d::math::rectCoordToSphericCoord(pit);
        glm::vec3 rect = gl3d::math::sphericCoordToRectCoord(spheric);
        rect.x = 0.0f;
    }

    try {
        DemoMainWindow w;
        w.show();
        return a.exec();
    }
    catch (QString err) {
        QMessageBox msgBox(QMessageBox::Critical, "Error",
                              err, 0, NULL);
        return msgBox.exec();
    }
}
