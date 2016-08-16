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
#include <QImage>

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

#include "kaola_engine/gl3d_out_headers.h"
#include "kaola_engine/gl3d_render_process.hpp"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "utils/gl3d_lock.h"
#include "utils/gl3d_math.h"

//yananli includes ----------------------------------------
#include <QWheelEvent>
#include <QVector>
#include <QLabel>
#include <QPixmap>

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
    typedef QPair<glm::vec2, gl3d_wall *> point_wall_pair;
    QVector<point_wall_pair> *wallsPoints;
    float drawhome_x1, drawhome_x2, drawhome_y1, drawhome_y2;
    QLabel *connectDot;
    glm::vec2 drawwall_start_point;
    glm::vec2 drawwall_connect_point;
    bool is_drawwall_start_point;
    bool is_drawwall_connect;
    typedef QPair<glm::vec2, glm::vec2> points_wall;
    QVector<points_wall> *points_for_walls;

    gl3d::gl3d_wall * new_wall;
    gl3d::gl3d_wall * old_wall;
    gl3d::gl3d_wall * new_walla;
    gl3d::gl3d_wall * new_wallb;
    gl3d::gl3d_wall * new_wallc;
    gl3d::gl3d_wall * new_walld;
    gl3d::gl3d_wall * connect_wall;
    gl3d::gl3d_wall * start_connect_wall;

    glm::vec2 draw_start_dot;

    void openglDrawWall(const int x, const int y);
    void getWallsPoint();

    QVector<QLabel> *doorsWindowsImages;

    void set_palette_init(QLabel *l, QPixmap p);

    QLabel *l_door;
};

#endif // MOPENGLVIEW_H
