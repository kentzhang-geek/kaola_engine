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
  *
  * the structure of this class is given as below
  *
  * class{
  *     public methods for function 1
  *     private members for function 1
  *     ....
  *     public members for function n
  *     private members for function n
  *
  *     ----
  *
  *     private inner classes
  *
  * }
  *
  * and the cpp implementation follow the same order as they are declared in
  * header file
  *
  */

#define TESS_DEBUG false
#define CONN_DEBUG false

#include <QMap>

#include <string>
#include "kaola_engine/glheaders.h"
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <pugixml.hpp>

#include "utils/io_utility.h"
#include "kaola_engine/gl3d_abstract_object.h"
#include "editor/bounding_box.h"
#include "editor/gl_utility.h"
#include "merchandise.h"

namespace bg = boost::geometry;
typedef bg::model::point<float, 2, bg::cs::cartesian> bg_Point;
typedef bg::model::polygon<bg_Point, false, false> bg_Polygon;

namespace klm{

    class SurfaceException final{
    public:
        SurfaceException(const std::string &message);
        ~SurfaceException() = default;
        std::string what() const;
    private:
        std::string message;
    };

    class Surface final{
    public:
        class Vertex;
        class BoundingBox;
    private:
        //this constructor is used only when loading Surface from file
        Surface(const Surface* parent = nullptr);
    //public methods defined by Surface
    public:
        Surface(const QVector<glm::vec3> &points, const Surface* parent = nullptr) throw(SurfaceException);
        ~Surface();

        //sub-surface APIs
        Surface* addSubSurface(const QVector<glm::vec3> &points);
        int getSurfaceCnt() const;
        Surface* getSubSurface(const unsigned int index);
        bool removeSubSurface(const Surface* subSurface);
        bool isRoot() const;

        //Geometry APIs
        /**
          this method returns the transformation matrix this surface to its
          parent coordinate system.
         * @brief getTransFromParent
         * @param ret
         */
        glm::mat4 getTransformFromParent() const;

        /**
         * this method returns the overall transform this surface to the world
         * coordinate system. it is used for rendering
         * @brief getTransformFromWorld
         * @param ret
         */
        glm::mat4 getRenderingTransform() const;
        /**
         * use this method for original vertex coordinates
         */
        void getLocalVertices(QVector<Vertex*> &localVertices) const;
        /**
         * this method is used for drill hole on parent, should never
         * be called if parent is nullptr
         */
        void getVerticesOnParent(QVector<Vertex*> &verticesOnParent) const;
        /**
         * this method gets coordinate this surface appears on parent system
         */
        void getVerticesToParent(QVector<Vertex*> &verticesToParent) const;

        void setScale(const glm::vec3 &scale);
        void setRotation(const glm::mat4 &rotation);
        void setTranslate(const glm::vec3 &translate);
        glm::mat4 getSurfaceTransform() const;

        bool isConnectiveSurface() const;        

        bool isVisible() const;
        void makeVisible();
        void hide();
        GLfloat getRoughArea();
        GLfloat getPreciseArea();

        const QVector<Surface::Vertex*>* getRenderingVertices();
        const QVector<Surface::Vertex*>* getConnectiveVerticies();
        const QVector<GLushort>* getRenderingIndices();
        const QVector<GLushort>* getConnectiveIndicies();

        /**
         * these methods are used for Surfacing Surface and Connective
         * faces
         */
        Surfacing* getSurfaceMaterial() const;
        void setSurfaceMaterial(Surfacing * surfacing);
        Surfacing* getConnectiveMaterial() const;
        void setConnectiveMateiral(Surfacing* connective);

        void addFurniture(Furniture* furniture);
        int removeFurniture(const std::string &pickID);
        Furniture* getFurniture(const std::string &pickID);

        /**
         * Saveing and loading Surface from pugi node
         * properties to be saved and loaded from xml
         * 1- local verticies         
         * 3- transformFromParent
         * 4- visibilities
         * 5- Surfacing
         * 6- connective Surfacing
         * 7- Furnitures (and their transformation)
         * 7- sub-Surfaces
         * 8- scale, rotation and tarnslate for Surface
         *
         * properties generated from saved Surface:(not saved or loaded)
         * 1- renderingVerticies
         * 2- renderingIndices
         * 3- connectiveVerticies
         * 4- connectiveIndicies
         */
        void save(pugi::xml_node &node);
        bool load(const pugi::xml_node &node);

    private:
        void updateSurfaceMesh();
        void updateConnectionMesh();

