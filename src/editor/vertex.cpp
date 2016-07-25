#include "editor/vertex.h"

using namespace klm;

Vertex::Vertex(const GLdouble &x, const GLdouble &y, const GLdouble &z,
               const GLdouble w, const GLdouble h) noexcept{
    data = new GLdouble[VERTEX_SIZE];
    data[X] = x;
    data[Y] = y;
    data[Z] = z;
    data[W] = w;
    data[H] = h;
}

Vertex::Vertex(const glm::vec3 &position, const glm::vec2 t) noexcept{
    data = new GLdouble[VERTEX_SIZE];
    data[X] = (GLdouble)position.x;
    data[Y] = (GLdouble)position.y;
    data[Z] = (GLdouble)position.z;
    data[W] = (GLdouble)t.x;
    data[H] = (GLdouble)t.y;
}

Vertex::Vertex(const Vertex &vertex) noexcept{
    data = new GLdouble[VERTEX_SIZE];
    for(int index = 0; index != VERTEX_SIZE; ++index){
        data[index] = vertex.data[index];
    }
}

Vertex::~Vertex(){    
    if(data){
        delete[] data;
    }
}

bool Vertex::equals(const Vertex &another) const noexcept{
    if(this->data == another.data){
        return true;
    } else {
        for(int index = 0; index != VERTEX_SIZE; ++index){
            if(this->data[index] != another.data[index]){
                return false;
            }
        }
        return true;
    }
}

void Vertex::getPosition(glm::vec3 &position) const noexcept{
    position.x = (GLfloat)data[X];
    position.y = (GLfloat)data[Y];
    position.z = (GLfloat)data[Z];
}

void Vertex::setPosition(const glm::vec3 &position) noexcept{
    data[X] = (GLdouble)position.x;
    data[Y] = (GLdouble)position.y;
    data[Z] = (GLdouble)position.z;
}

void Vertex::getTexture(glm::vec2 &texture) const noexcept{
    texture.x = (GLfloat)data[W];
    texture.y = (GLfloat)data[H];
}

void Vertex::setTexture(const glm::vec2 &texture) noexcept{
    data[W] = (GLdouble)texture.x;
    data[H] = (GLdouble)texture.y;
}

GLdouble* Vertex::getData() const noexcept{
    return data;
}

GLdouble Vertex::getX() const{
    return data[X];
}

void Vertex::setX(GLdouble x){
    data[X] = x;
}

GLdouble Vertex::getY() const{
    return data[Y];
}

void Vertex::setY(GLdouble y){
    data[Y] = y;
}

GLdouble Vertex::getZ() const {
    return data[Z];
}

void Vertex::setZ(GLdouble z){
    data[Z] = z;
}

GLdouble Vertex::getW() const{
    return data[W];
}

void Vertex::setW(GLdouble w){
    data[W] = w;
}

GLdouble Vertex::getH() const{
    return data[H];
}

void Vertex::setH(GLdouble h){
    data[H] = h;
}

void Vertex::print() const{
    std::cout<<"Vertex "<<this<<" : ("<<getX()<<","<<getY()<<","<<getZ()<<","<<getW()<<","<<getH()<<")"<<std::endl;
}

const int Vertex::X  = 0;
const int Vertex::Y  = 1;
const int Vertex::Z  = 2;
const int Vertex::W  = 3;
const int Vertex::H  = 4;
const int Vertex::VERTEX_SIZE = 5;
