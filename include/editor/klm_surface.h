#ifndef SURFACE_H
#define SURFACE_H

/**
  *
  *                      _ooOoo_
  *                     o8888888o
  *                     88" . "88
  *                     (| -_- |)
  *                     O\  =  /O
  *                  ____/`---'\____
  *                .'  \\|     |//  `.
  *               /  \\|||  :  |||//  \
  *              /  _||||| -:- |||||-  \
  *              |   | \\\  -  /// |   |
  *              | \_|  ''\---/''  |   |
  *              \  .-\__  `-`  ___/-. /
  *            ___`. .'  /--.--\  `. . __
  *         ."" '<  `.___\_<|>_/___.'  >'"".
  *        | | :  `- \`.;`\ _ /`;.`/ - ` : | |
  *        \  \ `-.   \_ __\ /__ _/   .-` /  /
  *    =====`-.____`-.___\_____/___.-`____.-'======
  *                      `=---='
  *    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  *               佛祖保佑       永无BUG
  *
  * @brief The Surface class is the core class in this system, it is used to
  * represent any surface from Wall, Ceiling, Floor and so on.
  */
#include <string>
#include "kaola_engine/glheaders.h"
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "editor/bounding_box.h"
#include "editor/gl_utility.h"
#include "editor/vertex.h"


namespace bg = boost::geometry;
typedef bg::model::point<float, 2, bg::cs::cartesian> bg_Point;
typedef bg::model::polygon<bg_Point, false, false> bg_Polygon;

using namespace std;

namespace klm{

    class SurfaceException final{
    public:
        SurfaceException(const string &message);
        ~SurfaceException() = default;
        string what() const;
    private:
        string message;
    };


    class Surface final{
    public:
        Surface(const QVector<glm::vec3> &points) throw(SurfaceException);
        ~Surface();
        void getSurfaceVertices(QVector<Vertex*> &localVertices) const;
        void getVerticiesToParent(QVector<Vertex*> &toParent) const;
        void getVerticiesOnParent(QVector<Vertex*> &onParent) const;
        void getTransFromParent(glm::mat4 &transform) const;

        void updateVertices();
        Surface * getParent() const;

        bool getRenderingVertices(GLfloat *&data, int &len) const;
        bool getRenderingIndicies(GLushort *&indecies, int &len) const;

        bool addSubSurface(const QVector<glm::vec3> &points);

        bool isRoot() const;

        int getSubSurfaceCnt() const;
        Surface* getSubSurface(const int index) const;

        void setHeightToParent(const GLfloat &height);
        GLfloat getHeightToParent() const;
        bool isConnectedToParent() const;

        bool getConnectiveVerticies(QVector<Vertex*> &connectiveVertices) const;
        bool getConnectiveIndicies(QVector<GLushort> &connectiveIndicies) const;

        bool isVisible() const;
        void setVisibility(const bool visible);

        void setDebug();
        bool isDebug() const;

        float getRoughArea() const;
        float getExactArea() const;

    public:
        static void deleteVertices(QVector<Vertex*>* vertices);
        static void deleteTessellator();
    private:
        static Surface* targetSurface;
        static void updateRenderingData(Surface* surface);

    //Tessellation properties and methods
    private:
        QVector<Vertex*>  *renderVertices;
        QVector<GLushort> *renderIndicies;

        QVector<Vertex*>  *connectiveVerticies;
        QVector<GLushort> *connectiveIndicies;

        void updateConnectivedData();

        //returns -1 if vertex is not yet added to rendering vertex
        //returns -2 if vertex is null or illegal
        //or the index of the vertex if it exists
        GLushort addRenderingVertex(const Vertex* vertex);

        void setTranslateToParent(const glm::vec3 &translate);

    //Surface Properties
    private:        
//        QVector<Vertex*> *verticesToParent;
        QVector<Vertex*> *localVertices;
        //this transform is used to move this surface
        //to its original coordinates as it passed in for
        //surface construction
        glm::mat4 *transFromParent;
        //this transform makes sure sub-surface will not rotate
        //but able to translate
        glm::vec3 *translateFromParent;
        bool visible;
        BoundingBox *boundingBox;
        //this polygon is stored in surface for collission test
        bg_Polygon *collisionTester;

    //sub surface and related proerties
    private:
        static void tessCombine(GLdouble coords[3],
                                GLdouble *vertex_data[4],
                                GLfloat weight[4], GLdouble **dataOut);
        static void tessBegin(GLenum type);
        static void tessVertex(const GLvoid *data);
        static void tessEnd();
        static void tessEdge();
        static void tessError(GLenum type);
        static GLUtesselator *tess;
    private:
        Surface* parent;
        QVector<Surface*> *subSurfaces;
        bool debug;
    };

}
#endif // SURFACE_H