        QVector<Surface::Vertex*> *connectiveVertices;
        QVector<GLushort> *connectiveIndices;

    private:
        bg_Polygon * parentialShape;
        bg_Polygon * independShape;
        BoundingBox* boundingBox;
        const Surface* parent;

        bool visible;
        QVector<Surface*> *subSurfaces;
        QVector<Surface::Vertex*> *localVerticies;

        glm::mat4* transformFromParent;

        /**
         * below properties are used for additinal transformation
         * after local verticies has been transformed to parential
         * verticies so that a sub-surface can be transformed even if
         * it is added to parent
         */
        glm::vec3* scale;
        glm::mat4* rotation;
        glm::vec3* translate;

        Surfacing* surfaceMaterial;
        Surfacing* connectiveMaterial;
        QMap<std::string, Furniture*> *attachedFurniture;

    //methods for tessellation
    private:
        static bool tesselate(Surface* surface);
        static void tessCombine(GLdouble coords[3],
                                GLdouble *vertex_data[4],
                                GLfloat weight[4], GLdouble **dataOut);
        static void tessBegin(GLenum type);
        static void tessVertex(const GLvoid *data);
        static void tessEnd();
        static void tessEdge();
        static void tessError(GLenum type);        
    private:
        static GLUtesselator *tess;
        static Surface* targetSurface;
        QVector<Surface::Vertex*> *renderingVerticies;
        QVector<GLushort> *renderingIndicies;


    // utility methods (static)
    public:        
        static const glm::vec3 NON_NORMAL;
        static const glm::vec3 Z_AXIS;

        static bool equalVecr(const glm::vec3 &v1, const glm::vec3 &v2);
        static glm::vec3 getNormal(const Surface::Vertex &p1,
                                   const Surface::Vertex &p2,
                                   const Surface::Vertex &p3);
        static glm::vec3 getPlanNormal(const QVector<glm::vec3> &points);
        static void getRotation(const glm::vec3 &source,
                                const glm::vec3 &destation,
                                glm::mat4 &result);
        static void transformVertex(const glm::mat4 &trans, Surface::Vertex &vertex);
        static void deleteVerticies(QVector<Surface::Vertex*> *vertices);        

    private:
        static void writeVerticies(pugi::xml_node &node, const QVector<Surface::Vertex*> &verticies, const string tag = "");
        static bool readVerticies(const pugi::xml_node &node, QVector<Surface::Vertex*> *ret);
        static void vertexLogger(const QVector<Surface::Vertex*> &verticies,
                                 const QVector<GLushort> &indicies,
                                 const std::string title);
    };

    //this class is used to preserve coordinate of a Surface;
    class Surface::Vertex final{
        public:
            static const int X;
            static const int Y;
            static const int Z;
            static const int W;
            static const int H;
            static const int SIZE;

            Vertex();
            Vertex(const Surface::Vertex &another);
            Vertex(const glm::vec3 &point);
            Vertex(GLdouble x, GLdouble y, GLdouble z, GLdouble w = 0, GLdouble h = 0);
            ~Vertex();

            GLdouble x() const;
            void x(GLdouble x);
            GLdouble y() const;
            void y(GLdouble y);
            GLdouble z() const;
            void z(GLdouble z);
            GLdouble w() const;
            void w(GLdouble w);
            GLdouble h() const;
            void h(GLdouble h);

            bool equals(const Surface::Vertex &another) const;
            GLfloat distance(const Surface::Vertex &another) const;
            void getPosition(glm::vec3 &position) const;
            void setPosition(const glm::vec3 &position);
            void getTexture(glm::vec2 &texture) const;
            void setTexture(const glm::vec2 &texture);
            GLdouble* getData();
        private:
            GLdouble* data;
    };

        //this class is the rounding for the surface
    class Surface::BoundingBox final{
        public:
            BoundingBox(const QVector<glm::vec3> &vertices);
            BoundingBox(const QVector<Surface::Vertex*> &vertices);
            ~BoundingBox() = default;
            glm::vec3 getCenter() const;
            void generateTexture(Surface::Vertex &vertex) const;
            void generateTexture(const QVector<Surface::Vertex*> &verticies) const;
            //reset this bounding box centers to world origin
            void reset();
        private:
            void init();
            void setMinimal(const glm::vec3 &vertex);
            void setMaximal(const glm::vec3 &vertex);
        private:
            GLdouble xMin, xMax, yMin, yMax, zMin, zMax;
    };

}


#endif // SURFACE_H
