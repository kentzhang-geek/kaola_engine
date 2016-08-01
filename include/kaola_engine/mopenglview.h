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
#include <QMap>
#include <queue>
#include <QOpenGLContext>
#include <QTimer>
#include <QOpenGLFunctions_4_1_Core>
#include <QGLWidget>
#include <QResource>
#include <QFile>

#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_render_process.hpp"
#include "utils/gl3d_global_param.h"

#include "../src/Qt_tests/drawhomewin.h"
#include "../src/Qt_tests/ui/pickupdig.h"

// OpenGL in Win
#include "glheaders.h"

// gl3d
#include "gl3d.hpp"

// test draw wall
#include "editor/gl3d_wall.h"

//yananli includes ----------------------------------------
#include <QWheelEvent>

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
    void keyPressEvent(QKeyEvent *event);
    void resizeGL(int width, int height);

    //yananli codes -----------------------------------------------------
    enum button_state {
        clear = 0,
        drawwall = 1
    };
    gl3d::gl3d_wall * new_wall;


    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
public slots:
    void view_change();
private:
    string res_path;
    QTimer * timer;
    QTimer * keyTimer;
    char key_press;
    int tmp_point_x, tmp_point_y;
    int wall_temp_id;
    PickupDig *puDig;
    int pickUpObjID;

    void openglDrawWall(const int x, const int y);
    void getWalls();
};

#endif // MOPENGLVIEW_H
