//#include "mainwindow.h"
//#include <QApplication>
//#include <iostream>
//#include "src/utils/qui/myhelper.h"
//#include <QString>
//#include <QMessageBox>
//#include "utils/gl3d_utils.h"
//#include "utils/gl3d_path_config.h"
//
//using namespace std;
//
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//
//    myHelper::SetUTF8Code();
//    myHelper::SetStyle("black");//黑色风格
////    myHelper::SetStyle("blue");//蓝色风格
////    myHelper::SetStyle("gray");//灰色风格
////    myHelper::SetStyle("navy");//天蓝色风格
//    myHelper::SetChinese();
//
//    QTranslator *translator = new QTranslator;
//    translator->load(QString(GL3D_PATH_BASE) + GL3D_PATH_SEPRATOR + "zh.qm");
//    if (translator->isEmpty()) {
//        QMessageBox msgBox(QMessageBox::Critical, "Error",
//                              "zh.qm", 0, NULL);
//        return msgBox.exec();
//    }
//    a.installTranslator(translator);
//
//    MainWindow w;
//    w.show();
//
//    try {
//        return a.exec();
//    }
//    catch (QString err) {
//        QMessageBox msgBox(QMessageBox::Critical, "Error",
//                              err, 0, NULL);
//        return msgBox.exec();
//    }
//}
