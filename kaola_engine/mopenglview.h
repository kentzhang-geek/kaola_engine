#ifndef MOPENGLVIEW_H
#define MOPENGLVIEW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGLWidget>
#include <QOpenGLWidget>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <QOpenGLContext>
#include <QTimer>
#include <QOpenGLFunctions_4_1_Core>
#include <QGLWidget>
#include <QResource>
#include <QFile>

// OpenGL in Win
#include "glheaders.h"

// gl3d
#include "gl3d.hpp"

using namespace std;

class MOpenGLView : public QGLWidget, QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    gl3d::scene * main_scene;
    void do_init();
    void create_scene();
    MOpenGLView();
    MOpenGLView(QWidget * x);
    void paintGL();
    void initializeGL();
    float time_factor;
    GLfloat angle;
private:
    string res_path;
    QTimer * timer;
};

#endif // MOPENGLVIEW_H
