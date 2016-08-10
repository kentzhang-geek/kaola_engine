#ifndef IOUTILITY_H
#define IOUTILITY_H

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <pugixml.hpp>
#include <string>
#include <sstream>
#include <QVector>

using namespace std;

namespace klm{

class IOUtility
{
public:
    IOUtility() = default;
    ~IOUtility() = default;

    static void writeMatrix(pugi::xml_node node, const glm::mat4 &matrix, const string tag = "");
    static void readMatrix(const pugi::xml_node node, glm::mat4 &matrix);
    static void writeVector(pugi::xml_node node, const glm::vec3 &vector, const string tag = "");
    static void readVector(const pugi::xml_node &node, glm::vec3 &vecto);
    static void writeIndicies(pugi::xml_node &node, const QVector<GLushort> &indicies, const string tag);
    static std::string readIndicies(const pugi::xml_node &node, QVector<GLushort> &indicies);


    static const string MATRIX;
    static const string USAGE;
    static const string VECTOR;
    static const int MATRIX_SIZE;
    static const int VECTOR_SIZE;
    static const string SURFACE;
    static const string NUM_OF_INDICIES;
    static const string INDEX;
    static const string VERTEX;
    static const string X;
    static const string Y;
    static const string Z;
    static const string W;
    static const string H;
    static const string VISIBLE;
    static const string SURFACE_MATEIRAL;
    static const string CONNECTIVE_MATERIAL;
    static const string TRANSFORM_FROM_PPARENT;
    static const string SURFACE_SCALE;
    static const string SURFACE_TRANSLATE;
    static const string SURFACE_ROTATION;
    static const string LOCAL_VERTIICES;
    static const string LOCAL_INDICES;
    static const string RENDERING_VERTICIES;
    static const string RENDERING_INDICES;
    static const string CONNECTIVE_VERTICIES;
    static const string CONNECTIVE_INDICES;

private:
    static void writeFloatsToNode(pugi::xml_node &node,
                                  const GLfloat* data,
                                  const unsigned int size);

    static void readFloatsFromNode(const pugi::xml_node &node,
                                   const std::string &targetName,
                                   QVector<GLfloat> &ret);

    static const string VALUE_TOKEN;

};

}

#endif // IOUTILITY_H
