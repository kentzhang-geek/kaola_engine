#ifndef MOPENGLVIEW_H
#define MOPENGLVIEW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <iostream>
#include <OpenGL/glu.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <QOpenGLContext>
#include <QTimer>
#include <oglplus/all.hpp>
#include <QOpenGLFunctions_4_1_Core>
#include <QGLWidget>

class MOpenGLView : public QGLWidget
{
    oglplus::VertexShader * vt;
public:
    void setCon(bool s);
    MOpenGLView();
    MOpenGLView(QWidget * x);
    void paintGL();
    void initializeGL();
    float time_factor;
    GLfloat angle;
    QTimer * timer;
};

#endif // MOPENGLVIEW_H
