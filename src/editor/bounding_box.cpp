#include "editor/bounding_box.h"

using namespace klm;

BoundingBox::BoundingBox(const QVector<glm::vec3> &points){
    initValues();
    for(QVector<glm::vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){
        swapIfLess(*point, xMin, yMin, zMin);
        swapIfGreater(*point, xMax, yMax, zMax);
    }
}

BoundingBox::BoundingBox(const QVector<Vertex*> &vertices){
    initValues();
    for(QVector<Vertex*>::const_iterator vertex = vertices.begin();
        vertex != vertices.end(); ++vertex){
        glm::vec3 point;
        (*vertex)->getPosition(point);
        swapIfLess(point, xMin, yMin, zMin);
        swapIfGreater(point, xMax, yMax, zMax);
    }
}

void BoundingBox::initValues(){
    xMin = numeric_limits<float>::max();
    yMin = numeric_limits<float>::max();
    zMin = numeric_limits<float>::max();
    xMax = numeric_limits<float>::min();
    yMax = numeric_limits<float>::min();
    zMax = numeric_limits<float>::min();
}

void BoundingBox::swapIfLess(const glm::vec3 &vector,
                 GLfloat &x, GLfloat &y, GLfloat &z) const noexcept{
    if(x > vector.x){
        x = vector.x;
    }
    if(y > vector.y){
        y = vector.y;
    }
    if(z > vector.z){
        z = vector.z;
    }
}

void BoundingBox::swapIfGreater(const glm::vec3 &vector,
                    GLfloat &x, GLfloat &y, GLfloat &z) const noexcept{
    if(x < vector.x){
        x = vector.x;
    }
    if(y < vector.y){
        y = vector.y;
    }
    if(z < vector.z){
        z = vector.z;
    }
}

void BoundingBox::genTexture(Vertex &vertex) const noexcept{    
    vertex.setW(vertex.getX() - xMin);
    vertex.setH(vertex.getY() - yMin);
}

void BoundingBox::genTexture(QVector<Vertex*> &verteces) const noexcept{
    for(QVector<Vertex*>::iterator vertex = verteces.begin();
        vertex != verteces.end(); ++vertex){
        genTexture(**vertex);
    }
}

GLfloat BoundingBox::getLength() const noexcept{
    return xMax - xMin;
}

GLfloat BoundingBox::getHeight() const noexcept{
    return yMax - yMin;
}

GLfloat BoundingBox::getWidth() const noexcept{
    return zMax - zMin;
}

glm::vec3 BoundingBox::getCenter() const noexcept{
    return glm::vec3((xMin + xMax)/2,
                (yMin + yMax)/2,
                (zMin + zMax)/2);
}

GLfloat BoundingBox::getXMax() const noexcept{
    return xMax;
}


GLfloat BoundingBox::getXMin() const noexcept{
    return xMin;
}

GLfloat BoundingBox::getYMax() const noexcept{
    return yMax;
}


GLfloat BoundingBox::getYMin() const noexcept{
    return yMin;
}

GLfloat BoundingBox::getZMax() const noexcept{
    return zMax;
}


GLfloat BoundingBox::getZMin() const noexcept{
    return zMin;
}
